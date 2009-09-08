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
   @file t__event.cc
   @brief Tests to check csl event behaviour
*/

#include "codesloop/common/test_timer.h"
#include "codesloop/nthread/event.hh"
#include "codesloop/nthread/thread.hh"
#include <assert.h>
#include <stdio.h>
#ifndef WIN32
# include <unistd.h>
# define SleepSeconds(A) ::sleep(A)
#else
# include <windows.h>
# define SleepSeconds(A) ::Sleep(A*1000)
#endif /* WIN32 */

using namespace csl::nthread;

/** @brief contains tests related to csl events */
namespace test_event
{
  /** @test baseline for timing */
  void test_init()
  {
    event e;
  }

  class event_callback1 : public thread::callback
  {
  public:
    event_callback1() : e_(0) {}

    virtual void operator()(void)
    {
      if( e_ ) { e_->wait(); }
      else     { throw("bad state"); }
    }
    virtual ~event_callback1() {}
    event * e_;
  };

  /** @test creating 100 threads and see if event works */
  void test_notify1()
  {
    event e;
    event_callback1 cb;
    cb.e_ = &e;
    thread t[100];

    for( int i=0;i<100;++i )
    {
      t[i].set_entry(cb);
      t[i].set_stack_size(16*1024);
      assert( t[i].start() == true );
      assert( t[i].start_event().wait() == true ); // DEADLOCK ???
    }

    while( e.waiting_count() != 100 )
    {
      printf("Waiting...[%d/%d] [%s:%d]\n",
        e.waiting_count()-100,
        e.available_count(),
        __FILE__,
        __LINE__);
      SleepSeconds(1);
    }

    assert( e.notify(10) == true );

    while( e.waiting_count() != 90 )
    {
      printf("Waiting...[%d/%d] [%s:%d]\n",
        e.waiting_count()-90,
        e.available_count(),
        __FILE__,
        __LINE__);
      SleepSeconds(1);
    }

    assert( e.notify_all() == true );

    while( e.waiting_count() > 0 )
    {
      printf("Waiting...[%d/%d] [%s:%d]\n",
        e.waiting_count(),
        e.available_count(),
        __FILE__,
        __LINE__);
      SleepSeconds(1);
    }

    assert( e.available_count() == 0 );
  }

  /** @test check the correctness of available_count */
  void test_available()
  {
    event e;

    // check available
    assert( e.available_count() == 0 );
    assert( e.wait_nb() == false );
    assert( e.wait(100) == false );

    // send 10 notify
    assert( e.available_count() == 0 );
    assert( e.notify(10) == true );
    assert( e.available_count() == 10 );

    // send notify all
    assert( e.notify_all() == true );
    assert( e.available_count() == 10 );

    // wait
    assert( e.wait() == true );
    assert( e.available_count() == 9 );

    // nb wait
    assert( e.wait_nb() == true );
    assert( e.available_count() == 8 );

    // notif all
    assert( e.notify_all() == true );
    assert( e.available_count() == 8 );

    // clear available
    e.clear_available();
    assert( e.available_count() == 0 );

    // last waits
    assert( e.wait_nb() == false );
    assert( e.wait(10) == false );
  }

  /** @test check the correctness of waiting_count */
  void test_waiting()
  {
    event e;

    // check waiting
    assert( e.wait_nb() == false );
    assert( e.wait(100) == false );
    assert( e.waiting_count() == 0 );

    // send 10 notify
    assert( e.notify(10) == true );
    assert( e.waiting_count() == 0 );

    // send notify all
    assert( e.notify_all() == true );
    assert( e.waiting_count() == 0 );

    // wait
    assert( e.wait() == true );
    assert( e.waiting_count() == 0 );

    // non-blocking wait
    assert( e.wait_nb() == true );
    assert( e.waiting_count() == 0 );

    // clear available
    e.clear_available();
    assert( e.wait_nb() == false );
    assert( e.wait(10) == false );
    assert( e.waiting_count() == 0 );
  }
}

using namespace test_event;

int main()
{
  csl_common_print_results(
    "test_init                   ",
    csl_common_test_timer_v0(test_init),"" );

  test_available();
  test_waiting();
  test_notify1();

  return 0;
}

/* EOF */
