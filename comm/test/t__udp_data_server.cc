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
   @file t__udp_data_server.cc
   @brief Tests to verify udp::data_srv routines
 */

#include "udp_hello.hh"
#include "udp_auth.hh"
#include "udp_data.hh"
#include "test_timer.h"
#include "common.h"
#include <assert.h>

using namespace csl::common;
using namespace csl::comm;
using namespace csl::sec;

/** @brief contains tests related to udp data servers */
namespace test_udp_data_server {

  void basic()
  {
    udp::hello_srv hellosrv;
    udp::auth_srv authsrv;
    udp::data_srv datasrv;
    udp::SAI h,a,d;

    hellosrv.use_exc(false);
    authsrv.use_exc(false);
    authsrv.debug(false);

    memset( &h,0,sizeof(h) );
    h.sin_family       = AF_INET;
    h.sin_addr.s_addr  = htonl(INADDR_LOOPBACK);
    h.sin_port         = htons(47781);

    d = a = h;
    a.sin_port = htons(47782);
    d.sin_port = htons(47783);

    hellosrv.addr(h);
    authsrv.addr(a);
    datasrv.addr(d);

    ecdh_key pubkey;
    bignum   privkey;

    pubkey.algname("prime192v3");

    /* generate keypair */
    assert( pubkey.gen_keypair(privkey) == true );

    authsrv.private_key(privkey);
    authsrv.public_key(pubkey);

    hellosrv.private_key(privkey);
    hellosrv.public_key(pubkey);

    //datasrv.private_key(privkey); // NOT NEEDED, use lookup session cbs instead
    //datasrv.public_key(pubkey);   // NOT NEEDED, use lookup session cbs instead

    // TODO set auth callbacks
    // TODO set data callbacks

    assert( hellosrv.start() );
    assert( authsrv.start() );
    assert( datasrv.start() );

    SleepSeconds( 60 );
  }

} // end of test_udp_data_server

using namespace test_udp_data_server;

int main()
{
  basic();
  return 0;
}

/* EOF */
