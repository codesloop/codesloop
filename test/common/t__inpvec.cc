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
   @brief Tests to verify in-place vector
 */

#if 0
#ifndef DEBUG
#define DEBUG
#endif /* DEBUG */
#endif

#include "codesloop/common/inpvec.hh"
#include "codesloop/common/test_timer.h"
#include "codesloop/common/ustr.hh"
#include "codesloop/common/logger.hh"
#include "codesloop/common/common.h"
#include <assert.h>
#include <vector>
#include <string>

using namespace csl::common;

/** @brief contains tests related to in-place vector */
namespace test_inpvec {

  static inline const wchar_t * get_namespace()   { return L"test_inpvec"; }
  static inline const wchar_t * get_class_name()  { return L"test_inpvec::noclass"; }
  static inline const wchar_t * get_class_short() { return L"noclass"; }

  void baseline()
  {
    inpvec<uint64_t> ipv;
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
      vec.push_back( s );
      assert( vec.get(i) == "Hello world" );
      //assert( vec.n_items() == i+1 );
      //vec.set(i,s);
      //vec.set(i/2,s);
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
      //assert( vec.size() == i+1 );
      //vec[i] = s;
      //vec[i/2] = s;
    }
  }

  void stds_stdvec(int p)
  {
    std::vector<std::string> vec;
    std::string s("Hello world");

    for( unsigned int i=0;i<static_cast<unsigned int>(p);++i )
    {
      vec.push_back(s);
      assert( vec[i] == "Hello world" );
      //assert( vec.size() == i+1 );
      //vec[i] = s;
      //vec[i/2] = s;
    }
  }

  void ulli_inpvec(int p)
  {
    inpvec<unsigned long long> vec;

    for( unsigned long long i=0;i<static_cast<unsigned long long>(p);++i )
    {
      vec.push_back( i );
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
      assert( vec[static_cast<size_t>(i)] == i );
      assert( vec.size() == i+1 );
      vec[static_cast<size_t>(i)] = i;
      vec[static_cast<size_t>(i)/2] = i;
    }
  }

  void iter_test(int p)
  {
    inpvec<uint64_t> vec;

    uint64_t i=0;

    for( i=0;i<static_cast<uint64_t>(p);++i )
    {
      vec.push_back(i);
    }

    // vec.debug();

    i=0;
    inpvec<uint64_t>::iterator it=vec.begin();
    const inpvec<uint64_t>::iterator & en(vec.end());

    for( ;it!=en;++it )
    {
      assert( *(*it) == i );
      ++i;
    }
  }

  void iter_std(int p)
  {
    std::vector<uint64_t> vec;

    uint64_t i=0;

    for( i=0;i<static_cast<uint64_t>(p);++i )
    {
      vec.push_back(i);
    }

    // vec.debug();

    i=0;
    std::vector<uint64_t>::iterator it=vec.begin();
    const std::vector<uint64_t>::iterator & en(vec.end());

    for( ;it!=en;++it )
    {
      assert( (*it) == i );
      ++i;
    }
  }

  void get_iter(int p)
  {
    inpvec<uint64_t> vec;

    uint64_t i=0;

    for( i=0;i<static_cast<uint64_t>(p);++i )
    {
      vec.push_back(i);
    }

    for( i=0;i<static_cast<uint64_t>(p);++i )
    {
      inpvec<uint64_t>::iterator it=vec.iterator_at(i);
      assert( (*it)[0] == i );
    }

    assert( vec.n_items() == static_cast<uint64_t>(p) );
  }

  /////////////////////////
  // testing functionality
  /////////////////////////

  void fun_push_back()
  {
    inpvec<uint64_t> vec;
    assert( vec.size() > 0 );
    assert( vec.n_items() == 0 );

    inpvec<uint64_t>::iterator i = vec.last_free();
    assert( i != vec.end() );
    assert( i != vec.begin() );
    assert( vec.begin() == vec.end() );

    // check iterators of an empty vector
    assert( vec.iterator_pos( i ) == 0 );
    assert( vec.iterator_pos( vec.begin() ) == 0 );
    assert( vec.iterator_pos( vec.end() ) == 0 );

    // default construct an item
    assert( i.construct() != 0 );

    // check vector's size
    assert( vec.n_items() == 1 );

    // check item after replacing its data
    assert( i.set(123ULL) != 0 );
    assert( vec.n_items() == 1 );

    // begin should return a non-end iterator
    assert( vec.begin() != vec.end() );
    i = vec.iterator_at( 0 );
    assert( (*i)[0] == 123ULL );
    assert( vec.get(0) == 123ULL );

    // move iterator to the end of vector
    ++i;
    assert( i == vec.end() );

    // check invalid free_at
    assert( vec.free_at(100) == false );
    assert( vec.n_items() == 1 );

    // check valid free_at
    assert( vec.free_at(0) == true );
    assert( vec.n_items() == 0 );
  }

  // TODO : test inpvec functionality:
  // - push_back() w/ non default constructor arguments
  // - last_free()
  // - iterator : set() w/ non default constructor arguments
  // - iterator : free() and double free()
  // - iterator : is_empty()
  // - iterator : operator() *
  // - iterator : operator++
  // - iterator : constructors
  // force_iterator_at(a), force_iterator_at(a,b)

  void fun_get_set()
  {
    inpvec<uint64_t> vec;

    for( uint64_t i=0;i<1000;++i )
    {
      assert( vec.last_free_pos() == i );
      assert( vec.is_free_at( i ) == true );
      assert( vec.iterator_at( i ) == vec.end() || vec.iterator_at( i ).is_empty() == true  );
      vec.set( i,i );
      assert( vec.get_ptr( i ) != 0 );
      assert( vec.get_ptr( i )[0] == i );
      assert( vec.is_free_at( i ) == false );
      assert( vec.iterator_at( i ).is_empty() == false );
      assert( vec.iterator_at( i+1 ).is_empty() == true );
      assert( vec.n_items() == (i+1) );
      assert( vec.iterator_pos(vec.iterator_at(i)) == i );
      assert( vec.last_free_pos() == i+1 );
    }

    for( uint64_t i=2000;i<4000;i+=3 )
    {
      assert( vec.is_free_at( i ) == true );
      assert( vec.iterator_at( i ) == vec.end() || vec.iterator_at( i ).is_empty() == true );
      vec.set( i,i );
      assert( vec.get_ptr( i )[0] == i );
      assert( vec.is_free_at( i ) == false );
      assert( vec.iterator_at( i ).is_empty() == false );
      assert( vec.iterator_at( i+1 ).is_empty() == true );
    }
  }

  void test_next_used()
  {
    inpvec<uint64_t> vec;

    CSL_DEBUGF(L"\n\nsetting: 2,99\n\n");
    vec.set( 2,99 );

#ifdef DEBUG
    vec.debug();
#endif /*DEBUG*/

    CSL_DEBUGF(L"\n\nsetting: 100,9999\n\n");
    vec.set( 100,9999 );

#ifdef DEBUG
    vec.debug();
#endif /*DEBUG*/

    CSL_DEBUGF(L"\n\nsetting: 1000,888888\n\n");
    vec.set( 1000,888888 );

#ifdef DEBUG
    vec.debug();
#endif /*DEBUG*/

    inpvec<uint64_t>::iterator it = vec.begin();
    uint64_t * x = 0;

    CSL_DEBUGF(L"\n\nnext_used\n\n");
    while( (x=it.next_used()) != 0 )
    {
      CSL_DEBUGF(L"next_used returned: %p",x);
      CSL_DEBUGF(L"\n\nnext_used\n\n");
    }

    CSL_DEBUGF(L"\n\nsetting: 0,777777777\n\n");
    vec.set( 0,777777777 );

    it = vec.begin();

    CSL_DEBUGF(L"\n\nnext_used B\n\n");
    while( (x=it.next_used()) != 0 )
    {
      CSL_DEBUGF(L"next_used returned: %p:%lld B  [pos=%lld]",x,*x,it.get_pos());
      CSL_DEBUGF(L"\n\nnext_used B\n\n");
    }
  }

} // end of test_inpvec

using namespace test_inpvec;

int main()
{
#ifdef DEBUG
  test_next_used();
  fun_get_set();
#else
  fun_get_set();
  csl_common_print_results( "fun_get_set         ", csl_common_test_timer_v0(fun_get_set),"" );
  csl_common_print_results( "push_back           ", csl_common_test_timer_v0(fun_push_back),"" );

  csl_common_print_results( "itm                 ", csl_common_test_timer_v0(itm),"" );
  csl_common_print_results( "baseline            ", csl_common_test_timer_v0(baseline),"" );

  csl_common_print_results( "get_iter 5          ", csl_common_test_timer_i1(get_iter,5),"" );
  csl_common_print_results( "get_iter 31         ", csl_common_test_timer_i1(get_iter,31),"" );
  csl_common_print_results( "get_iter 150        ", csl_common_test_timer_i1(get_iter,150),"" );
  csl_common_print_results( "get_iter 3000       ", csl_common_test_timer_i1(get_iter,3000),"" );

  csl_common_print_results( "iter_test 5         ", csl_common_test_timer_i1(iter_test,5),"" );
  csl_common_print_results( "iter_test 31        ", csl_common_test_timer_i1(iter_test,31),"" );
  csl_common_print_results( "iter_test 150       ", csl_common_test_timer_i1(iter_test,150),"" );
  csl_common_print_results( "iter_test 3000      ", csl_common_test_timer_i1(iter_test,3000),"" );

  csl_common_print_results( "iter_std 5          ", csl_common_test_timer_i1(iter_std,5),"" );
  csl_common_print_results( "iter_std 31         ", csl_common_test_timer_i1(iter_std,31),"" );
  csl_common_print_results( "iter_std 150        ", csl_common_test_timer_i1(iter_std,150),"" );
  csl_common_print_results( "iter_std 3000       ", csl_common_test_timer_i1(iter_std,3000),"" );

  csl_common_print_results( "ustr_inpvec 5       ", csl_common_test_timer_i1(ustr_inpvec,5),"" );
  csl_common_print_results( "ustr_inpvec 31      ", csl_common_test_timer_i1(ustr_inpvec,31),"" );
  csl_common_print_results( "ustr_inpvec 150     ", csl_common_test_timer_i1(ustr_inpvec,150),"" );
  csl_common_print_results( "ustr_inpvec 3000    ", csl_common_test_timer_i1(ustr_inpvec,3000),"" );

  csl_common_print_results( "ustr_stdvec 5       ", csl_common_test_timer_i1(ustr_stdvec,5),"" );
  csl_common_print_results( "ustr_stdvec 31      ", csl_common_test_timer_i1(ustr_stdvec,31),"" );
  csl_common_print_results( "ustr_stdvec 150     ", csl_common_test_timer_i1(ustr_stdvec,150),"" );
  csl_common_print_results( "ustr_stdvec 3000    ", csl_common_test_timer_i1(ustr_stdvec,3000),"" );

  csl_common_print_results( "stds_stdvec 5       ", csl_common_test_timer_i1(stds_stdvec,5),"" );
  csl_common_print_results( "stds_stdvec 31      ", csl_common_test_timer_i1(stds_stdvec,31),"" );
  csl_common_print_results( "stds_stdvec 150     ", csl_common_test_timer_i1(stds_stdvec,150),"" );
  csl_common_print_results( "stds_stdvec 3000    ", csl_common_test_timer_i1(stds_stdvec,3000),"" );

  csl_common_print_results( "ulli_inpvec 5       ", csl_common_test_timer_i1(ulli_inpvec,5),"" );
  csl_common_print_results( "ulli_inpvec 31      ", csl_common_test_timer_i1(ustr_inpvec,31),"" );
  csl_common_print_results( "ulli_inpvec 150     ", csl_common_test_timer_i1(ustr_inpvec,150),"" );
  csl_common_print_results( "ulli_inpvec 3000    ", csl_common_test_timer_i1(ulli_inpvec,3000),"" );

  csl_common_print_results( "ulli_stdvec 5       ", csl_common_test_timer_i1(ulli_stdvec,5),"" );
  csl_common_print_results( "ulli_stdvec 31      ", csl_common_test_timer_i1(ustr_stdvec,31),"" );
  csl_common_print_results( "ulli_stdvec 150     ", csl_common_test_timer_i1(ustr_stdvec,150),"" );
  csl_common_print_results( "ulli_stdvec 3000    ", csl_common_test_timer_i1(ulli_stdvec,3000),"" );
#endif
  return 0;
}

/* EOF */
