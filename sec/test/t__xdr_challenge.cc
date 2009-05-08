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
   @file t__xdr_challenge.cc
   @brief Tests to verify xdr_challenge
 */

#include "xdr_challenge.hh"
#include "ecdh_key.hh"
#include "bignum.hh"
#include "zfile.hh"
#include "xdrbuf.hh"
#include "pbuf.hh"
#include "test_timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

using namespace csl::sec;
using namespace csl::common;

/** @brief contains tests related to xdr_challenge */
namespace test_xdr_challenge {

  /** @test performance baseline */
  void baseline()
  {
    xdr_challenge x;
  }

  /** @test basic XDR conversion */
  void xdr()
  {
    xdr_challenge a,b;
    bignum bn;
    for( unsigned int i=0;i<xdr_challenge::max_keys_offered;++i )
    {
      ecdh_key k;
      k.algname("prime192v3");
      bignum private_key;
      assert( k.gen_keypair(private_key) == true );
      a.set(k,i);
    }
    pbuf pb;
    xdrbuf xb(pb);
    assert( a.to_xdr(xb) == true );
    assert( pb.size() > 0 );

    zfile zf;
    assert( zf.put_data(pb) == true );
    assert( zf.write_file("xdr_challenge.xdrbin") == true );

    xb.rewind();

    assert( b.from_xdr(xb) == true );
    assert( a == b );
    assert( a[0].has_data() == true );
    assert( a[1].has_data() == true );
    assert( a[2].has_data() == true );
  }

  struct rndata
  {
    size_t len_;
    const char * filename_;
  };

  static rndata random_files[] = {
    { 15,     "random.15" },
    { 27,     "random.27" },
    { 99,     "random.99" },
    { 119,    "random.119" },
    { 279,    "random.279" },
    { 589,    "random.589" },
    { 1123,   "random.1123" },
    { 1934,   "random.1934" },
    { 28901,  "random.28901" },
    { 31965,  "random.31965" },
    { 112678, "random.112678" },
    { 0, 0 }
  };

  /** @test to see XDR conversion survives random garbage */
  void random_xdr()
  {
    rndata * p = random_files;
    while( p->len_ )
    {
      zfile zf;
      assert( zf.read_file(p->filename_) == true );
      assert( zf.get_size() == p->len_ );
      pbuf pb;
      assert( zf.get_data(pb) == true );
      xdrbuf xb(pb);
      xb.rewind();
      xdr_challenge val;
      assert( val.from_xdr(xb) == false );
      ++p;
    };
  }

} // end of test_xdr_challenge

using namespace test_xdr_challenge;

int main()
{
  xdr();
  csl_common_print_results( "baseline     ", csl_common_test_timer_v0(baseline),"" );
  csl_common_print_results( "random_xdr   ", csl_common_test_timer_v0(random_xdr),"" );
  return 0;
}

/* EOF */
