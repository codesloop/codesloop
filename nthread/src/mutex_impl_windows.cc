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

#include "mutex.hh"
#include "common.h"

/**
  @file mutex_impl_windows.cc
  @brief windows native implementation of mutex
*/

namespace csl
{
  namespace nthread
  {
    struct mutex::impl
    {
      bool                invalid_;
      unsigned int        waiting_;
      unsigned int        locked_;

      CRITICAL_SECTION    lock_var_;
      HANDLE              mutex_var_;
      CRITICAL_SECTION *  lock_;
      HANDLE *            mutex_;

      impl() : invalid_(false), waiting_(0), locked_(0)
      {
        InitializeCriticalSection( &lock_var_ );

        mutex_var_ = CreateMutex(
          NULL,  // security attr
          FALSE,  // not locked (owned) initally
          NULL ); // name

        lock_     = &lock_var_;
        mutex_    = &mutex_var_;
      }

      ~impl()
      {
        unsigned int locked  = 0;
        EnterCriticalSection( lock_ );
        invalid_   = true;
        locked     = locked_;
        LeaveCriticalSection( lock_ );

        // release
        for( unsigned int i=0;i<locked;++i )
          if( ReleaseMutex( *mutex_ ) == 0 ) break;

        EnterCriticalSection( lock_ );
        mutex_  = 0;
        locked_ = 0;
        LeaveCriticalSection( lock_ );

        CloseHandle( mutex_var_ );
        DeleteCriticalSection( &lock_var_ );
      }

      bool is_locked_nl()
      {
        return (locked_ > 0 ? true : false);
      }

      bool lock(unsigned long timeout_ms)
      {
        bool ret = true;
        bool invalid = false;

        if( timeout_ms == 0 ) { timeout_ms = INFINITE; }

        {
          EnterCriticalSection( lock_ );
          ++waiting_;
          invalid = invalid_;
          LeaveCriticalSection( lock_ );
        }

        if( invalid ) ret = false;
        else
        {
          DWORD err = WaitForSingleObject( *mutex_, timeout_ms );
          switch( err )
          {
          case WAIT_FAILED:
          case WAIT_TIMEOUT:
          case WAIT_ABANDONED:
            ret  = false;
            break;
          };
        }

        {
          EnterCriticalSection( lock_ );
          --waiting_;
          if( ret ) ++locked_;
          LeaveCriticalSection( lock_ );
        }
        return ret;
      }

      bool try_lock()
      {
        bool ret = true;
        bool invalid = false;

        {
          EnterCriticalSection( lock_ );
          ++waiting_;
          invalid = invalid_;
          LeaveCriticalSection( lock_ );
        }

        if( invalid ) ret = false;
        else
        {
          DWORD err = WaitForSingleObject( *mutex_, 0 );
          switch( err )
          {
          case WAIT_FAILED:
          case WAIT_TIMEOUT:
          case WAIT_ABANDONED:
            ret  = false;
            break;
          };
        }

        {
          EnterCriticalSection( lock_ );
          --waiting_;
          if( ret ) ++locked_;
          LeaveCriticalSection( lock_ );
        }
        return ret;
      }

      bool unlock()
      {
        bool ret = false;
        bool do_release = false;
        bool invalid = false;
        {
          EnterCriticalSection( lock_ );
          if( locked_ > 0 )
          {
            do_release = true;
            invalid = invalid_;
          }
          LeaveCriticalSection( lock_ );

          if( do_release )
          {
            if( ReleaseMutex( *mutex_ ) )
            {
              --locked_;
              if( invalid == false ) ret = true;
            }
          }
        }
        return ret;
      }

      bool recursive_unlock()
      {
        bool ret = true;
        {
          EnterCriticalSection( lock_ );
          if( locked_ > 0 )
          {
            for( unsigned int i=0; i<locked_; ++i )
            {
              if( ReleaseMutex( *mutex_ ) == 0 )
              {
                ret = false;
                break;
              }
            }
          }
          else { ret = false; }
          //
          if( invalid_ ) { ret = false; }
          LeaveCriticalSection( lock_ );
        }
        return ret;
      }

      bool is_locked()
      {
        bool ret = false;
        EnterCriticalSection( lock_ );
        ret = is_locked_nl();
        LeaveCriticalSection( lock_ );
        return ret;
      }

      unsigned int waiting_count()
      {
        unsigned int ret = 0;
        EnterCriticalSection( lock_ );
        ret = waiting_;
        LeaveCriticalSection( lock_ );
        return ret;
      }

      unsigned int locked_count()
      {
        unsigned int ret = 0;
        EnterCriticalSection( lock_ );
        ret = locked_;
        LeaveCriticalSection( lock_ );
        return ret;
      }
    };
  } /* namespace nthread */
} /* namespace csl */

/* EOF */

