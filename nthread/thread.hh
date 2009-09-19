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

#ifndef _csl_nthread_thread_hh_included_
#define _csl_nthread_thread_hh_included_

/**
   @file thread.hh
   @brief thread helper

   Implementation is based on pthread (posix threads)
*/

#include "codesloop/common/common.h"
#include "codesloop/common/obj.hh"
#include "codesloop/nthread/pevent.hh"
#ifdef __cplusplus
#include <memory>

namespace csl
{
  namespace nthread
  {
    /**
       @brief helper for thread creation

       thread class is a wrapper over pthread functions. it collects a few features and
       gives a different, object-oriented interface,

       the design decision here is not to expose any pthread details, so
       the underlying implementation may change if needed. this is also true
       for the underlying pthread concepts. this is intended to be a higher
       level abstraction, than what pthread provides.

       the thread object stores a callback object that should be inherited from thread::callback

       when the thread is started than the operator()() is called on the supplied object. this
       call is the thread entry.

       the thread object provides a few convenience functions such as:

       @li waiting for the thread startup through a pevent object
       @li waiting for the thread exit through a pevent object
       @li querying wether the thread was started, exited or running
       @li stoping the thread (by thread cancellation)

      */
    class thread
    {
    public:
      /** @brief constructor */
      thread();

      /** @brief destructor */
      ~thread();

      /** @brief the abstract base class for the thread entry callback */
      class callback
      {
        public:
          /** @brief the operator that has to be implemented by the user supplied implementation */
          virtual void operator()(void) = 0;
          virtual ~callback() {}
      };

      /**
         @brief mandatory call to set the thread entry object

         if no thread entry was set than the thread start will do nothing
         and the started and exited states will not be touched by start()

        */
      void set_entry(callback & entry);

      /** @brief thread users may optionally set the stack size
          @param sz is the desired size */
      void set_stack_size(unsigned long sz);

      /** @brief query the given stack size */
      unsigned long get_stack_size();

      /**
         @brief starts a new thread
         @return true if OK, false if no entry was set or error happened

         the start function launches the new thread and that new thread will do the following:

         @li registers emergency cleanup function, that will send permanent exit event as a last chance
         @li sets cancellation state (ENABLE, ASYNCHRONOUS)
         @li calls the thread callback
         @li when it returns sets the cacnellation state (DISABLE)
         @li send permanent event (pevent)
         @li deregisters emergency handler

         */
      bool start();

      /**
         @brief tries to stop the running thread
         @returns true if OK, false if not running, exited or other error

         the function checks if the thread is really running and calls pthread_cancel if so

         stop() checks wether the thread exits in 50 ms. if yes it returns true.
         otherwise itreturns false

         please be advised to check and test your code, because pthread_cancel may not
         really stops the thread as expected. to be on the safe side cancel_here() function
         should be called at places where the thread may be canceled

         the library uses deferred cancellation mode, rather than asynchronous. this is why
         calling cancel_here() is advised.
        */
      bool stop();

      /**
          @brief helps flagging cancellation points

          this function is to tell the library where the thread could be cancelled

          certain C library functions do care about thread cancellation, but others don't
          this function is to help the cancellation process

          the cancel_here() function calls pthread_testcancel()
        */
      static void cancel_here();

      /** @brief returns a waitable pevent object that will be notified when the thread has started */
      pevent & start_event();

      /** @brief returns a waitable pevent object that will be notified when the thread has exited */
      pevent & exit_event();

      /** @brief return true if started */
      bool is_started();

      /** @brief return true if the thread is running */
      bool is_running();

      /** @brief return true if the thread has exited */
      bool is_exited();

      struct impl;

    private:
      std::auto_ptr<impl> impl_;

      // no-copy
      thread(const thread & other);
      thread & operator=(const thread & other);

      CSL_OBJ(csl::nthread,thread);
      USE_EXC();
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_nthread_thread_hh_included_ */

/* EOF */
