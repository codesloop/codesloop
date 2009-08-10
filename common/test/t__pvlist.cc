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
   @file t__pvlist.cc
   @brief Tests to verify pvlist
*/

#ifndef DEBUG
#define DEBUG
#endif /* DEBUG */

#include "pvlist.hh"
#include "test_timer.h"
#include "common.h"
#include <vector>
#include <assert.h>
#ifdef TEST_BOOST_POOL
#  include <boost/pool/object_pool.hpp>
#endif

using namespace csl::common;

/** @brief contains tests related to pvlist */
namespace test_pvlist {

  /** @test nop_destructor (reference for performance testing) */
  void test_nop_destructor_ref()
  {
    pvlist< 32,void,nop_destructor<void> > pv;
    pv.free_all();
  }

  /**
   @test to verify nop_destructor works with 100 pointers and does not free() them
   */
  void test_nop_destructor_100p()
  {
    char t[100];
    pvlist< 32,void,nop_destructor<void> > pv;
    for( int i=0;i<100;++i )
    {
      pv.push_back( t+i );
    }
    pv.free_all();
  }

  /** @test free_destructor (reference for performance testing) */
  void test_free_destructor_ref()
  {
    pvlist< 32,void,free_destructor<void> > pv;
    pv.free_all();
  }

  /** @test to verify free_destructor frees 100 malloced pointers */
  void test_free_destructor_100p()
  {
    pvlist< 32,void,free_destructor<void> > pv;
    for( int i=0;i<100;++i )
    {
      pv.push_back( malloc(4) );
    }
    pv.free_all();
  }

  /** @test delete_destructor (reference for performance testing) */
  void test_delete_destructor_ref()
  {
    pvlist< 32,char,delete_destructor<char> > pv;
    pv.free_all();
  }

  /** @test to verify delete_destructor frees 100 new()-d pointers */
  void test_delete_destructor_100p()
  {
    pvlist< 32,char,delete_destructor<char> > pv;
    for( int i=0;i<100;++i )
    {
      pv.push_back( new char );
    }
    pv.free_all();
  }

  /** @test to verify bs() returns the right blocksize */
  void test_bs()
  {
    pvlist< 32,char,nop_destructor<char> > pv;
    assert( pv.bs() == 32 );
  }

  /** @test get_at() */
  void test_get_at()
  {
    pvlist< 32,char,nop_destructor<char> > pv;

    for( int i=0;i<100;++i )
    {
      pv.push_back( reinterpret_cast<char *>(i) );
      assert( pv.n_items() == static_cast<size_t>(i+1) );
    }

    for( int i=0;i<100;++i )
    {
      assert( pv.get_at(i) == reinterpret_cast<char *>(i) );
    }
  }

  /** @test set_at() and get_at() */
  void test_set_get_at()
  {
    pvlist< 32,char,nop_destructor<char> > pv;

    for( int i=0;i<100;++i )
    {
      pv.push_back( reinterpret_cast<char *>(i) );
      assert( pv.set_at( i,reinterpret_cast<char *>(i+1) ) == true );
      if( pv.get_at(i) != reinterpret_cast<char *>(i+1) )
      {
        printf( "%d %p %p [nitems=%ld]\n",
          i,
          pv.get_at(i),
          reinterpret_cast<char *>(i+1),
          static_cast<unsigned long>(pv.n_items()) );

        pv.debug();
      }
      assert( pv.get_at(i) == reinterpret_cast<char *>(i+1) );
      assert( pv.n_items() == static_cast<size_t>(i+1) );
    }
  }

  /** @test free() -- nop_destructor */
  void test_free_nop()
  {
    char t[2];
    pvlist< 32,char,nop_destructor<char> > pv;

    for( int i=0;i<50;++i )
    {
      pv.push_back( t );
      pv.push_back( t+1 );
    }

    for( int i=0;i<100;++i )
    {
      assert( pv.get_at(i) != 0 );
    }

    assert( pv.n_items() == 100 );
    pv.free( t );
    pv.free( t+1 );
    pv.free( t+2 );
    assert( pv.n_items() == 100 );

    for( int i=0;i<100;++i )
    {
      assert( pv.get_at(i) == 0 );
    }
  }

  /** @test free() -- free_destructor */
  void test_free_free()
  {
    char * t[2];
    pvlist< 32,char,free_destructor<char> > pv;

    t[0] = reinterpret_cast<char *>(malloc(1));
    t[1] = reinterpret_cast<char *>(malloc(1));

    for( int i=0;i<50;++i )
    {
      pv.push_back( t[0] );
      pv.push_back( t[1] );
    }

    for( int i=0;i<100;++i )
    {
      assert( pv.get_at(i) != 0 );
    }

    assert( pv.n_items() == 100 );
    pv.free( t[0] );
    pv.free( t[1] );
    pv.free( reinterpret_cast<char *>(8) );
    assert( pv.n_items() == 100 );

    for( int i=0;i<100;++i )
    {
      assert( pv.get_at(i) == 0 );
    }
  }

  /** @test free() -- delete_destructor */
  void test_free_delete()
  {
    char * t[2];
    pvlist< 32,char,delete_destructor<char> > pv;

    t[0] = new char;
    t[1] = new char;

    for( int i=0;i<50;++i )
    {
      pv.push_back( t[0] );
      pv.push_back( t[1] );
    }

    for( int i=0;i<100;++i )
    {
      assert( pv.get_at(i) != 0 );
    }

    assert( pv.n_items() == 100 );
    pv.free( t[0] );
    pv.free( t[1] );
    pv.free( reinterpret_cast<char *>(8) );
    assert( pv.n_items() == 100 );

    for( int i=0;i<100;++i )
    {
      assert( pv.get_at(i) == 0 );
    }
  }

  /** @test free_one() -- nop_destructor */
  void test_free_one_nop()
  {
    char t[2];
    pvlist< 32,char,nop_destructor<char> > pv;

    for( int i=0;i<50;++i )
    {
      pv.push_back( t );
      pv.push_back( t+1 );
    }

    for( int i=0;i<100;++i )
    {
      assert( pv.get_at(i) != 0 );
    }

    assert( pv.n_items() == 100 );
    pv.free_one( t );
    pv.free_one( t+1 );
    pv.free_one( t+2 );
    assert( pv.n_items() == 100 );

    assert( pv.get_at(0) == 0 );
    assert( pv.get_at(1) == 0 );

    for( int i=2;i<100;++i )
    {
      assert( pv.get_at(i) != 0 );
      pv.set_at(i,0);
    }
  }

  /** @test free_one() -- free_destructor */
  void test_free_one_free()
  {
    char * t[2];
    pvlist< 32,char,free_destructor<char> > pv;

    t[0] = reinterpret_cast<char *>(malloc(1));
    t[1] = reinterpret_cast<char *>(malloc(1));

    for( int i=0;i<50;++i )
    {
      pv.push_back( t[0] );
      pv.push_back( t[1] );
    }

    for( int i=0;i<100;++i )
    {
      assert( pv.get_at(i) != 0 );
    }

    assert( pv.n_items() == 100 );
    pv.free_one( t[0] );
    pv.free_one( t[1] );
    pv.free_one( reinterpret_cast<char *>(8) );
    assert( pv.n_items() == 100 );

    assert( pv.get_at(0) == 0 );
    assert( pv.get_at(1) == 0 );

    for( int i=2;i<100;++i )
    {
      assert( pv.get_at(i) != 0 );
      pv.set_at(i,0);
    }
  }

  /** @est free_one() -- delete_destructor */
  void test_free_one_delete()
  {
    char * t[2];
    pvlist< 32,char,delete_destructor<char> > pv;

    t[0] = new char;
    t[1] = new char;

    for( int i=0;i<50;++i )
    {
      pv.push_back( t[0] );
      pv.push_back( t[1] );
    }

    for( int i=0;i<100;++i )
    {
      assert( pv.get_at(i) != 0 );
    }

    assert( pv.n_items() == 100 );
    pv.free_one( t[0] );
    pv.free_one( t[1] );
    pv.free_one( reinterpret_cast<char *>(8) );
    assert( pv.n_items() == 100 );

    assert( pv.get_at(0) == 0 );
    assert( pv.get_at(1) == 0 );

    for( int i=2;i<100;++i )
    {
      assert( pv.get_at(i) != 0 );
      pv.set_at(i,0);
    }
  }

  /** @test free_all() -- nop_destructor */
  void test_free_all_nop()
  {
    char t[2];
    pvlist< 32,char,nop_destructor<char> > pv;

    for( int j=0;j<10;++j )
    {
      for( int i=0;i<50;++i )
      {
        pv.push_back( t );
        pv.push_back( t+1 );
      }

      assert( pv.n_items() == 100 );
      pv.free_all();
      assert( pv.n_items() == 0 );
    }
  }

  /** @test free_all() -- free_destructor */
  void test_free_all_free()
  {
    pvlist< 32,char,free_destructor<char> > pv;

    for( int j=0;j<10;++j )
    {
      for( int i=0;i<100;++i )
      {
        pv.push_back( reinterpret_cast<char *>(malloc(1)) );
      }

      assert( pv.n_items() == 100 );
      pv.free_all();
      assert( pv.n_items() == 0 );
    }
  }

  /** @test free_all() -- delete_destructor */
  void test_free_all_delete()
  {
    pvlist< 32,char,delete_destructor<char> > pv;

    for( int j=0;j<10;++j )
    {
      for( int i=0;i<100;++i )
      {
        pv.push_back( new char );
      }

      assert( pv.n_items() == 100 );
      pv.free_all();
      assert( pv.n_items() == 0 );
    }
  }

  /** @test iterator */
  void test_iter()
  {
    pvlist< 3,char,nop_destructor<char> > pv;
    unsigned int lk = 0;
    for( int i=0;i<100;++i )
    {
      pv.push_back(reinterpret_cast<char *>(i));
      lk += i;
    }
    pvlist< 3,char,nop_destructor<char> >::iterator it(pv.begin());
    pvlist< 3,char,nop_destructor<char> >::iterator end(pv.end());
    assert( lk > 80 );
    unsigned int lk2 = 0;
    unsigned int ll = 0;

    assert( pv.n_items() == 100 );
    for( ;it!=end;++it )
    {
      unsigned long l = reinterpret_cast<unsigned long>(*it);
      lk2 += l;
      ++ll;
    }
    assert( ll == 100 );
    assert( lk == lk2 );
    assert( lk2 > 80 );
  }

  /** @test iterator */
  void test_iter2()
  {
    pvlist< 32,char,nop_destructor<char> > pv;
    for( int i=0;i<100;++i )
    {
      pv.push_back(reinterpret_cast<char *>(1));
      assert( *(pv.last()) != 0 );
    }
    pvlist< 32,char,nop_destructor<char> >::iterator it(pv.begin());
    pvlist< 32,char,nop_destructor<char> >::iterator end(pv.end());

    unsigned int ll = 0;
    assert( pv.n_items() == 100 );

    for( ;it!=end;++it )
    {
      ++ll;
    }
    assert( ll == 100 );
    assert( pv.last() != pv.begin() );
    assert( pv.last() != pv.end() );
    assert( *(pv.last()) != 0 );
  }

  /** @test perf baseline */
  void perf_baseline()
  {
  }

  /** @test perf empty array */
  void perf_empty_pointer_arrray()
  {
    void * array[64];
  }

  /** @test perf empty pointer vector */
  void perf_empty_pointer_vector()
  {
    std::vector<void *> array;
  }

  /** @test perf empty pvlist */
  void perf_empty_pvlist()
  {
    pvlist< 64,void,nop_destructor<void> > array;
  }

#ifdef TEST_BOOST_POOL
  /** @test perf empty boost pool */
  void perf_empty_boost_pool()
  {
    boost::object_pool<void *> p;
  }
#endif /*TEST_BOOST_POOL*/

  /** @test perf add 64 pointers to array */
  void perf_add64_pointer_arrray()
  {
    void * array[64];
    for( unsigned int i=0;i<64;++i ) array[i] = 0;
  }

  /** @test perf add 64 pointers to pointer vector */
  void perf_add64_pointer_vector_2()
  {
    std::vector<void *> array;
    array.reserve(64);
    for( unsigned int i=0;i<64;++i ) array[i] = 0;
  }

  /** @test perf add 64 pointers to pointer vector #2 */
  void perf_add64_pointer_vector()
  {
    std::vector<void *> array;
    for( unsigned int i=0;i<64;++i ) array.push_back(0);
  }

  /** @test perf add 64 pointers to pvlist */
  void perf_add64_pvlist()
  {
    pvlist< 64,void,nop_destructor<void> > array;
    for( unsigned int i=0;i<64;++i ) array.push_back(0);
  }

#ifdef TEST_BOOST_POOL
  /** @test perf get 64 pointers from boost pool */
  void perf_add64_boost_pool()
  {
    boost::object_pool<void *> p;
    for( unsigned int i=0;i<64;++i ) p.malloc();
  }
#endif /*TEST_BOOST_POOL*/

  /* perf results with g3 (no optimization)
  perf_baseline                  2089.896 ms, 536870910 calls,   0.000004 ms/call,   256888816.476992 calls/sec
  perf_empty_pointer_arrray      2693.552 ms, 536870910 calls,   0.000005 ms/call,   199317076.484879 calls/sec
  perf_empty_pointer_vector      1897.402 ms, 16777214 calls,   0.000113 ms/call,   8842203.180981 calls/sec
  perf_empty_pvlist              2658.475 ms, 67108862 calls,   0.000040 ms/call,   25243367.720216 calls/sec
  perf_empty_boost_pool          1883.067 ms, 33554430 calls,   0.000056 ms/call,   17819031.399308 calls/sec
  perf_add64_pointer_arrray      3005.747 ms,  8388606 calls,   0.000358 ms/call,   2790855.650858 calls/sec
  perf_add64_pointer_vector_2    2493.541 ms,  1048574 calls,   0.002378 ms/call,   420516.045255 calls/sec
  perf_add64_pointer_vector      1739.492 ms,   262142 calls,   0.006636 ms/call,   150700.319404 calls/sec
  perf_add64_pvlist              1851.417 ms,  2097150 calls,   0.000883 ms/call,   1132726.986951 calls/sec
  perf_add64_boost_pool          2564.660 ms,   524286 calls,   0.004892 ms/call,   204427.097549 calls/sec
  */

  /* perf results with O3 (optimization)
  perf_baseline                  2360.328 ms, 536870910 calls,   0.000004 ms/call,   227456061.191495 calls/sec
  perf_empty_pointer_arrray      2288.613 ms, 536870910 calls,   0.000004 ms/call,   234583527.228063 calls/sec
  perf_empty_pointer_vector      2346.472 ms, 536870910 calls,   0.000004 ms/call,   228799197.262955 calls/sec
  perf_empty_pvlist              2335.345 ms, 536870910 calls,   0.000004 ms/call,   229889335.408687 calls/sec
  perf_empty_boost_pool          2196.547 ms, 134217726 calls,   0.000016 ms/call,   61103962.719669 calls/sec
  perf_add64_pointer_arrray      2277.449 ms, 536870910 calls,   0.000004 ms/call,   235733450.013590 calls/sec
  perf_add64_pointer_vector_2    2183.660 ms,  8388606 calls,   0.000260 ms/call,   3841534.854327 calls/sec
  perf_add64_pointer_vector      2147.284 ms,  1048574 calls,   0.002048 ms/call,   488325.717511 calls/sec
  perf_add64_pvlist              1922.388 ms,  8388606 calls,   0.000229 ms/call,   4363638.349802 calls/sec
  perf_add64_boost_pool          1720.609 ms,  2097150 calls,   0.000820 ms/call,   1218841.700816 calls/sec
  */

} // end of namespace test_pvlist

using namespace test_pvlist;

int main()
{
  csl_common_print_results(
    "test_iter                   ",
    csl_common_test_timer_v0(test_iter),"" );

  csl_common_print_results(
    "test_iter2                  ",
    csl_common_test_timer_v0(test_iter2),"" );

  csl_common_print_results( 
    "test_nop_destructor_ref     ",
    csl_common_test_timer_v0(test_nop_destructor_ref),"" );
  
  csl_common_print_results( 
    "test_nop_destructor_100p    ",
    csl_common_test_timer_v0(test_nop_destructor_100p),"" );
  
  csl_common_print_results( 
    "test_free_destructor_ref    ",
    csl_common_test_timer_v0(test_free_destructor_ref),"" );
  
  csl_common_print_results( 
    "test_free_destructor_100p   ",
    csl_common_test_timer_v0(test_free_destructor_100p),"" );

  csl_common_print_results( 
    "test_delete_destructor_ref  ",
    csl_common_test_timer_v0(test_delete_destructor_ref),"" );
  
  csl_common_print_results( 
    "test_delete_destructor_100p ",
    csl_common_test_timer_v0(test_delete_destructor_100p),"" );
  
  csl_common_print_results( 
    "test_bs                     ",
    csl_common_test_timer_v0(test_bs),"" );

  csl_common_print_results( 
    "test_get_at                 ",
    csl_common_test_timer_v0(test_get_at),"" );
  
  csl_common_print_results( 
    "test_set_get_at             ",
    csl_common_test_timer_v0(test_set_get_at),"" );
  
  csl_common_print_results( 
    "test_free_nop               ",
    csl_common_test_timer_v0(test_free_nop),"" );

  csl_common_print_results( 
    "test_free_free              ",
    csl_common_test_timer_v0(test_free_free),"" );

  csl_common_print_results( 
    "test_free_delete            ",
    csl_common_test_timer_v0(test_free_delete),"" );

  csl_common_print_results( 
    "test_free_one_nop           ",
    csl_common_test_timer_v0(test_free_one_nop),"" );

  csl_common_print_results( 
    "test_free_one_free          ",
    csl_common_test_timer_v0(test_free_one_free),"" );

  csl_common_print_results( 
    "test_free_one_delete        ",
    csl_common_test_timer_v0(test_free_one_delete),"" );
  
  csl_common_print_results( 
    "test_free_all_nop           ",
    csl_common_test_timer_v0(test_free_all_nop),"" );

  csl_common_print_results( 
    "test_free_all_free          ",
    csl_common_test_timer_v0(test_free_all_free),"" );

  csl_common_print_results( 
    "test_free_all_delete        ",
    csl_common_test_timer_v0(test_free_all_delete),"" );

  csl_common_print_results( 
    "perf_baseline               ",
    csl_common_test_timer_v0(perf_baseline),"" );

  csl_common_print_results( 
    "perf_empty_pointer_arrray   ",
    csl_common_test_timer_v0(perf_empty_pointer_arrray),"" );

  csl_common_print_results( 
    "perf_empty_pointer_vector   ",
    csl_common_test_timer_v0(perf_empty_pointer_vector),"" );

  csl_common_print_results( 
    "perf_empty_pvlist           ",
    csl_common_test_timer_v0(perf_empty_pvlist),"" );

#ifdef TEST_BOOST_POOL
  csl_common_print_results(
    "perf_empty_boost_pool       ",
    csl_common_test_timer_v0(perf_empty_boost_pool),"" );
#endif

  csl_common_print_results( 
    "perf_add64_pointer_arrray   ",
    csl_common_test_timer_v0(perf_add64_pointer_arrray),"" );

  csl_common_print_results( 
    "perf_add64_pointer_vector   ",
    csl_common_test_timer_v0(perf_add64_pointer_vector),"" );

  csl_common_print_results( 
    "perf_add64_pointer_vector_2 ",
    csl_common_test_timer_v0(perf_add64_pointer_vector_2),"" );

  csl_common_print_results( 
    "perf_add64_pvlist           ",
    csl_common_test_timer_v0(perf_add64_pvlist),"" );

#ifdef TEST_BOOST_POOL
  csl_common_print_results(
    "perf_add64_boost_pool       ",
    csl_common_test_timer_v0(perf_add64_boost_pool),"" );
#endif

  return 0;
}

/* EOF */
