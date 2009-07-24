/*
Copyright (c) 2008,2009, David Beck, Tamas Foldi

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
   @brief Tests to verify in-place vector
 */

#include "inpvec.hh"
#include "test_timer.h"
#include "ustr.hh"
#include "common.h"
#include <assert.h>
#include <vector>

using namespace csl::common;

/** @brief contains tests related to in-place vector */
namespace test_inpvec {

  void baseline()
  {
    inpvec<ustr> ipv;
  }

  void itm()
  {
    inpvec<ustr>::item i;
    i.mul_alloc(1);
    //assert( i.get(0) == 0 );
    ustr s("Hello world");
    i.set(0,s);
    //assert( i.get(0) != 0 );
    assert( i.get(0) == "Hello world" );
    assert( i.n_items() == 1 );
  }

  void ustr_inpvec(int p)
  {
    inpvec<ustr> vec;
    ustr s("Hello world");

    for( unsigned int i=0;i<static_cast<unsigned int>(p);++i )
    {
      vec.set( vec.n_items(),s );
      assert( vec.get(i) == "Hello world" );
      assert( vec.n_items() == i+1 );
      vec.set(i,s);
      vec.set(i/2,s);
    }
  }

  void ustr_stdvec(int p)
  {
    std::vector<ustr> vec;
    ustr s("Hello world");

    for( unsigned int i=0;i<static_cast<unsigned int>(p);++i )
    {
      vec.push_back(s);
      assert( vec[i] == "Hello world" );
      assert( vec.size() == i+1 );
      vec[i] = s;
      vec[i/2] = s;
    }
  }

  void ulli_inpvec(int p)
  {
    inpvec<unsigned long long> vec;

    for( unsigned long long i=0;i<static_cast<unsigned long long>(p);++i )
    {
      vec.set( vec.n_items(),i );
      assert( vec.get(i) == i );
      assert( vec.n_items() == i+1 );
      vec.set(i,i);
      vec.set(i/2,i);
    }
  }

  void ulli_stdvec(int p)
  {
    std::vector<unsigned long long> vec;

    for( unsigned long long i=0;i<static_cast<unsigned long long>(p);++i )
    {
      vec.push_back( i );
      assert( vec[i] == i );
      assert( vec.size() == i+1 );
      vec[i] = i;
      vec[i/2] = i;
    }
  }
} // end of test_inpvec

using namespace test_inpvec;

int main()
{

  csl_common_print_results( "ustr_inpvec 5       ", csl_common_test_timer_i1(ustr_inpvec,5),"" );
  csl_common_print_results( "ustr_stdvec 5       ", csl_common_test_timer_i1(ustr_stdvec,5),"" );

  csl_common_print_results( "ustr_inpvec 50      ", csl_common_test_timer_i1(ustr_inpvec,50),"" );
  csl_common_print_results( "ustr_stdvec 50      ", csl_common_test_timer_i1(ustr_stdvec,50),"" );

  csl_common_print_results( "ustr_inpvec 3000    ", csl_common_test_timer_i1(ustr_inpvec,3000),"" );
  csl_common_print_results( "ustr_stdvec 3000    ", csl_common_test_timer_i1(ustr_stdvec,3000),"" );

  csl_common_print_results( "ulli_inpvec 5       ", csl_common_test_timer_i1(ulli_inpvec,5),"" );
  csl_common_print_results( "ulli_stdvec 5       ", csl_common_test_timer_i1(ulli_stdvec,5),"" );

  csl_common_print_results( "ulli_inpvec 50      ", csl_common_test_timer_i1(ustr_inpvec,50),"" );
  csl_common_print_results( "ulli_stdvec 50      ", csl_common_test_timer_i1(ustr_stdvec,50),"" );

  csl_common_print_results( "ulli_inpvec 3000    ", csl_common_test_timer_i1(ulli_inpvec,3000),"" );
  csl_common_print_results( "ulli_stdvec 3000    ", csl_common_test_timer_i1(ulli_stdvec,3000),"" );

  csl_common_print_results( "itm                 ", csl_common_test_timer_v0(itm),"" );
  csl_common_print_results( "baseline            ", csl_common_test_timer_v0(baseline),"" );

  return 0;
}

/* EOF */
