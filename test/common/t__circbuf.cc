/*
Copyright (c) 2008,2009,2010, CodeSLoop Team

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
   @file t__circbuf.cc
   @brief Tests to verify circbuf
 */

#include "codesloop/common/circbuf.hh"
#include "codesloop/common/test_timer.h"
#include "codesloop/common/common.h"
#include <assert.h>
#include <list>

using namespace csl::common;

/** @brief contains tests related to circbuf */
namespace test_circbuf {

  /** @test performance baseline */
  void baseline()
  {
    circbuf<int,40> cb;
  }

  void stdlist()
  {
    std::list<int> ls;
  }

  static circbuf<int,40> cb_;

  void circpush()
  {
    int & i = cb_.push();
    i = 0;
  }

  void prepcomm_push()
  {
    int & i = cb_.prepare();
    i = 0;
    cb_.commit( i );
  }

  static std::list<int> ls_;

  void listpush()
  {
    ls_.push_back(0);
    if( ls_.size() > 40 ) ls_.pop_front();
  }

  void circtst()
  {
    circbuf<int,40> cb;
    for( int i=0;i<40;++i )
    {
      int & it = cb.push();
      it = i;
      assert( cb.newest() == i );
      assert( cb.oldest() == 0 );
      assert( cb.size() == static_cast<unsigned int>(i+1) );
    }

    for( int i=0;i<40;++i )
    {
      int & it = cb.pop();
      assert( it == i );
      assert( cb.n_items() == 40U-1-i );
    }
  }

  void listtst()
  {
    std::list<int> ls;

    for( int i=0;i<40;++i )
    {
      ls.push_back(i);
      assert( ls.back() == i );
      assert( ls.front() == 0 );
      assert( ls.size() == static_cast<unsigned int>(i+1) );
    }

    for( int i=0;i<40;++i )
    {
      int it = ls.front();
      ls.pop_front();
      assert( it == i );
      assert( ls.size() == 40U-1-i );
    }
  }

  void prepcomm()
  {
    circbuf<int,40> cb;
    for( int i=0;i<40;++i )
    {
      int & it = cb.prepare();
      it = i;
      cb.commit( it );
      assert( cb.newest() == i );
      assert( cb.oldest() == 0 );
      assert( cb.n_items() == static_cast<unsigned int>(i+1) );
    }

    for( int i=0;i<40;++i )
    {
      int & it = cb.pop();
      assert( it == i );
      assert( cb.n_items() == 40U-1-i );
    }
  }

  void testrollback()
  {
    // TODO
  }

} // end of namespace test_circbuf

using namespace test_circbuf;

int main()
{
  csl_common_print_results( "baseline       ", csl_common_test_timer_v0(baseline),"" );
  csl_common_print_results( "stdlist        ", csl_common_test_timer_v0(stdlist),"" );
  csl_common_print_results( "circpush       ", csl_common_test_timer_v0(circpush),"" );
  csl_common_print_results( "prepcomm_push  ", csl_common_test_timer_v0(prepcomm_push),"" );
  csl_common_print_results( "listpush       ", csl_common_test_timer_v0(listpush),"" );
  csl_common_print_results( "circtst        ", csl_common_test_timer_v0(circtst),"" );
  csl_common_print_results( "listtst        ", csl_common_test_timer_v0(listtst),"" );
  csl_common_print_results( "prepcomm       ", csl_common_test_timer_v0(prepcomm),"" );
  return 0;
}

/* EOF */
