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

#ifndef _csl_common_queue_impl_hh_included
#define _csl_common_queue_impl_hh_included

/**
    @file queue_impl.hh
    @brief TODO: complete description
*/

/* this is to be included in queue.hh */

#ifdef __cplusplus

namespace csl
{
  namespace common
  {
    template <typename T> void queue<T>::free_item( item * i )
    {
      lock_helper l(this);
      ENTER_FUNCTION();
      CSL_DEBUG_ASSERT( i != NULL );
      if( i )
      {
        CSL_DEBUGF(L"free item at:%lld [next_:%lld]",i->pos_,i->next_);
        items_.free_at( i->pos_ );
      }
      LEAVE_FUNCTION();
    }

    template <typename T> T * queue<T>::append_item( item * i, iterator_t & it )
    {
      ENTER_FUNCTION();
      T * ret = 0;
      CSL_DEBUG_ASSERT( i != NULL );
      if( i )
      {
        CSL_DEBUGF(L"head_:%p tail_:%p n_items_:%lld size:%lld",head_,tail_,n_items_,size());

        i->pos_       = it.get_pos();
        i->next_      = 0;
        ret           = &(i->item_);

        CSL_DEBUGF(L"append_at(i->pos_:%lld) ret:%p",i->pos_,ret);

        if( tail_ == 0 )
        {
          CSL_DEBUGF(L"removing first item");
          CSL_DEBUG_ASSERT( head_ == 0 );
          head_ = tail_ = i;
        }
        else
        {
          tail_->next_  = i->pos_;
          tail_         = i;
          tail_->next_  = 0;
        }
        ++n_items_;
      }
      RETURN_FUNCTION( ret );
    }

    template <typename T>
    typename queue<T>::handler & queue<T>::handler::operator=(handler & other)
    {
      q_   = other.q_;
      i_   = other.i_; other.i_ = 0; // !!!
      return *this;
    }

    template <typename T> T * queue<T>::handler::operator->() const
    {
      if( i_ ) return (&(i_->item_));
      else     return 0;
    }

    template <typename T> T * queue<T>::handler::get() const
    {
      if( i_ ) return (&(i_->item_));
      else     return 0;
    }

    template <typename T> void queue<T>::handler::set( queue * q, item * i )
    {
      reset();
      q_ = q;
      i_ = i;
    }

    template <typename T> void queue<T>::handler::reset()
    {
      if( i_ != NULL && q_ != NULL )
      {
        q_->free_item( i_ );
        i_ = 0;
        q_ = 0;
      }
    }

    template <typename T> T * queue<T>::push(const T & t)
    {
      T * ret = 0;
      {
        lock_helper l(this);
        typename inpvec<item>::iterator ii(items_.end());
        item * i = (items_.first_free(ii).set(t));
        ret = append_item( i, ii );
      }
      on_new_item();
      return ret;
    }

    template <typename T> bool queue<T>::pop(handler & h)
    {
      ENTER_FUNCTION();
      bool ret = false;
      {
        lock_helper l(this);
        if( head_ )
        {
          CSL_DEBUGF(L"head_:%p tail_:%p n_items_:%lld",head_,tail_,n_items_);
          item * i = head_;

          if( head_ == tail_ ) { head_ = tail_ = 0; } // last item
          else
          {
            head_         = 0; // enforce error if failed
            item * inext  = items_.get_ptr( i->next_ );

            if( inext == NULL ) { THR(common::exc::rs_invalid_state,false); }
            head_ = inext;
          }
          --n_items_;
           h.set(this,i);
          CSL_DEBUGF(L"head_:%p tail_:%p n_items_:%lld",head_,tail_,n_items_);
          ret = true;
        }
        else
        {
          // error, no items in the queue
          CSL_DEBUGF(L"no items in the queue: head_=NULL tail_:%p [n_items_:%lld]",tail_,n_items_);
          ret = false;
        }
      }
      if( ret == true ) on_del_item();
      RETURN_FUNCTION( ret );
    }

    template <typename T> uint64_t queue<T>::n_items()
    {
      uint64_t ret = 0;
      lock_helper l(this);
      ret = n_items_;
      return ret;
    }

    template <typename T> uint64_t queue<T>::size()
    {
      uint64_t ret = 0;
      lock_helper l(this);
      ret = items_.n_items();
      return ret;
    }
  }
}
#endif /* __cplusplus */
#endif /* _csl_common_queue_impl_hh_included */
