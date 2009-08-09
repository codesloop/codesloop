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

#ifndef _csl_nthread_pevent_hh_included_
#define _csl_nthread_pevent_hh_included_

/**
   @file pevent.hh
   @brief waitable permanent events

   Implementation is based on pthread (posix threads)
*/

#ifdef __cplusplus
#include <memory>

namespace csl
{
  namespace nthread
  {
    /**
       @brief users of pevent may wait for notification from other threads and may post permanent notifications

       pevent is a wrapper over event with an additional twist @see event.hh and @see nthread::event

       pevent is designed to help in a situation when an infinite number of notifications
       should be posted or in other words a specific condition is permanent and all wait
       operation should complete with success immediately

       in case of pevent it has two modes of operation:

       @li it works like event if the permanent mode was not set
       @li if the permanent mode was set it returns all wait operations immediately
     */
    class pevent
    {
    public:
      /** @brief constructor */
      pevent();

      /** @brief destructor */
      ~pevent();

      /**
          @brief clears both available notifcations and the permanent flag
          @see event::clear_available()

          calls clear_permanent() and event::clear_available()
        */
      void clear_available();

      /**
         @brief works like event::notify()
         @see event::notify()

         there is an additional twist here: if permanent flag was set than
         the notification is ignored
        */
      bool notify(unsigned int n=1);

      /**
         @brief works like event::notify_all()
         @see event::notify_all()

         there is an additional twist here: if permanent flag was set than
         the notification is ignored
        */
      bool notify_all();

      /**
         @brief works like event::wait()
         @see event::wait()

         if the permanent flag is set that it returns true immediately, the
         number of available notifications will not change.

         if the permanent flags is not set, than it works like event::wait and
         the number of available notifications will decrease or wait if none arrived
        */
      bool wait(unsigned long timeout_ms=0);

      /**
         @brief works like event::wait_nb()
         @see event::wait_nb()

         if the permanent flag is set that it returns true immediately, the
         number of available notifications will not change.

         if the permanent flags is not set, than it works like event::wait_nb() and
         the number of available notifications will decrease if there are already some
        */
      bool wait_nb();

      /** @brief works like event::waiting_count() */
      unsigned int waiting_count();

      /** @brief works like event::available_count() */
      unsigned int available_count();

      /**
         @brief sets the permanent flag

         when the permanent flag is set it has the side effect that it zeroes the available counter
         and also unblocks all waiting threads
        */
      void set_permanent();

      /** @brief clears the permanent flag */
      void clear_permanent();

      /** @brief checks the permanent flag */
      bool is_permanent();

    private:
      struct impl;
      std::auto_ptr<impl> impl_;

      // no-copy
      pevent(const pevent & other);
      pevent & operator=(const pevent & other);
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_nthread_pevent_hh_included_ */

/* EOF */
