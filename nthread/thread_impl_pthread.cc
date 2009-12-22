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


#if 0
#ifndef DEBUG
#define DEBUG
#define DEBUG_ENABLE_INDENT
//#define DEBUG_VERBOSE
#endif /* DEBUG */
#endif //0

#include "codesloop/nthread/thread.hh"
#include "codesloop/nthread/mutex.hh"
#include "codesloop/common/common.h"
#include "codesloop/common/logger.hh"
#include <pthread.h>

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
      CSL_OBJ(csl::nthread,thread::impl);

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
        ENTER_FUNCTION();
        scoped_mutex m(mtx_);
        start_routine_ = &entry;
        LEAVE_FUNCTION();
      }

      void set_stack_size(unsigned long sz)
      {
        ENTER_FUNCTION();
        CSL_DEBUGF( L"set_stack_size(sz:%ld)",sz );
        scoped_mutex m(mtx_);
        stack_size_ = sz;
        LEAVE_FUNCTION();
      }

      unsigned long get_stack_size()
      {
        ENTER_FUNCTION();
        unsigned long ret = 0;
        {
          scoped_mutex m(mtx_);
          ret = stack_size_;
        }
        CSL_DEBUGF( L"get_stack_size() => %ld",ret );
        RETURN_FUNCTION( ret );
      }

      bool start()
      {
        ENTER_FUNCTION();
        scoped_mutex m(mtx_);

        if( start_routine_ == &dummy_callback_ )
        {
          RETURN_FUNCTION( false );
        }

        if( is_started() || is_exited() )
        {
          RETURN_FUNCTION( false );
        }

        if( stack_size_ ) pthread_attr_setstacksize(&attr_, stack_size_);

        if( pthread_create( &tid_, &attr_, thread_entry_, this ) )
        {
          // error
          RETURN_FUNCTION( false );
        }
        else
        {
          RETURN_FUNCTION( true );
        }
      }

      bool stop()
      {
        ENTER_FUNCTION();

        if( !is_started() ) RETURN_FUNCTION( false );
        if( is_exited() )   RETURN_FUNCTION( false );

        {
          scoped_mutex m(mtx_);
          pthread_cancel( tid_ );
        }
        bool ret = exit_evt_.wait(50);
        CSL_DEBUGF( L"stop() => %s", (ret==true?"TRUE":"FALSE") );
        RETURN_FUNCTION( ret );
      }

      pevent & start_event() { return start_evt_; }
      pevent & exit_event()  { return exit_evt_;  }

      bool is_started()
      {
        ENTER_FUNCTION();
        bool ret = start_evt_.is_permanent();
        CSL_DEBUGF( L"is_started() => %s",(ret==true?"TRUE":"FALSE") );
        RETURN_FUNCTION( ret );
      }

      bool is_exited()
      {
        ENTER_FUNCTION();
        bool ret = exit_evt_.is_permanent();
        CSL_DEBUGF( L"is_exited() => %s",(ret==true?"TRUE":"FALSE") );
        RETURN_FUNCTION( ret );
      }

      bool is_running()
      {
        ENTER_FUNCTION();
        bool ret = (is_started()==true && is_exited()==false);
        CSL_DEBUGF( L"is_running() => %s", (ret==true?"TRUE":"FALSE") );
        RETURN_FUNCTION( ret );
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

