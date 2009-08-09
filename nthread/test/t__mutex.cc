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
   @file t__mutex.cc
   @brief Tests to check csl mutex behaviour
*/

#include "test_timer.h"
#include "mutex.hh"
#include "event.hh"
#include "thread.hh"
#include <assert.h>
#include <stdio.h>
#ifndef WIN32
#  include <unistd.h>
#else
#  include <windows.h>
#endif /* WIN32 */

using namespace csl::nthread;

/** @brief contains tests related to csl mutexes */
namespace test_mutex
{
  /** @test baseline for timing */
  void test_init()
  {
    mutex m;
  }

  class event_callback2 : public thread::callback
  {
  public:
    event_callback2() : e_(0), m_(0) {}

    virtual void operator()(void)
    {
      scoped_mutex m(*m_);
      e_->notify();
    }
    virtual ~event_callback2() {}
    event * e_;
    mutex * m_;
  };

  /** @test concurrent locking test w/ 200 threads */
  void test_lock()
  {
    event e;
    mutex m;
    event_callback2 cb;
    cb.e_ = &e;
    cb.m_ = &m;
    thread t[100];

    for( int i=0;i<100;++i )
    {
      t[i].set_entry(cb);
      t[i].set_stack_size(16*1024);
      assert( t[i].start() == true );
    }

    for( int i=0;i<100;++i )
    {
      assert( e.wait() == true );
      assert( m.locked_count() <= 1 );
    }

    assert( m.waiting_count() == 0 );
    assert( m.locked_count() == 0 );
  }
}

using namespace test_mutex;

int main()
{
  csl_common_print_results(
    "test_init                   ",
    csl_common_test_timer_v0(test_init),"" );

  csl_common_print_results(
    "test_lock                   ",
    csl_common_test_timer_v0(test_lock),"" );

  return 0;
}

/* EOF */
