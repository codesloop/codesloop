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
   @file t__mpool.cc
   @brief Tests to verify mpool
*/

#ifndef DEBUG
#define DEBUG
#endif /* DEBUG */

#include "codesloop/common/mpool.hh"
#include "codesloop/common/test_timer.h"
#include "codesloop/common/common.h"
#include <vector>
#include <assert.h>

using namespace csl::common;

/** @brief contains tests related to mpool */
namespace test_mpool {

  /** @test simple */
  void test_ref()
  {
    mpool <> mp;
  }

  /** @test simple */
  void test_simple()
  {
    mpool<> p;
    char * p1 = reinterpret_cast<char *>(p.allocate(10));
    char * p2 = reinterpret_cast<char *>(p.allocate(20));
    
    assert( p.is_from(p1) == true );
    assert( p.find(p1)    == true );
    assert( p.free(p1)    == true );
    assert( p.free(p1)    == false );
    assert( p.find(p1)    == false );
    assert( p.is_from(p1) == false );

    assert( p.is_from(p2) == true );
    assert( p.find(p2)    == true );
    assert( p.free(p2)    == true );
    assert( p.free(p2)    == false );
    assert( p.find(p2)    == false );
    assert( p.is_from(p2) == false );
    
    p.free_all();
  }  
} // end of namespace test_mpool

using namespace test_mpool;

int main()
{
  csl_common_print_results( 
    "test_ref                    ",
    csl_common_test_timer_v0(test_ref),"" );
  
  csl_common_print_results( 
    "test_simple                 ",
    csl_common_test_timer_v0(test_simple),"" );
  
  return 0;
}

/* EOF */
