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
   @file t__ecdh_key.cc
   @brief Tests to verify ecdh_key
 */

#include "ecdh_key.hh"
#include "bignum.hh"
#include "test_timer.h"
#include "zfile.hh"
#include "xdrbuf.hh"
#include "pbuf.hh"
#include "common.h"
#include "str.hh"
#include "ustr.hh"
#include <assert.h>

using namespace csl::sec;
using namespace csl::common;

/** @brief contains tests related to ecdh_key */
namespace test_ecdh_key {

  /** @test performance baseline */
  void baseline()
  {
    ecdh_key k;
  }

  /** @test set algorithm name */
  void bl_prime192v3_1()
  {
    ecdh_key k;
    k.algname("prime192v3");
  }

  /** @test generate prime192v3 keypair */
  void bl_prime192v3_2()
  {
    ecdh_key k;
    k.algname("prime192v3");
    bignum private_key;
    assert( k.gen_keypair(private_key) == true );
  }

  /** @test print internal data */
  void print_prime192v3()
  {
    ecdh_key k1,k2;
    k1.algname("prime192v3");
    k2.algname("prime192v3");

    bignum private_key1;
    bignum private_key2;

    assert( k1.gen_keypair(private_key1) == true );
    assert( k2.gen_keypair(private_key2) == true );

    ustr shared1,shared2;

    assert( k1.gen_sha1hex_shared_key(private_key2,shared1) == true );
    assert( k2.gen_sha1hex_shared_key(private_key1,shared2) == true );

    assert( shared1 == shared2 );
    assert( shared1.size() > 10 );

    k1.print();
    private_key1.print();

    k2.print();
    private_key2.print();

    PRINTF(L"SHARED KEY1: %s\n",shared1.c_str());
    PRINTF(L"SHARED KEY2: %s\n",shared2.c_str());

    pbuf pb1,pb2;

    assert( k1.gen_shared_key(private_key2,pb1) == true );
    assert( k2.gen_shared_key(private_key1,pb2) == true );

    PRINTF(L"Key sizes: [%d] [%d]\n",pb1.size(),pb2.size());
  }

  /** @test generate prime192v3 keypairs and shared key */
  void prime192v3_keypair()
  {
    ecdh_key k1,k2;
    k1.algname("prime192v3");
    k2.algname("prime192v3");

    bignum private_key1;
    bignum private_key2;

    assert( k1.gen_keypair(private_key1) == true );
    assert( k2.gen_keypair(private_key2) == true );

    ustr shared1,shared2;

    assert( k1.gen_sha1hex_shared_key(private_key2,shared1) == true );
    assert( k2.gen_sha1hex_shared_key(private_key1,shared2) == true );

    assert( shared1 == shared2 );
    assert( shared1.size() > 10 );

    k1.print();
    private_key1.print();

    k2.print();
    private_key2.print();

    PRINTF(L"SHARED KEY: %s\n",shared1.c_str());
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
      ecdh_key val;
      assert( val.from_xdr(xb) == false );
      ++p;
    };
  }

} // end of test_ecdh_key

using namespace test_ecdh_key;

int main()
{
  if (!setlocale(LC_CTYPE, "")) {
    fprintf(stderr, "Can't set the specified locale! "
                    "Check LANG, LC_CTYPE, LC_ALL.\n");
    exit(-1);
  }

  print_prime192v3();
  prime192v3_keypair();

  csl_common_print_results( "baseline         ", csl_common_test_timer_v0(baseline),"" );
  csl_common_print_results( "bl_prime192v3_1  ", csl_common_test_timer_v0(bl_prime192v3_1),"" );
  csl_common_print_results( "bl_prime192v3_2  ", csl_common_test_timer_v0(bl_prime192v3_2),"" );
  csl_common_print_results( "random_xdr       ", csl_common_test_timer_v0(random_xdr),"" );

  return 0;
}

/* EOF */
