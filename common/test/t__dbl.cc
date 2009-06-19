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
   @file t__dbl.cc
   @brief Tests to verify dbl
 */

#include "dbl.hh"
#include "int64.hh"
#include "str.hh"
#include "ustr.hh"
#include "xdrbuf.hh"
#include "test_timer.h"
#include "common.h"
#include <assert.h>
#include <string>

using namespace csl::common;

/** @brief contains tests related to dbl */
namespace test_dbl {

  /** @test baseline for performance comparison */
  void baseline()
  {
    dbl v;
  }

  void conv_int()
  {
    long long iorig=12345678, idest=0;
    dbl v;
    assert( v.from_integer( iorig ) == true );
    assert( v.to_integer( idest ) == true );
    assert( iorig == idest );

    int64 i,i2;
    iorig = 987654;
    assert( i.from_integer( iorig ) == true );
    assert( v.from_integer( i ) == true );
    assert( v.to_integer( i2 ) == true );
    assert( i2.to_integer( idest ) == true );
    assert( iorig == idest );
  }

  void conv_double()
  {
    double dorig=12345.6789, ddest=0;
    dbl v;
    assert( v.from_double( dorig ) == true );
    assert( v.to_double( ddest ) == true );
    assert( dorig == ddest );

    dbl d,d2;
    dorig = 987654.321;
    assert( d.from_double( dorig ) == true );
    assert( v.from_double( d ) == true );
    assert( v.to_double( d2 ) == true );
    assert( d2.to_double( ddest ) == true );
    assert( dorig == ddest );
  }

  void to_integer_o()
  {
    dbl v;
    int64 o;
    assert( v.from_string(L"998877.22221111") == true );
    assert( v.to_integer(o) == true ); /**/
    assert( static_cast<long long>(v.value()) == o.value() );
    assert( o.value() == 998877LL );
  }

  void to_integer_l()
  {
    dbl v;
    long long o;
    assert( v.from_string("-998877.22221111") == true );
    assert( v.to_integer(o) == true ); /**/
    assert( static_cast<long long>(v.value()) == o );
    assert( o == -998877LL );
  }

  void to_double_o()
  {
    dbl v;
    dbl o;
    assert( v.from_integer( 199988812LL ) == true );
    assert( v.to_double(o) == true );
    assert( o.value() == 199988812.0 );
  }

  void to_double_d()
  {
    dbl v;
    double o;
    assert( v.from_integer( 199988812LL ) == true );
    assert( v.to_double(o) == true );
    assert( o == 199988812.0 );
  }

  void to_string_so()
  {
    dbl v;
    str o;
    assert( v.from_double( 3.14 ) == true );
    assert( v.to_string( o ) == true );
    assert( o == L"3.140000000000" );
  }

  void to_string_su()
  {
    dbl v;
    ustr o;
    assert( v.from_double( 3.14 ) == true );
    assert( v.to_string( o ) == true );
    assert( o == "3.140000000000" );
  }

  void to_string_ss()
  {
    dbl v;
    std::string o;
    assert( v.from_double( 3.14 ) == true );
    assert( v.to_string( o ) == true );
    assert( o == "3.140000000000" );
  }

  void to_binary_o()
  {
    dbl v;
    binry o;
    assert( v.from_double( 398881.33314 ) == true );
    assert( v.to_binary( o ) == true );
    dbl o2;
    assert( o.to_double(o2) );
    assert( o2.value() == 398881.33314 );
    dbl o3;
    assert( o3.from_binary(o) == true );
    assert( o2.value() == o3.value() );
  }

  void to_binary_u()
  {
    dbl v;
    int64 i;
    assert( i.from_integer(99991111LL) == true );
    unsigned char o[sizeof(double)];
    size_t sz = 0;
    assert( v.from_integer( i ) == true );
    assert( v.to_binary( o,sz ) == true );
    assert( sz == sizeof(double) );
    dbl v2;
    assert( v2.from_binary( o,sz ) == true );
    assert( v2.value() == 99991111.0 );
  }

  void to_binary_v()
  {
    dbl v;
    dbl i;
    assert( i.from_integer(-99991111LL) == true );
    unsigned char o[sizeof(double)];
    void * vp = o;
    size_t sz = 0;
    assert( v.from_double(i) == true );
    assert( v.to_binary( vp,sz ) == true );
    assert( sz == sizeof(double) );
    dbl v2;
    assert( v2.from_binary( vp,sz ) == true );
    assert( v2.value() == -99991111.0 );
  }

  void to_xdr()
  {
    dbl v;
    // TODO
  }

  void to_var()
  {
    dbl v;
    // TODO
  }

  void from_string_so()
  {
    dbl v;
    str o(L"997653.123987");
    assert( v.from_string(o) == true );
    assert( v.value() == 997653.123987 );
  }

  void from_string_uo()
  {
    dbl v;
    ustr o(L"-997653.123987");
    assert( v.from_string(o) == true );
    assert( v.value() == -997653.123987 );
  }

  void from_string_ss()
  {
    dbl v;
    std::string o("-997653.123987");
    assert( v.from_string(o) == true );
    assert( v.value() == -997653.123987 );
  }

  void from_xdr()
  {
    dbl v;
    // TODO
  }

  void from_var()
  {
    dbl v;
    // TODO
  }


} // end of test_dbl

using namespace test_dbl;

int main()
{
  csl_common_print_results( "baseline         ", csl_common_test_timer_v0(baseline),"" );
  csl_common_print_results( "conv_int         ", csl_common_test_timer_v0(conv_int),"" );
  csl_common_print_results( "conv_double      ", csl_common_test_timer_v0(conv_double),"" );

  /* conversions */
  csl_common_print_results( "to_integer_o     ", csl_common_test_timer_v0(to_integer_o),"" );
  csl_common_print_results( "to_integer_l     ", csl_common_test_timer_v0(to_integer_l),"" );
  csl_common_print_results( "to_double_o      ", csl_common_test_timer_v0(to_double_o),"" );
  csl_common_print_results( "to_double_d      ", csl_common_test_timer_v0(to_double_d),"" );
  csl_common_print_results( "to_string_so     ", csl_common_test_timer_v0(to_string_so),"" );
  csl_common_print_results( "to_string_su     ", csl_common_test_timer_v0(to_string_su),"" );
  csl_common_print_results( "to_string_ss     ", csl_common_test_timer_v0(to_string_ss),"" );
  csl_common_print_results( "to_binary_o      ", csl_common_test_timer_v0(to_binary_o),"" );
  csl_common_print_results( "to_binary_u      ", csl_common_test_timer_v0(to_binary_u),"" );
  csl_common_print_results( "to_binary_v      ", csl_common_test_timer_v0(to_binary_v),"" );
  csl_common_print_results( "to_xdr           ", csl_common_test_timer_v0(to_xdr),"" );
  csl_common_print_results( "to_var           ", csl_common_test_timer_v0(to_var),"" );
  csl_common_print_results( "from_string_so   ", csl_common_test_timer_v0(from_string_so),"" );
  csl_common_print_results( "from_string_uo   ", csl_common_test_timer_v0(from_string_uo),"" );
  csl_common_print_results( "from_string_ss   ", csl_common_test_timer_v0(from_string_ss),"" );
  csl_common_print_results( "from_xdr         ", csl_common_test_timer_v0(from_xdr),"" );
  csl_common_print_results( "from_var         ", csl_common_test_timer_v0(from_var),"" );

  return 0;
}

/* EOF */
