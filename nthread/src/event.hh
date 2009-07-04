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

#ifndef _csl_nthread_event_hh_included_
#define _csl_nthread_event_hh_included_

/**
   @file event.hh
   @brief waitable events

   Implementation is based on pthread (posix threads)
*/

#ifdef __cplusplus
#include <memory>

namespace csl
{
  namespace nthread
  {
    /**
       @brief users of event may wait for notification from other threads

       event is a wrapper over pthread condition variable facilities

       it includes various additional features like counting the number of
       waiting threads, a more user friendly timeout specification and
       the hiding of the original pthread interface

       the design decision here is not to expose any pthread details, so
       the underlying implementation may change if needed. this is also true
       for the underlying pthread concepts. this is intended to be a higher
       level abstraction, than what pthread provides.

       the event object buffers pending notifications: if noone is waiting
       for a notification, than it increases the number of available notifications
       and if a new wait or wait_nb request arrives than it will be decreased
       those wait* ops will return immediately with success (true)

       the event object provide means for both posting a number of notifications at once
       and also to notify all waiting threads. for the latter the user does not need
       to specify the number of events to be posted
     */
    class event 
    {
    public:
      /** @brief constructor */
      event();

      /** @brief destructor */
      ~event();

      /**
         @brief clears all available notifications

         the rationale here is to provide means for undo notification operations
        */
      void clear_available();

      /**
         @brief send n notifications
         @param n is the number of the notifications to be sent
         @return true if succeed or false if the event object is destroyed or n is 0

         if there is noone waiting for the event, than it still works and the number of
         available events increases. this available events will be decreased by:

         @li wait  @em or
         @li wait_nb  @em or
         @li clear_available

        */
      bool notify(unsigned int n=1);

      /**
         @brief notify all waiting threads
         @return true if succeed or false if the event object is destroyed

         if there is noone waiting than it zeroes the available count. if there is any
         thread waits than it sets the available_count to the waiting_count and wakes up
         the waiting threads. in effect this will zero the available_count at the end.

         so either ways, when this function completes and the waiting threads return,
         both the available and waiting counters will be zero
        */
      bool notify_all();

      /**
         @brief wait for a notification
         @param timeout_ms is the number of milliseconds to wait for the event
         @return true if the notification arrived or false if timed out or other error

         the wait operation blocks for at most the given number in milliseconds. the system
         time resolution and the arrival of interrupt signal may change the effective amount
         of wait

         if timeout_ms is zero, than it waits infinitely

         if available count is not zero, than it returns immediately and decrease that by one
        */
      bool wait(unsigned long timeout_ms=0);

      /**
         @brief non blocking wait for a notification
         @return true if the notification arrived or false if error

         the non blocking wait operation returns immediately

         if available count is not zero, than it returns true immediately and decrease that by one

         if available count is zero, than it returns false immediately
        */
      bool wait_nb();


      /**
         @brief non blocking accessor for waiting count
         @return the number of currently blocked threads
        */
      unsigned int waiting_count();

      /**
         @brief non blocking accessor for available count
         @return the number of available notifications
        */
      unsigned int available_count();

    private:
      struct impl;
      std::auto_ptr<impl> impl_;

      // no-copy
      event(const event & other);
      event & operator=(const event & other);
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_nthread_event_hh_included_ */

/* EOF */
