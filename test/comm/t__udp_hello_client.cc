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
   @file t__udp_client2.cc
   @brief Tests to verify udp::cli routines
 */

#include "codesloop/comm/udp_hello.hh"
#include "codesloop/common/test_timer.h"
#include "codesloop/common/common.h"
#include <assert.h>

using namespace csl::common;
using namespace csl::sec;
using namespace csl::comm;

/** @brief contains tests related to udp clients */
namespace test_udp_client {

  void basic()
  {
    udp::hello_cli c;
    SAI h;

    c.use_exc(false);

    memset( &h,0,sizeof(h) );
    h.sin_family       = AF_INET;
    h.sin_addr.s_addr  = htonl(INADDR_LOOPBACK);
    h.sin_port         = htons(47781);

    c.addr(h);

    ecdh_key pubkey;
    bignum   privkey;

    pubkey.algname("prime192v3");

    /* generate keypair */
    assert( pubkey.gen_keypair(privkey) == true );

    c.private_key(privkey);
    c.public_key(pubkey);

    assert( c.hello( 3000 ) == true );
  }

  static udp::hello_cli * global_client_ = 0;

  void hello()
  {
    assert( global_client_->hello( 3000 ) == true );
  }

#if 0
  void start()
  {
    assert( global_client_->start( 3000 ) == true );
  }
#endif

} // end of test_udp_client

using namespace test_udp_client;

int main()
{
  udp::hello_cli c_global;
  SAI h;

  c_global.use_exc(false);

  memset( &h,0,sizeof(h) );
  h.sin_family       = AF_INET;
  h.sin_addr.s_addr  = htonl(INADDR_LOOPBACK);

  h.sin_port         = htons(47781);
  c_global.addr(h);

  ecdh_key pubkey;
  bignum   privkey;

  pubkey.algname("prime192v3");

  /* generate keypair */
  assert( pubkey.gen_keypair(privkey) == true );

  c_global.private_key(privkey);
  c_global.public_key(pubkey);

  global_client_ = &c_global;

  csl_common_print_results( "basic      ", csl_common_test_timer_v0(basic),"" );
  csl_common_print_results( "hello      ", csl_common_test_timer_v0(hello),"" );
  //csl_common_print_results( "start      ", csl_common_test_timer_v0(start),"" );
  return 0;
}

/* EOF */
