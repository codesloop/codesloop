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
   @file t__int64.cc
   @brief Tests to verify int64
 */

#include "codesloop/common/int64.hh"
#include "codesloop/common/dbl.hh"
#include "codesloop/common/str.hh"
#include "codesloop/common/ustr.hh"
#include "codesloop/common/xdrbuf.hh"
#include "codesloop/common/test_timer.h"
#include "codesloop/common/common.h"
#include <assert.h>
#include <string>

using namespace csl::common;

/** @brief contains tests related to int64 */
namespace test_int64 {

  /** @test baseline for performance comparison */
  void baseline()
  {
    int64 v;
  }

  void to_integer_o()
  {
    int64 v;
    int64 o;
    assert( v.from_integer(12345678LL) == true );
    assert( v.to_integer(o) == true ); /**/
    assert( v.value() == o.value() );
    assert( o.value() == 12345678LL );
  }

  void to_integer_l()
  {
    int64 v;
    long long o = 0;
    assert( v.from_integer(12345678LL) == true );
    assert( v.to_integer(o) == true ); /**/
    assert( v.value() == 12345678LL );
    assert( v.value() == o );
  }

  void to_double_o()
  {
    int64 v;
    dbl o;
    assert( v.from_integer(12345678LL) == true );
    assert( v.to_double(o) == true ); /**/
    assert( o.value() == static_cast<double>(12345678LL) );
  }

  void to_double_d()
  {
    int64 v;
    double o = 0.0;
    assert( v.from_double(12345678.0) == true );
    assert( v.to_double(o) == true ); /**/
    assert( o == static_cast<double>(v.value()) );
    assert( o == 12345678.0 );
  }

  void to_string_so()
  {
    int64 v;
    str o;
    assert( v.from_integer(98765432LL) == true );
    assert( v.to_string(o) == true ); /**/
    assert( o == L"98765432" );
  }

  void to_string_su()
  {
    int64 v;
    ustr o;
    assert( v.from_integer(98765432LL) == true );
    assert( v.to_string(o) == true ); /**/
    assert( o == "98765432" );
  }

  void to_string_ss()
  {
    int64 v;
    std::string o;
    assert( v.from_double(98765432.0) == true );
    assert( v.to_string(o) == true ); /**/
    assert( o == "98765432" );
  }

  void to_binary_o()
  {
    int64 v;
    binry o;
    long long l = 98765432LL;
    assert( v.from_integer(l) == true );
    assert( v.to_binary(o) == true ); /**/
    assert( ::memcmp( o.value().data(),&l,o.value().size() ) == 0 );
  }

  void to_binary_u()
  {
    int64 v;
    long long o = 98765432LL;
    unsigned char l[sizeof(long long)];
    size_t sz=0;

    assert( v.from_string(L"98765432") == true );
    assert( v.to_binary(l,sz) == true ); /**/
    assert( sz == sizeof(long long) );
    assert( ::memcmp( &o,l,sz ) == 0 );
  }

  void to_binary_v()
  {
    int64 v;
    long long o = 98765432LL;
    unsigned char l[sizeof(long long)];
    size_t sz=0;
    void * vp = l;

    assert( v.from_string("98765432") == true );
    assert( v.to_binary(vp,sz) == true ); /**/
    assert( sz == sizeof(long long) );
    assert( ::memcmp( &o,vp,sz ) == 0 );
  }

  void to_xdr()
  {
    int64 v,v2;
    pbuf p;
    xdrbuf xb(p);
    assert( v.from_integer(12345678LL) == true );
    assert( v.to_xdr(xb) == true ); /**/
    xb.rewind();
    assert( v2.from_xdr(xb) == true );
    assert( v.value() == v2.value() );
    assert( v2.value() == 12345678LL );
  }

  void to_var()
  {
    int64 v;
    int64 vi; dbl vd; binry vb; str vs; ustr vu;
    assert( v.from_string(L"989843431") == true );

    assert( v.to_var(vi) == true ); /**/
    assert( v.to_var(vd) == true ); /**/
    assert( v.to_var(vb) == true ); /**/
    assert( v.to_var(vs) == true ); /**/
    assert( v.to_var(vu) == true ); /**/

    assert( vi.value() == 989843431LL );
    assert( vd.value() == 989843431.0 );
    assert( vs == L"989843431" );
    assert( vu == "989843431" );

    long long ck;
    assert( vi.to_integer(ck) == true );
    assert( ::memcmp( vb.value().data(), &ck, vb.value().size() ) == 0 );
  }

  void from_integer_o()
  {
    int64 v;
    int64 o;
    assert( o.from_double(65987612.0) == true );
    assert( v.from_integer(o) == true ); /**/
    assert( v.value() == 65987612LL );
  }

  void from_double_o()
  {
    int64 v;
    dbl o;
    assert( o.from_string("65987612.000") == true );
    assert( v.from_double(o) == true ); /**/
    assert( v.value() == 65987612LL );
  }

  void from_string_so()
  {
    int64 v;
    str s;
    assert( s.from_string("378937809.000") == true );
    assert( v.from_string(s) == true ); /**/
    assert( v.value() == 378937809LL );
  }

  void from_string_uo()
  {
    int64 v;
    ustr s;
    assert( s.from_string(L"378937809.000") == true );
    assert( v.from_string(s) == true ); /**/
    assert( v.value() == 378937809LL );
  }

  void from_string_ss()
  {
    int64 v;
    std::string  s("378937809.000");
    assert( v.from_string(s) == true ); /**/
    assert( v.value() == 378937809LL );
  }

  void from_string_sc()
  {
    int64 v;
    const char * s = "378937809.000";
    assert( v.from_string(s) == true ); /**/
    assert( v.value() == 378937809LL );
  }

  void from_string_sw()
  {
    int64 v;
    const wchar_t * s = L"378937809.000";
    assert( v.from_string(s) == true ); /**/
    assert( v.value() == 378937809LL );
  }

  void from_binary_o()
  {
    int64 v;
    binry o;
    long long l = 44337788LL;
    assert( o.from_integer(l) == true );
    assert( v.from_binary(o) == true ); /**/
    assert( v.value() == l );
  }

  void from_binary_u()
  {
    int64 v;
    binry o;
    long long l = 44337788LL;
    assert( o.from_integer(l) == true );
    unsigned char u[sizeof(long long)];
    size_t sz = 0;
    assert( o.to_binary(u,sz) == true );
    assert( v.from_binary(u,sz) == true ); /**/
    assert( v.value() == l );
  }

  void from_binary_v()
  {
    int64 v;
    binry o;
    long long l = 44337788LL;
    assert( o.from_integer(l) == true );
    unsigned char u[sizeof(long long)];
    size_t sz = 0;
    void * vp = u;
    assert( o.to_binary(vp,sz) == true );
    assert( v.from_binary(vp,sz) == true ); /**/
    assert( v.value() == l );
  }

  void from_var()
  {
    int64 v;
    int64 vi; dbl vd; binry vb; str vs; ustr vu;

    assert( vi.from_string("22996644") == true );
    assert( vd.from_string(L"22996644") == true );
    assert( vb.from_integer(12345678LL) == true );
    assert( vs.from_integer(22996644LL) == true );
    assert( vu.from_integer(22996644LL) == true );

    assert( v.from_var( vi ) == true );
    assert( v.value() == 22996644LL );
    assert( v.from_var( vd ) == true );
    assert( v.value() == 22996644LL );
    assert( v.from_var( vb ) == true );
    assert( v.value() == 12345678LL );
    assert( v.from_var( vs ) == true );
    assert( v.value() == 22996644LL );
    assert( v.from_var( vu ) == true );
    assert( v.value() == 22996644LL );
  }


  void conv_int()
  {
    long long iorig=12345678, idest=0;
    int64 v;
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
    int64 v;
    assert( v.from_double( dorig ) == true );
    assert( v.to_double( ddest ) == true );
    dorig = 12345.0;
    assert( dorig == ddest );

    dbl d,d2;
    dorig = 987654.321;
    assert( d.from_double( dorig ) == true );
    assert( v.from_double( d ) == true );
    assert( v.to_double( d2 ) == true );
    assert( d2.to_double( ddest ) == true );
    dorig = 987654.0;
    assert( dorig == ddest );
  }

} // end of test_int64

using namespace test_int64;

int main()
{
  /* conversions */
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
  csl_common_print_results( "from_integer_o   ", csl_common_test_timer_v0(from_integer_o),"" );
  csl_common_print_results( "from_double_o    ", csl_common_test_timer_v0(from_double_o),"" );
  csl_common_print_results( "from_string_so   ", csl_common_test_timer_v0(from_string_so),"" );
  csl_common_print_results( "from_string_uo   ", csl_common_test_timer_v0(from_string_uo),"" );
  csl_common_print_results( "from_string_ss   ", csl_common_test_timer_v0(from_string_ss),"" );
  csl_common_print_results( "from_string_sc   ", csl_common_test_timer_v0(from_string_sc),"" );
  csl_common_print_results( "from_string_sw   ", csl_common_test_timer_v0(from_string_sw),"" );
  csl_common_print_results( "from_binary_o    ", csl_common_test_timer_v0(from_binary_o),"" );
  csl_common_print_results( "from_binary_u    ", csl_common_test_timer_v0(from_binary_u),"" );
  csl_common_print_results( "from_binary_v    ", csl_common_test_timer_v0(from_binary_v),"" );
  csl_common_print_results( "from_var         ", csl_common_test_timer_v0(from_var),"" );

  return 0;
}

/* EOF */
