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
   @file t__hash.cc
   @brief Tests to verify hash table
 */

#if 0
#ifndef DEBUG
#define DEBUG
#define DEBUG_ENABLE_INDENT
#endif /* DEBUG */
#endif

#define COMPARE_STD
#ifdef COMPARE_STD
#include <ext/hash_map>

namespace __gnu_cxx
{
  template<> struct hash< uint64_t >
  {
    size_t operator()( const uint64_t & x ) const
    {
      uint64_t z = x;
      return static_cast<size_t>(z);
    }
  };
};
#endif

#include "hash.hh"
#include "tbuf.hh"
#include "pbuf.hh"

#include "test_timer.h"
#include "logger.hh"
#include "common.h"
#include <assert.h>
#include <vector>

using namespace csl::common;

/** @brief contains tests related to hash */
namespace test_hash {

  // hash_baseline               2717.156 ms, 67108862 calls,   0.000040 ms/call,   24698199.882524 calls/sec
  // tbuf_baseline               2086.350 ms, 134217726 calls,   0.000016 ms/call,   64331356.675534 calls/sec
  // pbuf_baseline               2491.725 ms, 16777214 calls,   0.000149 ms/call,   6733172.400646 calls/sec

  static inline const wchar_t * get_namespace()   { return L"test_hash"; }
  static inline const wchar_t * get_class_name()  { return L"test_hash::noclass"; }
  static inline const wchar_t * get_class_short() { return L"noclass"; }

  void hash_baseline() { hash<uint64_t,uint64_t> o; }
  void tbuf_baseline() { tbuf<1024> o; }
  void pbuf_baseline() { pbuf o; }

  void funct0()
  {
    hash<uint64_t,uint64_t> h;
    h.set( 0ULL,0ULL );
  }

  void funct1(int n)
  {
    hash<uint64_t,uint64_t> h;

    for( uint64_t i=0ULL;i<static_cast<uint64_t>(n);++i )
    {
      CSL_DEBUGF(L"set(%lld,%lld)\n",i+0,i);
      h.set( i+0,i );

#ifdef DEBUG
      h.debug();
#endif /*DEBUG*/

      CSL_DEBUGF(L"set(%lld,%lld)\n",i+1,i);
      h.set( i+1,i );

#ifdef DEBUG
      h.debug();
#endif /*DEBUG*/

      CSL_DEBUGF(L"set(%lld,%lld)\n",i+2,i);
      h.set( i+2,i );

#ifdef DEBUG
      h.debug();
#endif /*DEBUG*/
    }
  }

  void stdhash(int n)
  {
#ifdef COMPARE_STD
    typedef __gnu_cxx::hash_map<uint64_t, uint64_t, __gnu_cxx::hash<uint64_t> > ghash;
    ghash h;

    for( uint64_t i=0ULL;i<static_cast<uint64_t>(n);++i )
    {
      h.insert( ghash::value_type(i+0,i) );
      h.insert( ghash::value_type(i+1,i) );
      h.insert( ghash::value_type(i+2,i) );
    }
#endif
  }

  void ghash_baseline()
  {
#ifdef COMPARE_STD
    typedef __gnu_cxx::hash_map<uint64_t, uint64_t, __gnu_cxx::hash<uint64_t> > ghash;
    ghash h;
#endif
  }

} // end of test_hash

using namespace test_hash;

int main()
{
#ifdef DEBUG
  funct1(5);
  //funct0();
#else

#if !0
  csl_common_print_results( "hash_baseline            ", csl_common_test_timer_v0(hash_baseline),"" );
  csl_common_print_results( "tbuf_baseline            ", csl_common_test_timer_v0(tbuf_baseline),"" );
  csl_common_print_results( "pbuf_baseline            ", csl_common_test_timer_v0(pbuf_baseline),"" );

#ifdef COMPARE_STD
  csl_common_print_results( "ghash_baseline           ", csl_common_test_timer_v0(ghash_baseline),"" );
#endif /*COMPARE_STD*/

  csl_common_print_results( "funct1(5)                ", csl_common_test_timer_i1(funct1,5),"" );
  csl_common_print_results( "funct1(31)               ", csl_common_test_timer_i1(funct1,31),"" );
  csl_common_print_results( "funct1(50)               ", csl_common_test_timer_i1(funct1,50),"" );
  csl_common_print_results( "funct1(100)              ", csl_common_test_timer_i1(funct1,100),"" );
  csl_common_print_results( "funct1(3000)             ", csl_common_test_timer_i1(funct1,3000),"" );

#ifdef COMPARE_STD
  csl_common_print_results( "stdhash(5)               ", csl_common_test_timer_i1(stdhash,5),"" );
  csl_common_print_results( "stdhash(31)              ", csl_common_test_timer_i1(stdhash,31),"" );
  csl_common_print_results( "stdhash(100)             ", csl_common_test_timer_i1(stdhash,100),"" );
  csl_common_print_results( "stdhash(3000)            ", csl_common_test_timer_i1(stdhash,3000),"" );
#endif /*COMPARE_STD*/

#else
  csl_common_print_results( "funct1(300000)           ", csl_common_test_timer_i1(funct1,300000),"" );
#endif

#endif /*DEBUG*/

  return 0;
}

/* EOF */
