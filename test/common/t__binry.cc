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
   @file t__binry.cc
   @brief Tests to verify binry
 */

#include "codesloop/common/int64.hh"
#include "codesloop/common/dbl.hh"
#include "codesloop/common/binry.hh"
#include "codesloop/common/str.hh"
#include "codesloop/common/ustr.hh"
#include "codesloop/common/xdrbuf.hh"
#include "codesloop/common/test_timer.h"
#include "codesloop/common/common.h"
#include <assert.h>
#include <string>

using namespace csl::common;

/** @brief contains tests related to binry */
namespace test_binry {

  /** @test baseline for performance comparison */
  void baseline()
  {
    binry v;
  }

  void to_integer_o()
  {
    binry v;
    int64 o;
    assert( v.from_integer(-298890LL) == true );
    assert( v.to_integer( o ) == true );
    assert( o.value() == -298890LL );
  }

  void to_integer_l()
  {
    binry v;
    long long o;
    assert( v.from_integer(-298890111LL) == true );
    assert( v.to_integer( o ) == true );
    assert( o == -298890111LL );
  }

  void to_double_o()
  {
    binry v;
    dbl o;
    assert( v.from_double(-298890.876542) == true );
    assert( v.to_double( o ) == true );
    assert( o.value() == -298890.876542 );
  }

  void to_double_d()
  {
    binry v;
    double o;
    assert( v.from_double(-244890.876542) == true );
    assert( v.to_double( o ) == true );
    assert( o == -244890.876542 );
  }

  void to_string_so()
  {
    binry v;
    str o;
    assert( v.from_string(L"árvíztűrő tükörfúrógép ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP") == true );
    assert( v.to_string(o) == true );
    assert( o == L"árvíztűrő tükörfúrógép ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP" );
  }

  void to_string_su()
  {
    binry v;
    ustr i("árvíztűrő tükörfúrógép ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP");
    ustr o;
    assert( v.from_string(i) == true );
    assert( v.to_string(o) == true );
    assert( o == "árvíztűrő tükörfúrógép ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP" );
  }

  void to_string_ss()
  {
    binry v;
    std::string o;
    assert( v.from_string("Hello world") == true );
    assert( v.to_string(o) == true );
    assert( o == "Hello world" );
  }

  void to_binary_o()
  {
    binry v;
    binry o;
    str i("árvíztűrő tükörfúrógép ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP");
    assert( v.from_string(i) == true );
    assert( v.to_binary(o) == true );
    assert( v.value() == o.value() );
  }

  void to_binary_u()
  {
    binry v;
    unsigned char tmp[300];
    assert( v.from_string("árvíztűrő tükörfúrógép ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP") == true );
    uint64_t sz = 0;
    assert( v.to_binary(tmp,sz) == true );
    assert( sz > 10 );
    binry::buf_t bf;
    assert( bf.set( tmp,sz ) == true );
    assert( v.value() == bf );
  }

  void to_binary_v()
  {
    binry v;
    unsigned char tmp[300];
    void * vp = tmp;
    assert( v.from_string("árvíztűrő tükörfúrógép ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP") == true );
    uint64_t sz = 0;
    assert( v.to_binary(vp,sz) == true );
    assert( sz > 10 );
    binry::buf_t bf;
    assert( bf.set( tmp,sz ) == true );
    assert( v.value() == bf );
  }

  void to_xdr()
  {
    binry v,v2;
    pbuf p;
    xdrbuf xb(p);
    assert( v.from_integer(12345678LL) == true );
    assert( v.to_xdr(xb) == true ); /**/
    xb.rewind();
    assert( v2.from_xdr(xb) == true );
    assert( v.value() == v2.value() );
    int64 i;
    assert( v2.to_integer(i) == true );
    assert( i.value() == 12345678LL );
  }

  void to_var()
  {
    binry v;
    // TODO
  }

  void from_integer_o()
  {
    binry v;
    int64 o,o2;
    assert( o.from_double(3.14) == true );
    assert( v.from_integer(o) == true );
    assert( v.value().size() == sizeof(long long) );
    assert( v.to_integer(o2) == true );
    assert( o2.value() == 3 );
  }

  void from_double_o()
  {
    binry v;
    dbl o,o2;
    assert( o.from_double(3.14) == true );
    assert( v.from_double(o) == true );
    assert( v.value().size() == sizeof(double) );
    assert( v.to_double(o2) == true );
    assert( o2.value() == 3.14 );
  }

  void from_string_ss()
  {
    binry v;
  }

  void from_binary_o()
  {
    binry v;
    binry o;
    str i("árvíztűrő tükörfúrógép ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP");
    assert( v.from_string(i) == true );
    assert( o.from_binary(v) == true );
    assert( v.value() == o.value() );
  }

  void from_binary_u()
  {
    binry v;
    unsigned char o[300];
    uint64_t sz;
    ustr i("árvíztűrő tükörfúrógép ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP");
    assert( v.from_string(i) == true );
    assert( v.to_binary(o,sz) == true );
    assert( sz > 10 );
    binry::buf_t bf;
    assert( bf.set( o,sz ) == true );
    assert( v.value() == bf );
  }

  void from_binary_v()
  {
    binry v;
    unsigned char o[300];
    void * vp = o;
    uint64_t sz;
    str i(L"árvíztűrő tükörfúrógép ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP");
    assert( v.from_string(i) == true );
    assert( v.to_binary(vp,sz) == true );
    assert( sz > 10 );
    binry::buf_t bf;
    assert( bf.set( o,sz ) == true );
    assert( v.value() == bf );
  }

  void from_var()
  {
    binry v;
    // TODO
  }

} // end of test_binry

using namespace test_binry;

int main()
{
  if (!setlocale(LC_CTYPE, ""))
  {
    fprintf(stderr, "Can't set the specified locale! Check LANG, LC_CTYPE, LC_ALL.\n");
    exit(-1);
  }

  csl_common_print_results( "baseline         ", csl_common_test_timer_v0(baseline),"" );

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
  csl_common_print_results( "from_string_ss   ", csl_common_test_timer_v0(from_string_ss),"" );
  csl_common_print_results( "from_binary_o    ", csl_common_test_timer_v0(from_binary_o),"" );
  csl_common_print_results( "from_binary_u    ", csl_common_test_timer_v0(from_binary_u),"" );
  csl_common_print_results( "from_binary_v    ", csl_common_test_timer_v0(from_binary_v),"" );
  csl_common_print_results( "from_var         ", csl_common_test_timer_v0(from_var),"" );

  return 0;
}

/* EOF */
