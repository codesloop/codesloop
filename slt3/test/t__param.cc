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
   @file t__param.cc
   @brief Tests to check and measure various slt3::param features
 */

#include "test_timer.h"
#include "param.hh"
#include "conn.hh"
#include "tran.hh"
#include "synqry.hh"
#include "common.h"
#include "str.hh"
#include <assert.h>

using namespace csl;
using namespace csl::slt3;
using csl::common::str;

/** @brief contains tests related to slt3::param */
namespace test_param {

  /** @test baseline for performance comparison */
  void baseline()
  {
    conn c;
    c.use_exc(false);
    tran t(c);
    synqry q(t);
    param & p(q.get_param(1));
    p.set(1.01);

    assert( c.use_exc() == false );
    assert( t.use_exc() == false );
    assert( q.use_exc() == false );
    assert( p.use_exc() == false );
    assert( p.get_double() == 1.01 );
  }

  template <typename FROM,typename TO>
  void test_conv(FROM f,TO t,param & p)
  {
    p.set(f);
    TO tx;
    assert( p.get(tx) == true );
    assert( tx == t );
  }

  /** @test convert between different datatypes */
  void test_conv()
  {
    conn c;
    c.use_exc(false);
    tran t(c);
    synqry q(t);
    param & p(q.get_param(1));

    // int to anything
    long long intval = 123456789ll;
    param::blob_t b;
    b.set((unsigned char *)(&intval),sizeof(intval));

    test_conv( intval, (double)123456789.0, p );
    test_conv( intval, str(L"123456789"), p );
    test_conv( intval, b, p );

    // double to anything
    double dblval = 357987.12345;
    b.set((unsigned char *)(&dblval),sizeof(dblval));

    test_conv( dblval, (long long)357987ll, p );
    test_conv( dblval, str(L"357987.1234500000"), p );
    test_conv( dblval, b, p );

    // string to anything
    str strval(L"Hello world");
    b.set( (unsigned char *)strval.data(), strval.nbytes() );

    test_conv( strval, (long long)0ll, p );
    test_conv( strval, (double)0.0, p );
    test_conv( strval, b, p );

    strval = L"12345678";
    b.set( (unsigned char *)strval.data(), strval.nbytes() );

    test_conv( strval, (long long)12345678ll, p );
    test_conv( strval, (double)12345678.0, p );
    test_conv( strval, b, p );

    strval = L"12345678.12345678";
    b.set( (unsigned char *)strval.data(), strval.nbytes() );

    test_conv( strval, (long long)12345678ll, p );
    test_conv( strval, (double)12345678.12345678, p );
    test_conv( strval, b, p );

    // blob to anything (reasonable)
    b.set( (unsigned char *)(&intval), sizeof(intval) );

    test_conv( b, intval, p );

    b.set( (unsigned char *)(&dblval), sizeof(dblval) );

    test_conv( b, dblval, p );

    b.set( (unsigned char *)strval.data(), strval.nbytes() );

    test_conv( b, strval, p );
  }

} // end of test_param

using namespace test_param;

int main()
{
  csl_common_print_results( "baseline    ", csl_common_test_timer_v0(baseline),"" );
  csl_common_print_results( "test_conv   ", csl_common_test_timer_v0(test_conv),"" );
  return 0;
}

/* EOF */
