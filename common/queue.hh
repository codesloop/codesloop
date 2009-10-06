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

#ifndef _csl_common_queue_hh_included
#define _csl_common_queue_hh_included

/**
    @file queue.hh
    @brief TODO: complete description
*/

#include "codesloop/common/inpvec.hh"
#include "codesloop/common/logger.hh"
#include "codesloop/common/hlprs.hh"
#include "codesloop/common/exc.hh"
#include "codesloop/common/common.h"
#ifdef __cplusplus

namespace csl
{
  namespace common
  {
    template <typename T> class queue
    {
      private:
        struct item
        {
          T         item_;
          uint64_t  pos_;   // reference itself (not strictly needed)
          uint64_t  next_;
        };

        inpvec<item>  items_;
        item *        head_;
        item *        tail_;
        uint64_t      n_items_;

        typedef typename inpvec<item>::iterator iterator_t;

        void free_item( item * i )
        {
          if( i ) { items_.free_at( i->pos_ ); }
        }

        T * append_item( item * i, iterator_t & it )
        {
          T * ret = 0;
          if( i )
          {
            i->pos_       = it.get_pos();
            i->next_      = 0;
            ret           = &(i->item_);
            tail_->next_  = i->pos_;

            // first item ?
            if( tail_ == 0 )
            {
              CSL_DEBUG_ASSERT( head_ == 0 );
              head_ = tail_ = i;
            }
          }
          return ret;
        }

      public:
        queue() : head_(0), tail_(0), n_items_(0), use_exc_(true) {}

        class handler
        {
          public:
            handler(queue * q, item * i) : q_(q), i_(i) { }

            ~handler()
            {
              // destructor checks wether it is OK to free the item of not
              if( i_ != NULL )
              {
                q_->free_item( i_ );
                i_ = 0;
                q_ = 0; // enforce error
              }
            }

            // copy modifies the source
            handler(handler & other)
            {
              q_   = other.q_;
              i_   = other.i_; other.i_ = 0; // !!!
            }

            // copy modifies the source
            handler & operator=(handler & other)
            {
              q_   = other.q_;
              i_   = other.i_; other.i_ = 0; // !!!
              return *this;
            }

            T * operator->() const
            {
              if( i_ ) return (&(i_->item_));
              else     return 0;
            }

            T * get() const
            {
              if( i_ ) return (&(i_->item_));
              else     return 0;
            }

          private:
            queue *   q_;
            item  *   i_;

            // no default construction
            handler() : q_(0), i_(0) {}
            // copy modifies the source, otherwise disallowed
            handler(const handler & other) {}
            handler & operator=(const handler & other) { return *this; }
        };

        friend class handler;

        T * push(const T & t)
        {
          typename inpvec<item>::iterator ii;
          item * i = (items_.last_free(ii).set(t));
          return append_item( i, ii );
        }

        template <typename T1>
        T * push(const T1 & t1)
        {
          typename inpvec<item>::iterator ii;
          item * i = (items_.last_free(ii).set(t1));
          return append_item( i, ii );
        }

        template <typename T1,typename T2>
        T * push(const T1 & t1,const T2 & t2)
        {
          typename inpvec<item>::iterator ii;
          item * i = (items_.last_free(ii).set(t1,t2));
          return append_item( i, ii );
        }

        template <typename T1,typename T2,typename T3>
        T * push(const T1 & t1,const T2 & t2,const T3 & t3)
        {
          typename inpvec<item>::iterator ii;
          item * i = (items_.last_free(ii).set(t1,t2,t3));
          return append_item( i, ii );
        }

        template <typename T1,typename T2,typename T3,typename T4>
        T * push(const T1 & t1,const T2 & t2,const T3 & t3,const T4 & t4)
        {
          typename inpvec<item>::iterator ii;
          item * i = (items_.last_free(ii).set(t1,t2,t3,t4));
          return append_item( i, ii );
        }

        handler pop()
        {
          if( head_ )
          {
            item * i = head_;

            if( head_ == tail_ ) { head_ = tail_ = 0; } // last item
            else
            {
              head_         = 0; // enforce error if failed
              item * inext  = items_.get_ptr( i->next_ );

              if( inext == NULL ) { THR(common::exc::rs_invalid_state,handler(0,0)); }
              head_ = inext;
            }
            --n_items_;
            return handler(this,i);
          }
          else
          {
            // error, no items in the queue
            return handler(0,0);
          }
        }

        uint64_t n_items() { return n_items_;            } ///<returns the number of active items
        uint64_t size()    { return items_.n_items();    } ///<returns the number of all allocated items

        /* event upcalls */
        inline virtual void on_new_item() {} ///<event upcall: called when new item is placed into the list
        inline virtual void on_del_item() {} ///<event upcall: called when an item is removed from the active list

        CSL_OBJ(csl::common,queue);
        USE_EXC();
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_common_queue_hh_included */
