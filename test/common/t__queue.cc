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
   @file t__inpvec.cc
   @brief Tests to verify queue
 */

#if 0
#ifndef DEBUG
#define DEBUG
#endif /* DEBUG */
#endif

#include "codesloop/common/tbuf.hh"
#include "codesloop/common/queue.hh"
#include "codesloop/common/test_timer.h"
#include "codesloop/common/ustr.hh"
#include "codesloop/common/logger.hh"
#include "codesloop/common/common.h"
#include <assert.h>
#include <list>
#include <string>

using namespace csl::common;

/** @brief contains tests related to queue */
namespace test_queue {

  struct X
  {
    unsigned int x;
    tbuf<1024>   buf;
    X() : x(0xDeadBeef) { }
  };

  void baseline_queue()
  {
    queue<uint64_t> q;
  }

  void baseline_queue2()
  {
    queue<X> q;
  }

  void queue_insert()
  {
    queue<uint64_t> q;
    for( uint64_t i=0;i<200;++i ) q.push( i );
  }

  void queue_insert2()
  {
    queue<X> q;
    X x;
    for( uint64_t i=0;i<200;++i ) q.push( x );
  }

  void queue_pushpop()
  {
    queue<uint64_t> q;
    for( uint64_t i=0;i<200;++i )
    {
      assert( q.n_items() == i );
      q.push( i );
    }
    uint64_t j=0;
    while( q.n_items() > 0 )
    {
      queue<uint64_t>::handler h;
      assert( q.pop(h) == true );
      assert( *(h.get()) == j );
      ++j;
    }
    assert( j == 200 );
    assert( q.n_items() == 0 );
    assert( q.size() == 0 );
  }

  void queue_pushpop2()
  {
    queue<X> q;
    X x;
    for( uint64_t i=0;i<200;++i )
    {
      assert( q.n_items() == i );
      q.push( x );
    }
    while( q.n_items() > 0 )
    {
      queue<X>::handler h;
      assert( q.pop(h) == true );
      assert( h->x == 0xDeadBeef );
    }
    assert( q.n_items() == 0 );
    assert( q.size() == 0 );
  }

  void baseline_stdlist()
  {
    std::list<uint64_t> q;
  }

  void baseline_stdlist2()
  {
    std::list<X> q;
  }

  void stdlist_insert()
  {
    std::list<uint64_t> q;
    for( uint64_t i=0;i<200;++i ) q.push_back( i );
  }

  void stdlist_insert2()
  {
    std::list<X> q;
    X x;
    for( uint64_t i=0;i<200;++i ) q.push_back( x );
  }

  void stdlist_pushpop()
  {
    std::list<uint64_t> q;
    for( uint64_t i=0;i<200;++i )
    {
      assert( q.size() == i );
      q.push_back( i );
    }
    uint64_t j=0;
    while( q.size() > 0 )
    {
      uint64_t x = q.front();
      q.pop_front();
      assert( x == j );
      ++j;
    }
    assert( j == 200 );
    assert( q.size() == 0 );
  }

  void stdlist_pushpop2()
  {
    std::list<X> q;
    X x;
    for( uint64_t i=0;i<200;++i )
    {
      assert( q.size() == i );
      q.push_back( x );
    }
    while( q.size() > 0 )
    {
      X & f(q.front());
      assert( f.x == 0xDeadBeef );
      q.pop_front();
    }
    assert( q.size() == 0 );
  }

} // end of test_queue

using namespace test_queue;

int main()
{
  csl_common_print_results( "baseline_queue    ", csl_common_test_timer_v0(baseline_queue),"" );
  csl_common_print_results( "baseline_stdlist  ", csl_common_test_timer_v0(baseline_stdlist),"" );
  csl_common_print_results( "baseline_queue2   ", csl_common_test_timer_v0(baseline_queue2),"" );
  csl_common_print_results( "baseline_stdlist2 ", csl_common_test_timer_v0(baseline_stdlist2),"" );
  csl_common_print_results( "queue_insert      ", csl_common_test_timer_v0(queue_insert),"" );
  csl_common_print_results( "stdlist_insert    ", csl_common_test_timer_v0(stdlist_insert),"" );
  csl_common_print_results( "queue_insert2     ", csl_common_test_timer_v0(queue_insert2),"" );
  csl_common_print_results( "stdlist_insert2   ", csl_common_test_timer_v0(stdlist_insert2),"" );
  csl_common_print_results( "queue_pushpop     ", csl_common_test_timer_v0(queue_pushpop),"" );
  csl_common_print_results( "stdlist_pushpop   ", csl_common_test_timer_v0(stdlist_pushpop),"" );
  csl_common_print_results( "queue_pushpop2    ", csl_common_test_timer_v0(queue_pushpop2),"" );
  csl_common_print_results( "stdlist_pushpop2  ", csl_common_test_timer_v0(stdlist_pushpop2),"" );
  return 0;
}

// EOF
