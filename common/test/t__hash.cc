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
   @file t__hash.cc
   @brief Tests to verify hash table
 */

#include "hash.hh"
#include "test_timer.h"
#include "common.h"
#include <assert.h>

using namespace csl::common;

/** @brief contains tests related to hash */
namespace test_hash {

  /** @test baseline for performance comparison */
  void baseline() { }

  /** @test mod 7 baseline for performance comparison */
  void mod7()
  {
    unsigned long long i = 12345678ULL;
    assert( (i%7) < 7 );
  }

  /** @test mod 7,11 baseline for performance comparison */
  void mod7_11()
  {
    unsigned long long i = 12345678ULL;
    assert( (i%7) < 7 );
    assert( (i%11) < 11 );
  }

  void hash_fun_1()
  {
    unsigned long long i = 12345678ULL;
    default_hash_function(i,1);
  }

  void hash_fun_2()
  {
    unsigned long long i = 12345678ULL;
    default_hash_function(i,2);
  }

  void hash_fun_6()
  {
    unsigned long long i = 12345678ULL;
    default_hash_function(i,6);
  }

  void hash_fun_64()
  {
    unsigned long long i = 12345678ULL;
    default_hash_function(i,64);
  }

  void hash_fun_ck()
  {
    // 1 bit hash
    assert( default_hash_function(0,1) == 0 );
    assert( default_hash_function(1,1) == 1 );
    assert( default_hash_function(2,1) == 0 );
    assert( default_hash_function(3,1) == 1 );

    // 2 bits
    assert( default_hash_function(0,2) == 0 );
    assert( default_hash_function(1,2) == 1 );
    assert( default_hash_function(2,2) == 2 );
    assert( default_hash_function(3,2) == 3 );
    assert( default_hash_function(4,2) == 0 );
    assert( default_hash_function(5,2) == 1 );
    assert( default_hash_function(6,2) == 2 );
    assert( default_hash_function(7,2) == 0 );

    // 3 bits
    assert( default_hash_function(0,3)  == 0 );
    assert( default_hash_function(1,3)  == 1 );
    assert( default_hash_function(2,3)  == 2 );
    assert( default_hash_function(3,3)  == 3 );
    assert( default_hash_function(4,3)  == 4 );
    assert( default_hash_function(5,3)  == 5 );
    assert( default_hash_function(6,3)  == 6 );
    assert( default_hash_function(7,3)  == 0 );
    assert( default_hash_function(8,3)  == 1 );
    assert( default_hash_function(9,3)  == 2 );

    // 4 bits
    assert( default_hash_function(0,4)  == 0000 );
    assert( default_hash_function(1,4)  == 0011 );
    assert( default_hash_function(2,4)  == 0002 );
    assert( default_hash_function(3,4)  == 0013 );
    assert( default_hash_function(4,4)  == 0004 );
    assert( default_hash_function(5,4)  == 0015 );
    assert( default_hash_function(6,4)  == 0006 );
    assert( default_hash_function(7,4)  == 0010 );
    assert( default_hash_function(8,4)  == 0001 );
    assert( default_hash_function(9,4)  == 0012 );
    assert( default_hash_function(10,4) == 0003 );

    // 5 bits
    assert( default_hash_function(0,5)  == 0000 );
    assert( default_hash_function(1,5)  == 0011 );
    assert( default_hash_function(2,5)  == 0022 );
    assert( default_hash_function(3,5)  == 0033 );
    assert( default_hash_function(4,5)  == 0004 );
    assert( default_hash_function(5,5)  == 0015 );
    assert( default_hash_function(6,5)  == 0026 );
    assert( default_hash_function(7,5)  == 0030 );
    assert( default_hash_function(8,5)  == 0001 );
    assert( default_hash_function(9,5)  == 0012 );
    assert( default_hash_function(10,5) == 0023 );
    assert( default_hash_function(11,5) == 0004 );
    assert( default_hash_function(12,5) == 0015 );
    assert( default_hash_function(13,5) == 0026 );
  }


} // end of test_hash

using namespace test_hash;

int main()
{
  csl_common_print_results( "hash_fun_ck         ", csl_common_test_timer_v0(hash_fun_ck),"" );
  csl_common_print_results( "hash_fun_64         ", csl_common_test_timer_v0(hash_fun_64),"" );
  csl_common_print_results( "baseline            ", csl_common_test_timer_v0(baseline),"" );
  csl_common_print_results( "mod7                ", csl_common_test_timer_v0(mod7),"" );
  csl_common_print_results( "mod7_11             ", csl_common_test_timer_v0(mod7_11),"" );
  csl_common_print_results( "hash_fun_1          ", csl_common_test_timer_v0(hash_fun_1),"" );
  csl_common_print_results( "hash_fun_2          ", csl_common_test_timer_v0(hash_fun_2),"" );
  csl_common_print_results( "hash_fun_6          ", csl_common_test_timer_v0(hash_fun_6),"" );

  return 0;
}

/* EOF */
