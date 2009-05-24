/*
Copyright (c) 2008,2009, David Beck

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
   @file t__reg.cc
   @brief Tests to check and measure various slt3::reg features
 */

#include "test_timer.h"
#include "reg.hh"
#include "exc.hh"
#include "common.h"
#include "mpool.hh"
#include <assert.h>
#include <stdlib.h>

using namespace csl::slt3;

/** @brief contains tests related to slt3::reg */
namespace test_reg {

  /** @test baseline for performance comparison */
  void baseline()
  {
    reg & r(reg::instance("test.db"));
  }

  /** @test registers and gets an item from database registry */
  void usage1()
  {
    reg & r(reg::instance("test.db"));
    reg::item i = { 0,"Hello","hello.db" };
    r.set( i );
    conn h;
    assert( r.get("Hello",h) == true );
    assert( h.close() == true );
  }

  /** @test simple usage scenario */
  void usage2()
  {
    reg & r(reg::instance("test.db"));
    reg::pool_t p;
    reg::item i;
    assert( r.get( "Hello",i,p ) == true );
    assert( std::string("Hello") == i.name_ );
    assert( std::string("hello.db") == i.path_ );
  }

  /** @test how double inserting the same values behave */
  void usage3()
  {
    reg & r(reg::instance("test.db"));
    reg::item i = { 0,"Hello","hello.db" };
    r.set( i );
    assert( r.set( i ) == false );
  }

  /** @test how lookup of nonexistent values behave */
  void usage4()
  {
    reg & r(reg::instance("test.db"));
    reg::pool_t p;
    reg::item i;
    assert( r.get( "Nonexsitant garbage",i,p ) == false );
    conn c;
    assert( r.get( "Nonexsitant garbage",c ) == false );
  }

} // end of test_reg

using namespace test_reg;

int main()
{
  UNLINK("test.db");
  UNLINK("hello.db");

  reg & r(reg::instance("test.db"));
  conn c;
  assert( r.get("Hello",c) == false );
  reg::item i = { 0,"Hello","hello.db" };
  assert( r.set( i ) == true );
  assert( r.get("Hello",c) == true );

  csl_common_print_results( "baseline           ", csl_common_test_timer_v0(baseline),"" );
  csl_common_print_results( "usage1             ", csl_common_test_timer_v0(usage1),"" );
  csl_common_print_results( "usage2             ", csl_common_test_timer_v0(usage2),"" );
  csl_common_print_results( "usage3             ", csl_common_test_timer_v0(usage3),"" );
  csl_common_print_results( "usage4             ", csl_common_test_timer_v0(usage4),"" );
  return 0;
}

/* EOF */
