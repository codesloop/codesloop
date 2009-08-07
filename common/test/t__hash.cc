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

#ifndef DEBUG
#define DEBUG
#endif /* DEBUG */

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

} // end of test_hash

using namespace test_hash;

int main()
{
#ifdef DEBUG
  funct0();
#else
  csl_common_print_results( "funct0                   ", csl_common_test_timer_v0(funct0),"" );
#endif /*DEBUG*/

  csl_common_print_results( "hash_baseline            ", csl_common_test_timer_v0(hash_baseline),"" );
  csl_common_print_results( "tbuf_baseline            ", csl_common_test_timer_v0(tbuf_baseline),"" );
  csl_common_print_results( "pbuf_baseline            ", csl_common_test_timer_v0(pbuf_baseline),"" );

  return 0;
}

/* EOF */
