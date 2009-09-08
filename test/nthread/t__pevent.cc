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
   @file t__pevent.cc
   @brief Tests to check csl permanent event behaviour
*/

#include "codesloop/common/test_timer.h"
#include "codesloop/nthread/pevent.hh"
#include "codesloop/nthread/thread.hh"
#include <assert.h>
#include <stdio.h>
#ifndef WIN32
# include <unistd.h>
# define SleepSeconds(A) ::sleep(A)
# define SleepMiliseconds(A) ::usleep(A*1000)
#else
# include <windows.h>
# define SleepSeconds(A) ::Sleep(A*1000)
# define SleepMiliseconds(A) ::Sleep(A)
#endif /* WIN32 */

using namespace csl::nthread;

/** @brief contains tests related to csl pevents */
namespace test_pevent
{
  /** @test baseline for timing */
  void test_init()
  {
    pevent p;
  }

  class pevent_callback1 : public thread::callback
  {
  public:
    pevent_callback1() : e1_(0) {}

    virtual void operator()(void)
    {
      e1_->notify();
      e2_->wait();
    }
    virtual ~pevent_callback1() {}
    pevent * e1_;
    pevent * e2_;
  };

  /** @test concurrent notification test w/ 200 threads */
  void test_notify1()
  {
    pevent e1,e2;
    pevent_callback1 cb;
    cb.e1_ = &e1;
    cb.e2_ = &e2;
    thread t[100];

    for( int i=0;i<100;++i )
    {
      t[i].set_entry(cb);
      t[i].set_stack_size(16*1024);
      assert( t[i].start() == true );
    }

    for( int i=0;i<100;++i )
    {
      assert( e1.wait() == true );
    }

    e2.set_permanent();

    for( int i=0;i<100;++i )
    {
      t[i].exit_event().wait();
    }

    assert( e1.waiting_count() == 0 );
    assert( e1.available_count() == 0 );
    assert( e1.is_permanent() == false );

    assert( e2.is_permanent() == true );
    assert( e2.available_count() == 0 );

    while( e2.waiting_count() > 0 )
    {
      printf("Waiting...[%d/%d] [%s:%d]\n",
        e2.waiting_count()-100,
        e2.available_count(),
        __FILE__,
        __LINE__);
      SleepSeconds(1);
    }
  }
}

using namespace test_pevent;

int main()
{
  csl_common_print_results(
    "test_init                   ",
    csl_common_test_timer_v0(test_init),"" );

  test_notify1();

  csl_common_print_results(
    "test_notify1                ",
    csl_common_test_timer_v0(test_notify1),"" );

  return 0;
}

/* EOF */
