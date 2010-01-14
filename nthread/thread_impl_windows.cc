/*
Copyright (c) 2008,2009,2010, CodeSLoop Team

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

#include "codesloop/nthread/thread.hh"
#include "codesloop/nthread/mutex.hh"
#include "codesloop/common/common.h"

/**
  @file thread_impl_windows.cc
  @brief windows native implementation of thread class
*/

namespace csl
{
  namespace nthread
  {
    namespace // anonymous
    {
      DWORD WINAPI thread_entry_( LPVOID lpParam );
      DWORD WINAPI thread_guard_( LPVOID lpParam );

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
      HANDLE                  entry_thread_;
      HANDLE                  guard_thread_;

      impl() : stack_size_(0),
        start_routine_(&dummy_callback_),
        entry_thread_(0),
        guard_thread_(0) { }

      ~impl()
      {
        stop();
        if( is_started() && guard_thread_ != 0 )
        {
          WaitForSingleObject( guard_thread_, INFINITE );
        }
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

      pevent & start_event() { return start_evt_; }
      pevent & exit_event()  { return exit_evt_;  }

      bool is_started()  { return start_evt_.is_permanent(); }
      bool is_exited()   { return exit_evt_.is_permanent();  }

      bool is_running()
      {
        return (is_started()==true &&
                is_exited()==false);
      }

      bool start()
      {
        unsigned long ss = 0;
        {
          scoped_mutex m(mtx_);
          if( start_routine_ == &dummy_callback_ ) return false;
          if( entry_thread_ ) return false;
          if( is_started() || is_exited() ) return false;
          ss = stack_size_;
        }

        HANDLE entry_thread = CreateThread(
          NULL,            // security attrs
          ss,              // stack size
          thread_entry_,   // entry
          this,            // param
          0,               // creation flag: run immediately
          NULL );          // thread id

        if( entry_thread == NULL )
        {
          // DWORD err = GetLastError();
          // DebugBreak();
          return false;
        }

        {
          scoped_mutex m(mtx_);
          entry_thread_ = entry_thread;
        }

        HANDLE guard_thread = CreateThread(
          NULL,            // security attrs
          32*1024,         // stack size
          thread_guard_,   // entry
          this,            // param
          0,               // creation flag: run immediately
          NULL );          // thread id

        if( guard_thread == NULL )
        {
          // DWORD err = GetLastError();
          // DebugBreak();
          return false;
        }

        {
          scoped_mutex m(mtx_);
          guard_thread_ = guard_thread;
        }

        return true;
      }

      bool stop()
      {
        if( !is_started() ) return false;
        if( is_exited() )   return false;

        {
          scoped_mutex m(mtx_);
          if( entry_thread_ ) TerminateThread( entry_thread_, 666 );
        }
        return exit_evt_.wait(50);
      }
    };

    namespace // anonymous
    {
      DWORD WINAPI thread_guard_( LPVOID lpParam )
      {
        thread::impl * p = reinterpret_cast<thread::impl *>(lpParam);

        if( p )
        {
          if( p->entry_thread_ )
          {
            // wait for thread exit
            if( WaitForSingleObject( p->entry_thread_, INFINITE ) == WAIT_OBJECT_0 )
            {
              p->exit_evt_.set_permanent();
            }
          }
        }
        return 0;
      }

      DWORD WINAPI thread_entry_( LPVOID lpParam )
      {
        thread::impl * p = reinterpret_cast<thread::impl *>(lpParam);
        if( p )
        {
          // flag start
          p->start_evt_.set_permanent();

          // start the thread routine
          (*(p->start_routine_))();

          // flag exit
          p->exit_evt_.set_permanent();
        }
        return 0;
      }
    }
  }
}

/* EOF */
