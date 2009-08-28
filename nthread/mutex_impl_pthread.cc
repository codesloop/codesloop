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

#include "mutex.hh"
#include "common.h"
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>

/**
  @file mutex_impl_pthread.cc
  @brief pthread implementation of mutex
*/

namespace csl
{
  namespace nthread
  {
    namespace // anonymous
    {
      class scoped_pt_mutex
      {
      public:
        scoped_pt_mutex(pthread_mutex_t * mtx) : mtx_(mtx)
        {
          pthread_mutex_lock(mtx_);
        }
        ~scoped_pt_mutex()
        {
          pthread_mutex_unlock(mtx_);
        }
      private:
        pthread_mutex_t * mtx_;
      };

      extern "C" void mutex_unlock_function_(void * mtx)
      {
        mutex * m = (mutex *)mtx;
        if( m ) m->unlock();
      }
    };

    struct mutex::impl
    {
      bool                invalid_;
      unsigned int        waiting_;
      unsigned int        locked_;
      pthread_t           locked_by_;
      pthread_mutexattr_t mtx_attr_;
      pthread_mutex_t   * mtx_;
      pthread_cond_t    * cond_;
      pthread_mutex_t     mtx_var_;
      pthread_cond_t      cond_var_;
      pthread_key_t       unlock_key_;

      impl() : invalid_(false), waiting_(0), locked_(0), locked_by_(0)
      {
        pthread_mutexattr_init(&mtx_attr_);
        pthread_mutexattr_settype(&mtx_attr_,PTHREAD_MUTEX_NORMAL);
        pthread_mutex_init(&mtx_var_,&mtx_attr_);
        pthread_cond_init(&cond_var_,NULL);
        pthread_key_create(&unlock_key_,mutex_unlock_function_);
        mtx_  = &mtx_var_;
        cond_ = &cond_var_;
      }

      ~impl()
      {
        // lock variables
        pthread_mutex_lock(mtx_);
        locked_  = 0;

        // tell waiting threads to abort
        invalid_ = true;
        if( waiting_ > 0 ) pthread_cond_broadcast(cond_);
        waiting_ = 0;
        locked_  = 0;

        // invalidate original mutex data
        mtx_  = (pthread_mutex_t *)0xbad;
        cond_ = (pthread_cond_t *)0xbad;

        // destroy the allocated pthread structures
        pthread_mutex_unlock(&mtx_var_);
        pthread_mutexattr_destroy(&mtx_attr_);
        pthread_mutex_destroy(&mtx_var_);
        pthread_cond_destroy(&cond_var_);
        pthread_setspecific(unlock_key_,NULL);
        pthread_key_delete(unlock_key_);
      }

      bool is_locked_nl()
      {
        return (locked_ > 0 ? true : false);
      }

      bool is_locked_by_me_nl()
      {
        return (locked_by_ == 0 ? false : pthread_equal(locked_by_,pthread_self()));
      }

      bool lock(unsigned long timeout_ms)
      {
        bool ret = true;

        struct timeval tv;
        struct timespec at;

        if( timeout_ms )
        {
          // this calculation is moved out of the locked section
          gettimeofday(&tv,NULL);
          // calculate future time value
          tv.tv_usec += (timeout_ms*1000);
          at.tv_sec   = tv.tv_sec + (tv.tv_usec/1000000);
          at.tv_nsec  = (tv.tv_usec%1000000)*1000;
        }

        {
          // locked section
          bool timed_out = false;
          int err=0;
          scoped_pt_mutex m(mtx_);

          ++waiting_;

          // about to be destroyed ?
          if( invalid_ ) { ret = false; }
          else if( timeout_ms )
          {
            // locked by someone else?
            while( is_locked_nl() == true && is_locked_by_me_nl() == false )
            {
              if( (err=pthread_cond_timedwait(cond_,mtx_,&at)) == ETIMEDOUT )
              {
                timed_out = true;
                ret = false;
                break;
              }
              else if( err == 0 )
              {
                /* do nothing: the while condition will stop if OK */
              }
              else
              {
                /* other error */
                timed_out = false;
                ret = false;
                break;
              }
            }
          }
          else
          {
            // locked by someone else?
            while( is_locked_nl() == true && is_locked_by_me_nl() == false )
            {
              if( pthread_cond_wait(cond_, mtx_) )
              {
                /* error */
                ret = false;
                break;
              }
            }
          }
          --waiting_;

          // lock timed out successful ?
          if( ret )
          {
            ++locked_;
            locked_by_ = pthread_self();
            pthread_setspecific(unlock_key_,this);
          }
        }
        return ret;
      }

      bool try_lock()
      {
        bool ret = false;
        {
          // locked section
          scoped_pt_mutex m(mtx_);

          // about to be destroyed ?
          if( invalid_ ) { ret = false; }
          else
          {
            // unlocked or locked by me ?
            if( is_locked_nl() == false || is_locked_by_me_nl() == true )
            {
              ++locked_;
              locked_by_ = pthread_self();
              pthread_setspecific(unlock_key_,this);
              ret = true;
            }
          }
        }
        return ret;
      }

      bool unlock()
      {
        bool ret = true;
        {
          // locked section
          scoped_pt_mutex m(mtx_);
          ret = is_locked_nl();

          if( ret == false )
          {
            // not locked ?
            goto bail_not_locked;
          }
          else if( is_locked_by_me_nl() == false )
          {
            // locked by someone else ?
            ret = false;
            goto bail_not_owner;
          }
          --locked_;
          if( locked_ == 0 )
          {
            pthread_setspecific(unlock_key_,NULL);
            locked_by_ = 0;
          }
          if( waiting_ > 0 ) pthread_cond_signal(cond_);
bail_not_locked:
bail_not_owner:
          ;
        }
        return ret;
      }

      bool recursive_unlock()
      {
        bool ret = true;
        {
          // locked section
          scoped_pt_mutex m(mtx_);
          ret = is_locked_nl();

          if( ret == false )
          {
            // not locked ?
            goto bail_not_locked;
          }
          else if( is_locked_by_me_nl() == false )
          {
            // locked by someone else ?
            ret = false;
            goto bail_not_owner;
          }
          locked_ = 0;
          locked_by_ = 0;
          pthread_setspecific(unlock_key_,NULL);

bail_not_locked:
bail_not_owner:
          ;
        }
        return ret;
      }

      bool is_locked()
      {
        bool ret = false;
        {
          scoped_pt_mutex m(mtx_);
          ret = is_locked_nl();
        }
        return ret;
      }

      unsigned int waiting_count()
      {
        unsigned int ret = 0;
        {
          scoped_pt_mutex m(mtx_);
          ret = waiting_;
        }
        return ret;
      }

      unsigned int locked_count()
      {
        unsigned int ret = 0;
        {
          scoped_pt_mutex m(mtx_);
          ret = locked_;
        }
        return ret;
      }
    };
  } /* namespace nthread */
} /* namespace csl */

/* EOF */

