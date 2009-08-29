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
   @file t__udp_auth_client.cc
   @brief Tests to verify udp::auth_client routines
 */

#include "udp_hello.hh"
#include "udp_auth.hh"
#include "test_timer.h"
#include "common.h"
#include <assert.h>

using namespace csl::common;
using namespace csl::sec;
using namespace csl::comm;

/** @brief contains tests related to udp clients */
namespace test_udp_auth_client {

  void basic()
  {
    udp::hello_cli ch;
    udp::auth_cli ca;
    SAI h,a;

    ch.use_exc(false);
    ca.use_exc(false);
    ca.debug(false);

    memset( &h,0,sizeof(h) );
    h.sin_family       = AF_INET;
    h.sin_addr.s_addr  = htonl(INADDR_LOOPBACK);
    h.sin_port         = htons(47781);
    a = h;
    a.sin_port         = htons(47782);

    ch.addr(h);
    ca.addr(a);

    ecdh_key pubkey;
    bignum   privkey;

    pubkey.algname("prime192v3");

    /* generate keypair */
    assert( pubkey.gen_keypair(privkey) == true );

    ch.private_key(privkey);
    ch.public_key(pubkey);

    ca.private_key(privkey);
    ca.public_key(pubkey);
    ca.login("LLL");
    ca.pass("PPP");

    assert( ch.hello( 3000 ) == true );

    ca.server_public_key(ch.server_public_key());

    assert( ca.auth( 3000 ) == true );
  }

  static udp::hello_cli * global_hello_client_ = 0;
  static udp::auth_cli  * global_auth_client_ = 0;

  void hello()
  {
    assert( global_hello_client_->hello( 3000 ) == true );
  }

  void start()
  {
    assert( global_auth_client_->auth( 3000 ) == true );
  }

} // end of test_udp_auth_client

using namespace test_udp_auth_client;

int main()
{
  udp::hello_cli ch_global;
  udp::auth_cli  ca_global;

  SAI h,a;

  ch_global.use_exc(false);
  ca_global.use_exc(false);

  memset( &h,0,sizeof(h) );
  h.sin_family       = AF_INET;
  h.sin_addr.s_addr  = htonl(INADDR_LOOPBACK);

  h.sin_port         = htons(47781);
  ch_global.addr(h);

  a = h;

  a.sin_port         = htons(47782);
  ca_global.addr(a);

  ecdh_key pubkey;
  bignum   privkey;

  pubkey.algname("prime192v3");

  /* generate keypair */
  assert( pubkey.gen_keypair(privkey) == true );

  ch_global.private_key(privkey);
  ch_global.public_key(pubkey);
  ca_global.private_key(privkey);
  ca_global.public_key(pubkey);
  ca_global.login("LLL");
  ca_global.pass("PPP");

  global_hello_client_ = &ch_global;
  global_auth_client_ = &ca_global;

  csl_common_print_results( "basic      ", csl_common_test_timer_v0(basic),"" );
  csl_common_print_results( "hello      ", csl_common_test_timer_v0(hello),"" );

  ca_global.server_public_key(ch_global.server_public_key());

  csl_common_print_results( "start      ", csl_common_test_timer_v0(start),"" );

  return 0;
}

/* EOF */
