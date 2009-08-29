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
   @file t__hash_helpers.cc
   @brief Tests to verify hash related macros
 */

//#if 0
#ifndef DEBUG
#define DEBUG_ENABLE_INDENT
#define DEBUG
#endif /* DEBUG */
//#endif

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

  static inline const wchar_t * get_namespace()   { return L"test_hash_helpers"; }
  static inline const wchar_t * get_class_name()  { return L"test_hash_helpers::noclass"; }
  static inline const wchar_t * get_class_short() { return L"noclass"; }

  void page_split()
  {
    typedef page<uint64_t,uint64_t> page_t;
    page_t p;

    for( uint64_t i=0;i<page_t::sz_;++i )
    {
      int r = p.add( i%(page_t::sz_),i,i,i%(page_t::sz_) );
      assert(  r == page_t::ok_ );
    }

    page_t::page_vec_t    res;
    pos_vec_t             ids;
    uint32_t              shift=0;
    page_t *              pg = res.construct(0);

    assert( p.n_items() == page_t::sz_ );

    p.split(3,0,*pg,0,ids);

    assert( ids.n_items() == 1 );

    // TODO more tests here
  }

  void index_getset()
  {
    hash_helpers::index idx;

    uint64_t i;
    uint64_t tpos;
    bool page,tpage;

    for( i=0;i<128;++i )
    {
      /* check that a get to empty value returns false */
      assert( idx.internal_get( i,tpos,page ) == false );

      /* flip-flop */
      if( (i&1ULL) == 0 ) page = true;
      else                page = false;

      /* set a new value */
      idx.internal_set( i,i,page );

      /* retrieve the value */
      assert( idx.internal_get( i,tpos,tpage ) == true );

      /* check data */
      assert( page == tpage );
      assert( tpos == i );

#ifdef DEBUG
      idx.debug();
#endif /*DEBUG*/
    }
  }

  void page_add(int n)
  {
    page<int,int> c;
    for( int i=0;i<n;++i )
    {
      c.add( i%3, i,i, i%3 );
      //assert( c.n_items() == static_cast<size_t>(i+1) );
#ifdef DEBUG
      c.debug();
#endif /*DEBUG*/
    }
  }

  void index_lookup_pp()
  {
    hash_helpers::index idx;
    uint64_t shift = 0;

    assert( idx.lookup_pagepos_for_hashkey(0,shift) == hash_helpers::index::not_found_ );

    /* set a link to the new block */
    idx.internal_set( 0,1,false );
    assert( idx.lookup_pagepos_for_hashkey(0,shift) == hash_helpers::index::not_found_ );

#ifdef DEBUG
    idx.debug();
#endif /*DEBUG*/

    /* set a link to the new block */
    idx.internal_set( hash_helpers::index::sz_,2,false );
    assert( idx.lookup_pagepos_for_hashkey(0,shift) == hash_helpers::index::not_found_ );

#ifdef DEBUG
    idx.debug();
#endif /*DEBUG*/

    idx.internal_set( hash_helpers::index::sz_*2,33,true );
    assert( idx.lookup_pagepos_for_hashkey(0,shift) == hash_helpers::index::sz_*2 );

#ifdef DEBUG
    idx.debug();
#endif /*DEBUG*/
  }

  void index_split()
  {
    hash_helpers::index      idx;
    hash_helpers::pos_vec_t  pv,pv2;
    uint64_t                 shift=0;

    CSL_DEBUGF(L"index_split() : set one page \n\n");

    idx.internal_set( 2,1,true );

#ifdef DEBUG
    idx.debug();
#endif /*DEBUG*/

    CSL_DEBUGF(L"index_split() : set two more pages {9,11} to be added as split\n\n");

    pv.set( 9,9 );
    pv.set( 11,11 );

    CSL_DEBUGF(L"index_split() : split A => {9,11} \n\n");

    idx.split( 2,shift,pv );

#ifdef DEBUG
    idx.debug();
#endif /*DEBUG*/

    CSL_DEBUGF(L"index_split() : set three more pages {7,21,23} to be added as split\n\n");

    pv2.set( 7,7 );
    pv2.set( 21,21 );
    pv2.set( 23,23 );

    CSL_DEBUGF(L"index_split() : split to three pages B[2] => {7,21,23} \n\n");

    idx.split( (9ULL<<(hash_helpers::index::bits_))+2ULL,shift,pv2 );

#ifdef DEBUG
    idx.debug();
#endif /*DEBUG*/

  }

  void index_get0()
  {
    hash_helpers::index idx;
    uint64_t d0,d1;
    for( uint64_t i=0;i<100;++i ) idx.get(i,d0,d1);
  }

  void create_page0()
  {
    typedef page<uint64_t,uint64_t> page_t;
    page_t::page_vec_t pv;

    for( uint64_t i=0;i<100;++i )
    {
      page_t::page_vec_t::iterator it(pv.last_free());
      page_t * ret = it.construct();
      uint64_t pgid = pv.iterator_pos( it );
    }
  }

  void page_has_item0()
  {
    typedef page<uint64_t,uint64_t> page_t;
    page_t p;

    for( uint64_t i=0;i<100;++i )
    {
      p.has_item(i);
    }
  }

  void page_add0()
  {
    typedef page<uint64_t,uint64_t> page_t;
    page_t p;

    for( uint64_t i=0;i<100;++i )
    {
      p.add(i,i,i,i);
    }
  }

} // end of test_hash_helpers

using namespace test_hash_helpers;

int main()
{

#ifdef DEBUG
  page_add0();
  //index_split();
  //index_lookup_pp();
  //page_add(9);
  //page_split();
  //index_getset();
#else

  csl_common_print_results( "page add 0                   ", csl_common_test_timer_v0(page_add0),"" );
  csl_common_print_results( "page has_item 0              ", csl_common_test_timer_v0(page_has_item0),"" );
  csl_common_print_results( "create page 0                ", csl_common_test_timer_v0(create_page0),"" );
  csl_common_print_results( "index get 0                  ", csl_common_test_timer_v0(index_get0),"" );

  csl_common_print_results( "index lookup page pos        ", csl_common_test_timer_v0(index_lookup_pp),"" );
  csl_common_print_results( "index split                  ", csl_common_test_timer_v0(index_split),"" );
  csl_common_print_results( "index getset (internal)      ", csl_common_test_timer_v0(index_getset),"" );

  csl_common_print_results( "page_split                   ", csl_common_test_timer_v0(page_split),"" );

  csl_common_print_results( "page_add (1)                 ", csl_common_test_timer_i1(page_add,1),"" );
  csl_common_print_results( "page_add (2)                 ", csl_common_test_timer_i1(page_add,2),"" );
  csl_common_print_results( "page_add (3)                 ", csl_common_test_timer_i1(page_add,3),"" );
  csl_common_print_results( "page_add (4)                 ", csl_common_test_timer_i1(page_add,4),"" );
  csl_common_print_results( "page_add (5)                 ", csl_common_test_timer_i1(page_add,5),"" );

  csl_common_print_results( "baseline (contained)         ", csl_common_test_timer_v0(baseline_contained),"" );
  csl_common_print_results( "baseline (page)              ", csl_common_test_timer_v0(baseline_page),"" );
#endif
  return 0;
}

/* EOF */
