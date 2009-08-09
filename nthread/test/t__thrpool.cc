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
   @file t__thrpool.cc
   @brief Tests to check thrpool behaviour
 */

#include "test_timer.h"
#include "thrpool.hh"
#include "mutex.hh"
#include "common.h"
#include <assert.h>
#include <stdio.h>

using namespace csl::nthread;

/** @brief contains tests related to csl threads */
namespace test_thrpool
{
  class handler : public thread::callback
  {
    public:
      virtual void operator()(void)
      {
        ev_.notify();
        SleepSeconds(0);
      }

      virtual ~handler() {}
      event ev_;
  };

  void basic()
  {
    event ev;
    handler h;

    thrpool pool;
    assert( pool.init( 1,4,1000,3,ev,h ) == true );

    assert( pool.start_event().wait(1000) == true );
    assert( pool.count() == 1 );
    assert( h.ev_.available_count() == 0 );

    while( ev.waiting_count() != 1 )
    {
      SleepSeconds(1);
      fprintf(stderr,".");
    }

    /* send 200 notifications */
    ev.notify( 200 );

    while( pool.count() != 4 )
    {
      assert( h.ev_.wait( 200 ) == true );
      SleepSeconds(2);
      fprintf(stderr," (%d:%d)",ev.waiting_count(),ev.available_count());
    }

    while( pool.count() != 1 )
    {
      pool.exit_event().wait(1000);
      fprintf(stderr," /%d:%d:%d/",ev.waiting_count(),ev.available_count(),pool.count());
    }
  }
}

using namespace test_thrpool;

int main()
{
  basic();
  return 0;
}

/* EOF */
