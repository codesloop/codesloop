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
  @file ex_thrpool.cc
  @brief basic thread pool usage
 */

#include "thrpool.hh"
#include "common.h"
#include "event.hh"

using namespace csl::nthread;

/*
** This example demonstrates the usage of the codesloop's threadpool facility
**
** thrpool is the class that manages a number of threads. the number of threads
** that are running in any given time is kept between the limits given during
** initialization.
**
** The concept that thrpool follows:
**
** - the parameters at initialization controls all aspects of its operation
** - the ev parameter is an event that the threads are waiting to be signaled
** - when ev is signaled a thread is woken up and calls the handler that is supplied
**   at initialization time
** - each running thread checks wether there are more events waiting to be dispatched
**   then the available number of thread. if so a new thread will be started if the
**   number of currently running threads wouldn't exceed the limit given to init
*/

class handler : public thread::callback
{
  public:
    virtual void operator()(void)
    {
      /* work to be done here */
    }

    virtual ~handler() {}

    /* member variables should be used for passing data to be processed
    ** by the worker threads
    */
};

int main()
{
  thrpool pool;

  unsigned int min_thread_to_be_run = 1;
  unsigned int max_thread_to_be_run = 4;
  unsigned int timeout_ms = 1000;
  unsigned int max_timedout_attempt = 3;

  event notification_event;
  handler handler_to_be_called;


  /* this initializes and starts the threadpool:
  **
  ** the supplied parameters are:
  **
  **   min_thread_to_be_run : tells how many thread should be kept
  **                          operational
  **
  **   max_thread_to_be_run : tells what is the maximum number of threads may
  **                          be running
  **
  **   timeout_ms :           tells how long should a thread wait for the
  **                          notification if a timeout happens 'max_timedout_attempt'
  **                          times then the thread checks if there are more then
  **                          'min_thread_to_be_run' threads are running and if thats
  **                          the case it exits
  **
  **   max_timedout_attempt : tells how many time the thread can timeout before
  **                          it checks if he is really needed
  **
  **   notification_event :   is the object all thread in the threadpool are waiting for
  **
  **   handler_to_be_called : is the object to be called when a new event arrives
  */
  pool.init(
            min_thread_to_be_run,
            max_thread_to_be_run,
            timeout_ms,
            max_timedout_attempt,
            notification_event,
            handler_to_be_called
           );

  /* we can wait for the threads to be started */
  if( pool.start_event().wait(1000) == false )
  {
    /* the wait for threads is timed out */
  }

  /* the thread pool is doing its task, while we can do our job */
  return 0;
}

/* EOF */
