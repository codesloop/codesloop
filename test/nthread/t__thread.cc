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
   @file t__thread.cc
   @brief Tests to check thread behaviour
*/

#include "test_timer.h"
#include "thread.hh"
#include "mutex.hh"
#include "common.h"
#include <assert.h>
#include <stdio.h>

using namespace csl::nthread;

/** @brief contains tests related to csl threads */
namespace test_thread
{
  /** @test baseline for timing */
  void test_ref()
  {
    thread t;
  }

  class dummy_callback : public thread::callback
  {
  public:
    static mutex m_;
    static int i_;

    virtual void operator()(void)
    {
      scoped_mutex m(m_);
      ++i_;
    }

    virtual ~dummy_callback() {}
  };

  int dummy_callback::i_ = 0;
  mutex dummy_callback::m_;

  /** @test creating a dummy thread */
  void test_start()
  {
    thread t;
    dummy_callback cb;
    t.set_entry(cb);
    assert( t.start() == true );
    assert( t.start_event().wait(9000) == true );
    assert( t.is_started() == true );
    assert( t.exit_event().wait(9000) == true );
    assert( t.is_exited() == true );
    assert( dummy_callback::i_ > 0 );
  }

  class sleep_callback : public thread::callback
  {
  public:
    static mutex m_;
    static int i_;

    virtual void operator()(void)
    {
      {
        scoped_mutex m(m_);
        ++i_;
      }
      for(int i=0;i<10000;++i )
      {
        SleepSeconds(1);
        thread::cancel_here();
      }
    }

    virtual ~sleep_callback() {}
  };

  int sleep_callback::i_ = 0;
  mutex sleep_callback::m_;

  /** @test check if stop works with a sleeping thread */
  void test_stop()
  {
    thread t;
    sleep_callback cb;
    t.set_entry(cb);
    assert( t.start() == true );
    assert( t.start_event().wait() == true );
    assert( t.is_started() == true );
    if( !t.stop() ) printf("-");
    assert( t.exit_event().wait() == true );
    assert( t.is_exited() == true );
    assert( sleep_callback::i_ > 0 );
  }
}

using namespace test_thread;

int main()
{
  csl_common_print_results(
    "test_ref                    ",
    csl_common_test_timer_v0(test_ref),"" );

  csl_common_print_results(
    "test_start                  ",
    csl_common_test_timer_v0(test_start),"" );

  test_stop();
  test_stop();

#ifndef WIN32
  // thread.stop() leaks memory because of winapi
  csl_common_print_results(
    "test_stop                   ",
    csl_common_test_timer_v0(test_stop),"" );
#endif
  return 0;
}

/* EOF */
