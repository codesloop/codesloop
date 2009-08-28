/*
Copyright (c) 2008,2009, CodeSLoop Team

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _csl_common_circbuf_hh_included_
#define _csl_common_circbuf_hh_included_

/**
   @file circbuf.hh
   @brief curcular buffer implementation designed for multithreaded message buffering
 */

#include "exc.hh"
#ifdef __cplusplus
#include "common.h"

namespace csl
{
  namespace common
  {
    /**
    @brief circular buffer

    the purpose of this class is to help implementing message buffers in a multithreaded
    environment. the memory management of the allocated items is handled by this class.

    users of circbuf may push or pop items into and from the circular buffer. the push operation
    may be split into prepare and {commit,rollback} steps. because of the item data is managed
    by circbuf (rather then the users) the multithreaded push may need to lock the item and the queue
    to work properly. here is when the two-phase push come into the picture. the two phases only
    need to lock the item and the queu for a short period of time.

    locking and other threading primitives are not built into circbuf. users are expected to subclass
    circbuf and implement the needed features there. virtual functions are defined to help this.

    the circbuf class uses a double-linked list to store items. it has three lists. one contains the
    active items. an other contains the free items to be reused. the third list contains the items that
    are about to be pushed (under preparation).
     */
    template <typename T,unsigned long long MaxSize> class circbuf : public obj
    {
      CSL_OBJ(csl::common,circuf);
      private:
        /**
        @brief internal list-item type to store the user's items
        */
        struct item
        {
          item * next_;
          item * prev_;
          T *    item_;

          // this is done in circbuf::circbuf() :
          // inline item() : next_(0), prev_(0), item_(0) {}

          inline ~item() { if(item_) delete item_; }

          /**
          @brief add it-item after this
          @param it is the item to be linked into the list
           */
          inline void link_after( item * it )
          {
            it->next_ = next_;
            it->prev_ = this;
            next_->prev_ = it;
            next_ = it;
          }

          /**
          @brief unlink the previous item from the list
           */
          inline item * unlink_before()
          {
            item * ret = prev_;
            if( prev_ == this ) return 0;

            prev_ = prev_->prev_;
            prev_->next_ = this;

            return ret;
          }
        };

      public:
        /** @brief default constructor */
        inline circbuf() : n_items_(0), size_(0), max_(MaxSize)
        {
          head_.next_ = &head_;
          head_.prev_ = &head_;
          head_.item_ = 0;
          freelist_.next_ = &freelist_;
          freelist_.prev_ = &freelist_;
          freelist_.item_ = 0;
          preplist_.next_ = &preplist_;
          preplist_.prev_ = &preplist_;
          preplist_.item_ = 0;
          if( max_ == 0 ) max_ = 1023ULL*4095ULL*1023ULL*0xFFFFFFFF;
        }

        /**
        @brief destructor

        frees all allocated memory from freelist, preplist and active lists
         */
        inline virtual ~circbuf()
        {
          item * p = head_.next_;

          while( p != &head_ ) { item * pt = p->next_; delete p; p = pt; }

          p = freelist_.next_;
          while( p != &freelist_ ) { item * pt = p->next_; delete p; p = pt; }

          p = preplist_.next_;
          while( p != &preplist_ ) { item * pt = p->next_; delete p; p = pt; }
        }

        /**
        @brief the first step of the 2-phase push

        this step allocates and reserves an item before pushing into the active list.
        this first checks if an item is available on the free list. if not then it
        checks if the list is full. if full and item is poped from the end of the list.
        if it is not full and no free item is available then it allocates a new one.
         */
        inline T & prepare()
        {
          item * ret = freelist_.unlink_before();
          if( ret == 0 )
          {
            if( n_items_ < max_ )
            {
              ++size_;
              ret = new item();
              ret->item_ = new T();
            }
            else
            {
              ret = head_.unlink_before();
            }
          }

          preplist_.link_after( ret );
          return *(ret->item_);
        }

        /**
        @brief second step of the 2-phase push
        @param t is the item to be commit-ed

        this step pushes an already reserved item into the active list and also
        signals on_new_item() to tell waiters that a new item is available.
         */
        inline void commit(const T & t)
        {
          item * it = preplist_.next_;
          while( it != &preplist_ )
          {
            if( it->item_ == &t )
            {
              it = it->next_->unlink_before();
              head_.link_after( it );

              ++n_items_;
              if( n_items_ > max_ )
              {
                n_items_ = max_;
                on_full();
              }
              on_new_item();
              break;
            }
          }
        }

        /**
        @brief second step of the 2-phase push
        @param t is the item to be rolled back

        the prepared (reserved) items may need to be rolled back
        when the user want to undo the prepare phase. this may be neccesary
        in case of error or bad input.
         */
        inline void rollback(const T & t)
        {
          item * it = preplist_.next_;
          while( it != &preplist_ )
          {
            if( it->item_ == &t )
            {
              it = it->next_->unlink_before();
              freelist_.link_after( it );
              break;
            }
          }
        }

        /**
        @brief 1-step push

        the one step push may or may not be a good idea in a multithreaded environment.
        this call finds or allocates a suitable item and before returning that to the user
        it signals on_new_item(). this makes it possible that the waiters may receive the
        data before it is actually updated. this may be circumvented by event objects.

        i suggest to use the 2-phase push mechanism in multithreaded environment.
         */
        inline T & push()
        {
          ++n_items_;
          item * ret = 0;

          if( n_items_ > max_ )
          {
            n_items_ = max_;
            ret = head_.unlink_before();
            head_.link_after( ret );
            on_full();
          }
          else
          {
            ret = freelist_.unlink_before();
            if( !ret )
            {
              ++size_;
              ret = new item();
              ret->item_ = new T();
            }
            head_.link_after( ret );
          }
          on_new_item();
          return *(ret->item_);
        }

        /**
        @brief pops an item from the circular buffer

        this removes the oldest item from the list and returns that to the user.
        if no items are in the list an exception is thrown.

        use n_items() to check if there is any item available in the active list.

        the removed items are placed into the freelist.

        on_empty() and on_del_item() may be signaled depending on the state of the
        active list.

        @todo 2-step pop is also needed
         */
        inline T & pop()
        {
          item * ret = head_.unlink_before();

          if( n_items_ == 0 )
          {
            ++size_;
            item * x = new item();
            freelist_.link_after(x);
            THR(common::exc::rs_empty,*(x->item_));
          }

          if( n_items_ == 1 ) { on_empty(); }

          --n_items_;

          if( ret )
          {
            freelist_.link_after( ret );
          }
          else
          {
            ++size_;
            item * x = new item();
            freelist_.link_after(x);
            THR(common::exc::rs_empty,*(x->item_));
          }

          on_del_item();

          return *(ret->item_);
        }

        /**
        @brief returns the newest item on the active list

        this only retrieves the newest item but does not remove from the active list
         */
        inline T & newest()
        {
          if( n_items_ == 0 )
          {
            ++size_;
            item * x = new item();
            freelist_.link_after(x);
            THR(common::exc::rs_empty,*(x->item_));
          }
          return *(head_.next_->item_);
        }

       /**
        @brief returns the oldest item on the active list

        this only retrieves the oldest item but does not remove from the active list
        */
 
        inline T & oldest()
        {
          if( n_items_ == 0 )
          {
            ++size_;
            item * x = new item();
            freelist_.link_after(x);
            THR(common::exc::rs_empty,*(x->item_));
          }
          return *(head_.prev_->item_);
        }

        unsigned long long n_items() { return n_items_; } ///<returns the number of active items
        unsigned long long size()    { return size_;    } ///<returns the number of all allocated items

        /* event upcalls */
        inline virtual void on_new_item() {} ///<event upcall: called when new item is placed into the list
        inline virtual void on_del_item() {} ///<event upcall: called when an item is removed from the active list 
        inline virtual void on_full() {}     ///<event upcall: called when the active list is full
        inline virtual void on_empty() {}    ///<event upcall: called when the active list becomes empty

        inline void use_exc(bool yesno) { use_exc_ = yesno; } ///<sets the exception usage
        inline bool use_exc() const     { return use_exc_; }  ///<checks the exception usage

      private:
        unsigned long long  n_items_;  ///<the number of active items
        unsigned long long  size_;     ///<the number of allocated items
        unsigned long long  max_;      ///<the maximum number of items to be stored
        item                head_;     ///<head of active list
        item                freelist_; ///<head of free list
        item                preplist_; ///<head of preapred (reserved) item list
        bool                use_exc_;  ///<use exceptions?
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_common_circbuf_hh_included_ */
