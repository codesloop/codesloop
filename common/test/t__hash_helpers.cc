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
   @file t__hash_helpers.cc
   @brief Tests to verify hash related macros
 */

#include "hash.hh"
#include "hash_helpers.hh"
#include "inpvec.hh"
#include "test_timer.h"
#include "common.h"
#include <assert.h>

using namespace csl::common;
using namespace csl::common::hash_helpers;

/** @brief contains tests related to hash */
namespace test_hash_helpers {

  void baseline_contained()
  {
    contained<uint64_t,uint64_t> c;
  }

  void baseline_page()
  {
    page<uint64_t,uint64_t> c;
  }

  void page_add(int n)
  {
    page<int,int> c;
    for( int i=0;i<n;++i )
    {
      c.add( i%(page<int,int>::sz_), i,i, i%(page<int,int>::sz_) );
      //assert( c.n_items() == static_cast<size_t>(i+1) );
    }
  }

  static inline const wchar_t * get_namespace()   { return L"test_hash_helpers"; }
  static inline const wchar_t * get_class_name()  { return L"test_hash_helpers::noclass"; }
  static inline const wchar_t * get_class_short() { return L"noclass"; }

  void page_split()
  {
    typedef page<uint64_t,uint64_t> page_t;
    page_t p;

    for( uint64_t i=0;i<page_t::sz_;++i )
    {
      int r = p.add( i%32,i,i,i%32 );
      assert(  r == page_t::ok_ );
    }

    page_t::page_vec_t    res;
    page_t::pos_vec_t     ids;
    uint32_t              shift=0;

    assert( p.n_items() == page_t::sz_ );

    p.split(shift,res,ids);

    // CSL_DEBUGF( L"Page size: %ld [res=%ld ids=%ld]", p.data_.n_items(),res.n_items(),ids.n_items() );
    // CSL_DEBUGF( L"hash_t::page::max_size_hint_ =  %d", hash_t::page::max_size_hint_ );

    // the split creates n-1 pages evenly spreading the keys into them
    assert( res.n_items() == (page_t::sz_ - 1) );

    // each page is created in the res vector, and its page ids (indexes) are stored
    // into the ids vector
    assert( ids.n_items() == (page_t::sz_ - 1) );

    page_t::page_vec_t::iterator it = res.begin();
    page_t::page_vec_t::iterator en = res.begin();

    for( ;it!=en;++it )
    {
      assert( (*it)->n_items() == 1 );
    }
  }

} // end of test_hash_helpers

using namespace test_hash_helpers;

int main()
{
  csl_common_print_results( "page_split                   ", csl_common_test_timer_v0(page_split),"" );

  csl_common_print_results( "page_add (1)                 ", csl_common_test_timer_i1(page_add,1),"" );
  csl_common_print_results( "page_add (2)                 ", csl_common_test_timer_i1(page_add,2),"" );
  csl_common_print_results( "page_add (3)                 ", csl_common_test_timer_i1(page_add,3),"" );
  csl_common_print_results( "page_add (4)                 ", csl_common_test_timer_i1(page_add,4),"" );
  csl_common_print_results( "page_add (5)                 ", csl_common_test_timer_i1(page_add,5),"" );

  csl_common_print_results( "baseline (contained)         ", csl_common_test_timer_v0(baseline_contained),"" );
  csl_common_print_results( "baseline (page)              ", csl_common_test_timer_v0(baseline_page),"" );

  return 0;
}

/* EOF */
