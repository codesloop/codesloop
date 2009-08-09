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

#include "thread.hh"
#include "mutex.hh"
#include "common.h"
#include <pthread.h> 
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>

/**
  @file thread_impl_pthread.cc
  @brief pthread implementation of thread class
*/

namespace csl
{
  namespace nthread
  {
    namespace // anonymous
    {
      extern "C" void * thread_entry_(void * arg);
      extern "C" void thread_cleanup_(void * arg);

      class dummy_callback : public thread::callback
      {
      public:
        virtual void operator()(void) {}
        virtual ~dummy_callback() {}
      };
    }

    struct thread::impl
    {
      pevent start_evt_;
      pevent exit_evt_;
      mutex  mtx_;

      unsigned long           stack_size_;
      dummy_callback          dummy_callback_;
      thread::callback *      start_routine_;
      pthread_t               tid_;
      pthread_attr_t          attr_;

      impl() : stack_size_(0), start_routine_(&dummy_callback_), tid_(0)
      {
        pthread_attr_init(&attr_);
      }

      ~impl()
      {
        stop();
        if( is_started() && tid_ != 0 ) pthread_join(tid_,NULL);
        pthread_attr_destroy(&attr_);
      }

      void set_entry(callback & entry)
      {
        scoped_mutex m(mtx_);
        start_routine_ = &entry;
      }

      void set_stack_size(unsigned long sz)
      {
        scoped_mutex m(mtx_);
        stack_size_ = sz;
      }

      unsigned long get_stack_size()
      {
        unsigned long ret = 0;
        {
          scoped_mutex m(mtx_);
          ret = stack_size_;
        }
        return ret;
      }

      bool start()
      {
        scoped_mutex m(mtx_);
        if( start_routine_ == &dummy_callback_ ) return false;
        if( is_started() || is_exited() ) return false;

        if( stack_size_ ) pthread_attr_setstacksize(&attr_, stack_size_);

        if( pthread_create( &tid_, &attr_, thread_entry_, this ) )
        {
          // error
          return false;
        }
        else
        {
          return true;
        }
      }

      bool stop()
      {
        if( !is_started() ) return false;
        if( is_exited() )   return false;

        {
          scoped_mutex m(mtx_);
          pthread_cancel( tid_ );
        }
        return exit_evt_.wait(50);
      }

      pevent & start_event() { return start_evt_; }
      pevent & exit_event()  { return exit_evt_;  }

      bool is_started()  { return start_evt_.is_permanent(); }
      bool is_exited()   { return exit_evt_.is_permanent();  }

      bool is_running()
      {
        return (is_started()==true &&
                is_exited()==false);
      }
    };

    namespace // anonymous
    {
      extern "C" void thread_cleanup_(void * arg)
      {
        thread::impl * p = (thread::impl *)arg;
        if( p ) p->exit_evt_.set_permanent();
      }

      extern "C" void * thread_entry_(void * arg)
      {
        thread::impl * p = (thread::impl *)arg;
        if( p )
        {
          int old_state=0,old_type=0;
          // first set cleanup routine
          pthread_cleanup_push(thread_cleanup_,arg);

          // flag start
          p->start_evt_.set_permanent();

          // now make it cancellable
          pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,&old_state);
          pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,&old_type);

          // start the thread routine
          (*(p->start_routine_))();

          // clear cancellable state
          pthread_setcanceltype(old_type,&old_type);
          pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,&old_state);

          // flag exit
          p->exit_evt_.set_permanent();

          // remove cleanup routine
          pthread_cleanup_pop(0);
        }
        return 0;
      }
    }
  }
}

/* EOF */

