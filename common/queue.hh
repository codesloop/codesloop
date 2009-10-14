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
      public:
        class handler;
        friend class handler;

      private:
        class lock_helper
        {
          private:
            queue * q_;
          public:
            lock_helper(queue * q) : q_(q) { q_->on_lock_queue();   }
            ~lock_helper()                 { q_->on_unlock_queue(); }
        };

        struct item
        {
          T         item_;
          uint64_t  pos_;   // reference itself (not strictly needed)
          uint64_t  next_;

          item() : pos_(0xBadBeafFeedBaccULL), next_(0xFeedBadBeefBaccULL) { }

          item(const item & other) :
            item_(other), pos_(0xBadBeafFeedBaccULL), next_(0xFeedBadBeefBaccULL) { }

          template <typename T1>
          item(const T1 & t1) :
            item_(t1), pos_(0xBadBeafFeedBaccULL), next_(0xFeedBadBeefBaccULL) { }

          template <typename T1, typename T2>
          item(const T1 & t1, const T2 & t2) :
            item_(t1, t2), pos_(0xBadBeafFeedBaccULL), next_(0xFeedBadBeefBaccULL) { }

          template <typename T1, typename T2, typename T3>
          item(const T1 & t1, const T2 & t2, const T3 & t3) :
            item_(t1, t2, t3), pos_(0xBadBeafFeedBaccULL), next_(0xFeedBadBeefBaccULL) { }

          template <typename T1, typename T2, typename T3, typename T4>
          item(const T1 & t1, const T2 & t2, const T3 & t3, const T4 & t4) :
            item_(t1, t2, t3, t4), pos_(0xBadBeafFeedBaccULL), next_(0xFeedBadBeefBaccULL) { }
        };

        inpvec<item>  items_;
        item *        head_;
        item *        tail_;
        uint64_t      n_items_;

        typedef typename inpvec<item>::iterator iterator_t;

        void free_item( item * i );
        T * append_item( item * i, iterator_t & it );

      public:
        queue() : head_(0), tail_(0), n_items_(0), use_exc_(false) {}
        virtual ~queue() {}

        class handler
        {
          public:
            explicit handler(queue * q=0, item * i=0) : q_(q), i_(i) { }
            ~handler() { reset(); }
            handler & operator=(handler & other);
            T * operator->() const;
            T * get() const;

          private:
            friend class queue;
            handler(const handler & other) : q_(0), i_(0) {} // enforce error

            void set( queue * q, item * i );
            void reset();

            queue *   q_;
            item  *   i_;
        };

        T * push(const T & t);

        template <typename T1>
        T * push(const T1 & t1)
        {
          T * ret = 0;
          {
            lock_helper l(this);
            typename inpvec<item>::iterator ii(items_.end());
            item * i = (items_.first_free(ii).set(t1));
            ret = append_item( i, ii );
          }
          on_new_item();
          return ret;
        }

        template <typename T1,typename T2>
        T * push(const T1 & t1,const T2 & t2)
        {
          T * ret = 0;
          {
            lock_helper l(this);
            typename inpvec<item>::iterator ii(items_.end());
            item * i = (items_.first_free(ii).set(t1,t2));
            ret = append_item( i, ii );
          }
          on_new_item();
          return ret;
        }

        template <typename T1,typename T2,typename T3>
        T * push(const T1 & t1,const T2 & t2,const T3 & t3)
        {
          T * ret = 0;
          {
            lock_helper l(this);
            typename inpvec<item>::iterator ii(items_.end());
            item * i = (items_.first_free(ii).set(t1,t2,t3));
            ret = append_item( i, ii );
          }
          on_new_item();
          return ret;
        }

        template <typename T1,typename T2,typename T3,typename T4>
        T * push(const T1 & t1,const T2 & t2,const T3 & t3,const T4 & t4)
        {
          T * ret = 0;
          {
            lock_helper l(this);
            typename inpvec<item>::iterator ii(items_.end());
            item * i = (items_.first_free(ii).set(t1,t2,t3,t4));
            ret = append_item( i, ii );
          }
          on_new_item();
          return ret;
        }

        bool pop(handler & h);
        uint64_t n_items();  ///<returns the number of active items
        uint64_t size();     ///<returns the number of all allocated items

        /** The purpose of new and del upcalls is to provide means for waitable queue.
        *** A waitable queue may use nthread::event or other synchronization facilities depending on the
        *** implementation. The base class however does not implement that for perforance reasons. Upcalls
        *** should be implemented by subclassing this class.
        ***/
        inline virtual void on_new_item() {} ///<event upcall: called when new item is placed into the list
        inline virtual void on_del_item() {} ///<event upcall: called when an item is removed from the active list

        /** The purpose of the lock unlock upcalls is to provide means for a threadsafe version.
        *** The threadsafe version should use a locking facility such as nthread::mutex to synchronize access
        *** to the given class. Upcalls should be implemented by subclassing this class.
        **/
        inline virtual void on_lock_queue() {} ///<event upcall: called when a queue should be locked
        inline virtual void on_unlock_queue() {} ///<event upcall: called when a queue should be unlocked

        CSL_OBJ(csl::common,queue);
        USE_EXC();
    };
  }
}
#endif /* __cplusplus */
#include "codesloop/common/queue_impl.hh"
#endif /* _csl_common_queue_hh_included */
