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

#include "pevent.hh"
#include "common.h"
#include <pthread.h> 
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>

/**
  @file pevent_impl_pthread.cc
  @brief pthread implementation of pevent class
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
    };

    struct pevent::impl
    {
      bool                invalid_;
      unsigned int        waiting_;
      unsigned int        available_;
      bool                permanent_;

      pthread_mutexattr_t mtx_attr_;
      pthread_mutex_t   * mtx_;
      pthread_cond_t    * cond_;
      pthread_mutex_t     mtx_var_;
      pthread_cond_t      cond_var_;

      impl() : invalid_(false), waiting_(0), available_(0), permanent_(false)
      {
        pthread_mutexattr_init(&mtx_attr_);
        pthread_mutexattr_settype(&mtx_attr_,PTHREAD_MUTEX_NORMAL);
        pthread_mutex_init(&mtx_var_,&mtx_attr_);
        pthread_cond_init(&cond_var_,NULL);
        mtx_  = &mtx_var_;
        cond_ = &cond_var_;
      }

      ~impl()
      {
        // lock variables
        pthread_mutex_lock(mtx_);

        // tell waiting threads to abort
        invalid_   = true;
        available_ = waiting_;
        if( waiting_ > 0 ) pthread_cond_broadcast(cond_);

        // invalidate original mutex data
        mtx_  = (pthread_mutex_t *)0xbad;
        cond_ = (pthread_cond_t *)0xbad;

        // destroy the allocated pthread structures
        pthread_mutex_unlock(&mtx_var_);
        pthread_mutexattr_destroy(&mtx_attr_);
        pthread_mutex_destroy(&mtx_var_);
        pthread_cond_destroy(&cond_var_);
      }

      bool notify(unsigned int n)
      {
        if( !n ) return false;

        bool ret = true;
        {
          scoped_pt_mutex m(mtx_);

          if( invalid_ )        { ret = false; }
          else if( permanent_ ) { ret = true; }
          else
          {
            available_ += n;

            if( waiting_ == 0 )
            {
              /* do nothing: noone is waiting */
            }
            else if( available_ >= waiting_ )
            {
              pthread_cond_broadcast(cond_);
            }
            else
            {
              for( unsigned int i=0;i<available_;++i )
              {
                pthread_cond_signal(cond_);
              }
            }
          }
        }
        return ret;
      }

      bool notify_all()
      {
        bool ret = true;
        {
          scoped_pt_mutex m(mtx_);

          if( invalid_ ) ret = false;
          else
          {
            if( available_ <= waiting_ )
            {
              available_ = waiting_;
            }
            if( waiting_ > 0 )
            {
              pthread_cond_broadcast(cond_);
            }
          }
        }
        return ret;
      }

      bool wait(unsigned long timeout_ms)
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

          if( invalid_ )        { ret = false;  }
          else if( permanent_ ) { ret = true; }
          else if( timeout_ms )
          {
            while( available_ == 0 )
            {
              if( (err=pthread_cond_timedwait(cond_, mtx_, &at)) == ETIMEDOUT )
              {
                timed_out = true;
                ret = false;
                break;
              }
              else if( err== 0 )
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
            while( available_ == 0 )
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

          if( ret && available_ )
          {
            --available_;
          }
        }

        // help others to complete
        ::sleep(0);
        return ret;
      }

      bool wait_nb()
      {
        bool ret = false;
        {
          scoped_pt_mutex m(mtx_);

          if( invalid_ )       { ret = false; }
          else if( available_ )
          {
            ret = true;
            --available_;
          }
          else if( permanent_ ) { ret = true;  }
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

      unsigned int available_count()
      {
        unsigned int ret = 0;
        {
          scoped_pt_mutex m(mtx_);
          ret = available_;
        }
        return ret;
      }

      void set_permanent()
      {
        scoped_pt_mutex m(mtx_);
        if( !permanent_ )
        {
          permanent_ = true;
          available_ = waiting_;
          if( waiting_ > 0 ) pthread_cond_broadcast(cond_);
        }
      }

      void clear_permanent()
      {
        scoped_pt_mutex m(mtx_);
        permanent_ = false;
      }

      bool is_permanent()
      {
        bool ret = false;
        {
          scoped_pt_mutex m(mtx_);
          ret = permanent_;
        }
        return ret;
      }

      void clear_available()
      {
        scoped_pt_mutex m(mtx_);
        available_ = 0;
        permanent_ = false;
      }
    };
  } /* namespace nthread */
} /* namespace csl */

/* EOF */

