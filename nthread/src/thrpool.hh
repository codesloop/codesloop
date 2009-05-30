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

#ifndef _csl_nthread_thrpool_hh_included_
#define _csl_nthread_thrpool_hh_included_

/**
   @file thrpool.hh
   @brief thread pool
 */

#include "event.hh"
#include "mutex.hh"
#include "thread.hh"
#ifdef __cplusplus
#include <list>

namespace csl
{
  namespace nthread
  {
    /** @todo document me */
    class thrpool
    {
      public:
        thrpool();
        virtual ~thrpool();

        bool init( unsigned int min,
                   unsigned int max,
                   unsigned int timeout,
                   unsigned int attempts,
                   event & ev,
                   thread::callback & handler );

        bool start_one();
        bool graceful_stop();
        bool unpolite_stop();

        unsigned int count();
        unsigned int min();
        unsigned int max();
        unsigned int timeout();
        unsigned int attempts();
        bool use_exc();
        void use_exc(bool yesno);
        event & exit_event();
        event & start_event();
        unsigned int cleanup();

        void on_entry();
        void on_exit();

      private:
        typedef std::pair<thread *,thread::callback *> thr_t;
        typedef std::list<thr_t> thrlist_t;

        unsigned int        count_;
        unsigned int        min_;
        unsigned int        max_;
        unsigned int        timeout_;
        unsigned int        attempts_;
        bool                use_exc_;
        bool                stop_me_;
        mutex               mtx_;
        event               exit_event_;
        event               start_event_;
        event *             ev_;
        thread::callback *  handler_;
        thrlist_t           threads_;
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_nthread_thrpool_hh_included_ */

/* EOF */
