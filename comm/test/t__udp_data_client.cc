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
   @file t__udp_data_client.cc
   @brief Tests to verify udp::data_cli routines
 */

#include "udp_hello.hh"
#include "udp_auth.hh"
#include "udp_data.hh"
#include "test_timer.h"
#include "common.h"
#include <assert.h>

using namespace csl::common;
using namespace csl::sec;
using namespace csl::comm;

/** @brief contains tests related to udp data clients */
namespace test_udp_data_client {

  void basic()
  {
    udp::hello_cli ch;
    udp::auth_cli  ca;
    udp::data_cli  cd;
    udp::SAI h,a,d;

    ch.use_exc(false);
    ca.use_exc(false);
    cd.use_exc(false);
    ca.debug(false);
    cd.debug(true);

    memset( &h,0,sizeof(h) );
    h.sin_family       = AF_INET;
    h.sin_addr.s_addr  = htonl(INADDR_LOOPBACK);
    h.sin_port         = htons(47781);
    d = a = h;
    a.sin_port         = htons(47782);
    d.sin_port         = htons(47783);

    ch.addr(h);
    ca.addr(a);
    cd.addr(d);

    ecdh_key pubkey;
    bignum   privkey;

    pubkey.algname("prime192v3");

    /* generate keypair */
    assert( pubkey.gen_keypair(privkey) == true );

    ch.private_key(privkey);
    ch.public_key(pubkey);

    assert( ch.hello( 3000 ) == true );

    ca.private_key(privkey);
    ca.public_key(pubkey);
    ca.login("LLL");
    ca.pass("PPP");
    ca.server_public_key(ch.server_public_key());

    assert( ca.auth( 3000 ) == true );

    cd.server_salt( ca.server_salt() );
    cd.my_salt( ca.my_salt() );
    cd.session_key( ca.session_key() );

    udp::b1024_t in,out;
    in.set( reinterpret_cast<const unsigned char *>("hello"),6 );

    assert( cd.send(in) == true );
    assert( cd.recv(out,3000) == true );
    assert( memcmp( out.data(),"HELLO",6 ) == 0 );

    assert( cd.send(in) == true );
    assert( cd.recv(out,3000) == true );
    assert( memcmp( out.data(),"HELLO",6 ) == 0 );

    assert( cd.send(in) == true );
    assert( cd.recv(out,3000) == true );
    assert( memcmp( out.data(),"HELLO",6 ) == 0 );

    assert( cd.send(in) == true );
    assert( cd.recv(out,3000) == true );
    assert( memcmp( out.data(),"HELLO",6 ) == 0 );
  }

  static udp::hello_cli * global_hello_client_  = 0;
  static udp::auth_cli  * global_auth_client_   = 0;
  static udp::data_cli  * global_data_client_   = 0;

  void hello()
  {
    assert( global_hello_client_->hello( 3000 ) == true );
  }

  void start()
  {
    assert( global_auth_client_->auth( 3000 ) == true );
  }

  void data()
  {
    udp::b1024_t in,out;
    in.set( reinterpret_cast<const unsigned char *>("hello"),6 );
    assert( global_data_client_->send(in) == true );
    assert( global_data_client_->recv(out,3000) == true );
    assert( memcmp( out.data(),"HELLO",6 ) == 0 );
  }
} // end of test_udp_data_client

using namespace test_udp_data_client;

int main()
{
  udp::hello_cli ch_global;
  udp::auth_cli  ca_global;
  udp::data_cli  cd_global;

  udp::SAI h,a,d;

  ch_global.use_exc(false);
  ca_global.use_exc(false);
  cd_global.use_exc(false);
  cd_global.debug(true);

  memset( &h,0,sizeof(h) );
  h.sin_family       = AF_INET;
  h.sin_addr.s_addr  = htonl(INADDR_LOOPBACK);

  h.sin_port         = htons(47781);
  d = a = h;
  a.sin_port         = htons(47782);
  d.sin_port         = htons(47783);

  ch_global.addr(h);
  ca_global.addr(a);
  cd_global.addr(d);

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

  global_hello_client_  = &ch_global;
  global_auth_client_   = &ca_global;
  global_data_client_   = &cd_global;

  ch_global.use_exc(false);
  ca_global.use_exc(false);
  cd_global.use_exc(false);

  csl_common_print_results( "basic      ", csl_common_test_timer_v0(basic),"" );
  csl_common_print_results( "hello      ", csl_common_test_timer_v0(hello),"" );

  ca_global.server_public_key(ch_global.server_public_key());

  csl_common_print_results( "start      ", csl_common_test_timer_v0(start),"" );

  cd_global.server_salt( ca_global.server_salt() );
  cd_global.my_salt( ca_global.my_salt() );
  cd_global.session_key( ca_global.session_key() );

  csl_common_print_results( "data       ", csl_common_test_timer_v0(data),"" );

  return 0;
}

/* EOF */
