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
#include <windows.h> 
#include <errno.h>

/**
  @file pevent_impl_windows.cc
  @brief windows native implementation of pevent class
*/

namespace csl
{
  namespace nthread
  {
    struct pevent::impl
    {
      bool                invalid_;
      unsigned int        waiting_;
      unsigned int        available_;
      bool                permanent_;

      HANDLE              event_var_;
      HANDLE              mutex_var_;
      HANDLE *            event_;
      HANDLE *            mutex_;

      impl() : invalid_(false), waiting_(0), available_(0), permanent_(false)
      {
        event_var_ = CreateEvent(
          NULL,        // security attr
          TRUE,        // manual
          FALSE,       // signaled
          NULL );      // name

        mutex_var_ = CreateMutex(
          NULL,        // security attr
          FALSE,       // not locked (owned) initally
          NULL );      // name

        event_    = &event_var_;
        mutex_    = &mutex_var_;
      }

      ~impl()
      {
        WaitForSingleObject( *mutex_, INFINITE );
        invalid_   = true;
        available_ = waiting_;
        ReleaseMutex( *mutex_ );

        // broadcast
        SetEvent( *event_ );

        WaitForSingleObject( *mutex_, INFINITE );
        mutex_ = 0;
        event_ = 0;
        ReleaseMutex( mutex_var_ );

        CloseHandle( mutex_var_ );
        CloseHandle( event_var_ );
      }

      bool notify(unsigned int n)
      {
        if( !n ) return false;

        bool ret = true;
        if( WaitForSingleObject( *mutex_, INFINITE ) == WAIT_OBJECT_0 )
        {
          if( invalid_ )        { ret = false; }
          else if( permanent_ ) { ret = true; }
          else
          {
            available_ += n;
            SetEvent( *event_ );
          }
          ReleaseMutex( *mutex_ );
        }
        return ret;
      }

      bool notify_all()
      {
        bool ret = true;

        if( WaitForSingleObject( *mutex_, INFINITE ) == WAIT_OBJECT_0 )
        {
          if( invalid_ )        { ret = false; }
          else if( permanent_ ) { ret = true; }
          else
          {
            if( waiting_ > available_ ) { available_ = waiting_; }
            if( waiting_ > 0 ) { SetEvent( *event_ ); }
          }
          ReleaseMutex( *mutex_ );
        }
        else { ret = false; }
        return ret;
      }

      bool wait(unsigned long timeout_ms)
      {
        bool ret        = true;

        if( timeout_ms == 0 ) { timeout_ms = INFINITE; }

        if( WaitForSingleObject( *mutex_, INFINITE ) == WAIT_OBJECT_0 )
        {
          if( invalid_ ) 
          {
            ret = false;
            ReleaseMutex( *mutex_ );
          }
          else if( permanent_ )
          {
            if( available_ ) { --available_; }
            ret = true;
            ReleaseMutex( *mutex_ );
          }
          else if( available_ )
          {
            --available_; 
            if( !available_ ) ResetEvent(*event_);
            ReleaseMutex( *mutex_ );
          }
          else
          {
            ++waiting_;
retry_wait:
            DWORD err = SignalObjectAndWait( *mutex_, *event_, timeout_ms, FALSE );

            if( WaitForSingleObject( *mutex_, INFINITE ) == WAIT_OBJECT_0 )
            {
              if( err == WAIT_TIMEOUT ) { ret = false; }
              else
              {
                if( available_ == 0 )
                { 
                  if( !permanent_ ) goto retry_wait; 
                } // this should not happen
                else 
                {
                  --available_;
                  if( !available_  ) { ResetEvent(*event_); }
                }
              }
              --waiting_; // must be here, to be protected by the lock
              ReleaseMutex( *mutex_ );
            }
            else { ret = false; }
          }
        }
        else { ret = false; }
        return ret;
      }

      bool wait_nb()
      {
        bool ret = false;
        if( WaitForSingleObject( *mutex_, INFINITE ) == WAIT_OBJECT_0 )
        {          
          if( invalid_ ) { ret = false; }
          else if( available_ )
          {
            ret = true;
            --available_;
            if( available_ == 0 ) ResetEvent( *event_ );
          }
          else if( permanent_ ) { ret = true; }
          ReleaseMutex( *mutex_ );
        }
        else { ret = false; }
        return ret;
      }

      unsigned int waiting_count()
      {
        unsigned int ret = 0;
        if( WaitForSingleObject( *mutex_, INFINITE ) == WAIT_OBJECT_0 )
        {
          ret = waiting_;
          ReleaseMutex( *mutex_ );
        }
        return ret;
      }

      unsigned int available_count()
      {
        unsigned int ret = 0;
        if( WaitForSingleObject( *mutex_, INFINITE ) == WAIT_OBJECT_0 )
        {
          ret = available_;
          ReleaseMutex( *mutex_ );
        }
        return ret;
      }

      void set_permanent()
      {
        if( WaitForSingleObject( *mutex_, INFINITE ) == WAIT_OBJECT_0 )
        {
          if( !permanent_ )
          {
            permanent_ = true;
            available_ = waiting_;
            if( waiting_ > 0 ) SetEvent(*event_);
          }
          ReleaseMutex( *mutex_ );
        }
      }

      void clear_permanent()
      {
        if( WaitForSingleObject( *mutex_, INFINITE ) == WAIT_OBJECT_0 )
        {
          permanent_ = false;
          ReleaseMutex( *mutex_ );
        }
      }

      bool is_permanent()
      {
        bool ret = false;
        if( WaitForSingleObject( *mutex_, INFINITE ) == WAIT_OBJECT_0 )
        {
          ret = permanent_;
          ReleaseMutex( *mutex_ );
        }
        return ret;
      }

      void clear_available()
      {
        if( WaitForSingleObject( *mutex_, INFINITE ) == WAIT_OBJECT_0 )
        {
          available_ = 0;
          ResetEvent( *event_ );
          ReleaseMutex( *mutex_ );
        }
      }
    };
  } /* namespace nthread */
} /* namespace csl */

/* EOF */
