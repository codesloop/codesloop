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
   @file t__udp_server2.cc
   @brief Tests to verify udp::srv routines
 */

#include "udp_hello.hh"
#include "test_timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

using namespace csl::common;
using namespace csl::comm;
using namespace csl::sec;

/** @brief contains tests related to udp servers */
namespace test_udp_server2 {

  void basic()
  {
    udp::hello_srv s;
    udp::SAI h;

    s.use_exc(false);
    s.debug(false);

    memset( &h,0,sizeof(h) );
    h.sin_family       = AF_INET;
    h.sin_addr.s_addr  = htonl(INADDR_LOOPBACK);
    h.sin_port         = htons(47781);

    s.addr( h );

    ecdh_key pubkey;
    bignum   privkey;

    pubkey.algname("prime192v3");

    /* generate keypair */
    assert( pubkey.gen_keypair(privkey) == true );

    s.private_key( privkey );
    s.public_key( pubkey );

    assert( s.start() == true );

    SleepSeconds( 60 );
  }

} // end of test_udp_server

using namespace test_udp_server2;

int main()
{
  //csl_common_print_results( "simplest      ", csl_common_test_timer_i1(simplest,0),"" );
  basic();
  return 0;
}

/* EOF */
