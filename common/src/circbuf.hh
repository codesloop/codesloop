
/*
Copyright (c) 2008,2009, David Beck

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

#include "exc.hh"
#ifdef __cplusplus
#include "common.h"

namespace csl
{
  namespace common
  {
    template <typename T,int MaxSize> class circbuf
    {
      private:
        struct item
        {
          item * next_;
          item * prev_;
          T *    item_;

          inline ~item() { if(item_) delete item_; }

          inline void link_after( item * it )
          {
            it->next_ = next_;
            it->prev_ = this;
            next_->prev_ = it;
            next_ = it;
          }

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
        inline circbuf() : n_items_(0), size_(0)
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
        }

        inline virtual ~circbuf()
        {
          item * p = head_.next_;

          while( p != &head_ ) { item * pt = p->next_; delete p; p = pt; }

          p = freelist_.next_;
          while( p != &freelist_ ) { item * pt = p->next_; delete p; p = pt; }

          p = preplist_.next_;
          while( p != &preplist_ ) { item * pt = p->next_; delete p; p = pt; }
        }

        inline T & prepare()
        {
          item * ret = freelist_.unlink_before();
          if( ret == 0 )
          {
            if( n_items_ < MaxSize )
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
              if( n_items_ > MaxSize )
              {
                n_items_ = MaxSize;
                on_full();
              }
              on_new_item();
              break;
            }
          }
        }

        inline void rollback(const T & t) // TODO test this
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

        inline T & push()
        {
          ++n_items_;
          item * ret = 0;

          if( n_items_ > MaxSize )
          {
            n_items_ = MaxSize;
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

        inline T & pop()
        {
          item * ret = head_.unlink_before();

          if( n_items_ == 0 )
          {
            ++size_;
            item * x = new item();
            freelist_.link_after(x);
            THR(common::exc::rs_empty,common::exc::cm_circbuf,*(x->item_));
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
            THR(common::exc::rs_empty,common::exc::cm_circbuf,*(x->item_));
          }

          on_del_item();

          return *(ret->item_);
        }

        inline T & newest()
        {
          if( n_items_ == 0 )
          {
            ++size_;
            item * x = new item();
            freelist_.link_after(x);
            THR(common::exc::rs_empty,common::exc::cm_circbuf,*(x->item_));
          }
          return *(head_.next_->item_);
        }

        inline T & oldest()
        {
          if( n_items_ == 0 )
          {
            ++size_;
            item * x = new item();
            freelist_.link_after(x);
            THR(common::exc::rs_empty,common::exc::cm_circbuf,*(x->item_));
          }
          return *(head_.prev_->item_);
        }

        unsigned int n_items() { return n_items_; }
        unsigned int size()    { return size_;    } // TODO test this

        /* event upcalls */
        inline virtual void on_new_item() {}
        inline virtual void on_del_item() {}
        inline virtual void on_full() {}
        inline virtual void on_empty() {}

        inline void use_exc(bool yesno) { use_exc_ = yesno; }
        inline bool use_exc() const     { return use_exc_; }

      private:
        unsigned int  n_items_;
        unsigned int  size_;
        item          head_;
        item          freelist_;
        item          preplist_;
        bool          use_exc_;
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_common_circbuf_hh_included_ */
