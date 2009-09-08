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

/**
  @file ex_thread1.cc
  @brief basic thread and event usage
 */

#include "codesloop/common/common.h"
#include "codesloop/nthread/thread.hh"
#include "codesloop/nthread/event.hh"

using namespace csl::nthread;

/* thread entry class can contain data needed for the given thread
   to do its task
*/
class thread_entry : public thread::callback
{
  public:
    event e_;

    virtual void operator()(void)
    {
      e_.wait();
    }
};

/* this example demonstrates the ability to:

   1, wait on thread start event
   2, wait on thread exit event
   3, send notification event
   4, check the number of waiting threads for an event
*/
int main()
{
  thread t;
  thread_entry e;

  /* set thread entry */
  t.set_entry(e);

  /* launch thread */
  t.start();

  /* wait for start signal */
  t.start_event().wait();

  /* check waiting */
  while( e.e_.waiting_count() == 0 ) { SleepSeconds(1); }

  /* send notify event to thread */
  e.e_.notify();

  /* wait for thread exit */
  t.exit_event().wait();

  return 0;
}

