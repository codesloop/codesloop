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
   @file t__udp_pkt.cc
   @brief Tests to verify udp_pkt routines
 */

#include "udp_pkt.hh"
#include "test_timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

using namespace csl::common;
using namespace csl::sec;
using namespace csl::comm;

/** @brief contains tests related to udp clients */
namespace test_udp_pkt {

  void basic()
  {
    udp_pkt p;
  }

  void init0()
  {
    udp_pkt p;

    ecdh_key cli_pubk,srv_pubk;
    bignum   cli_privk,srv_privk;
  }

  void init1()
  {
    udp_pkt p;

    ecdh_key cli_pubk,srv_pubk;
    bignum   cli_privk,srv_privk;

    cli_pubk.algname("prime192v3");
    srv_pubk.algname("prime192v3");

    assert( cli_pubk.gen_keypair(cli_privk) == true );
    assert( srv_pubk.gen_keypair(srv_privk) == true );
  }

  void print_hex(const char * prefix,const void * vp,size_t len)
  {
    unsigned char * hx = (unsigned char *)vp;
    printf("%s [%ld]: ",prefix,(unsigned long)len);
    for(size_t i=0;i<len;++i)
    {
      if( (hx[i]<='Z' && hx[i]>='A') ||
           (hx[i]<='9' && hx[i]>='0') ||
           (hx[i]<='z' && hx[i]>='A') )
        printf(".%c",hx[i]);
      else
        printf("%.2X",hx[i]);
    }
    printf("\n");
  }

  void content(int dbg)
  {
    udp_pkt cli;
    cli.debug(dbg == 1);

    ecdh_key cli_pubk,srv_pubk;
    bignum   cli_privk,srv_privk;
    udp_srv_info info;

    cli_pubk.algname("prime192v3");
    srv_pubk.algname("prime192v3");

    assert( cli_pubk.gen_keypair(cli_privk) == true );
    assert( srv_pubk.gen_keypair(srv_privk) == true );

    cli.own_pubkey(cli_pubk);
    cli.own_privkey(cli_privk);

    unsigned int hello_len = 0;
    assert( cli.prepare_hello(hello_len) != 0 );

    if( dbg )
    {
      print_hex(">HELLO",cli.data(),hello_len);
    }

    info.public_key(srv_pubk);

    udp_pkt srv;
    srv.debug(dbg == 1);
    srv.own_pubkey(srv_pubk);
    srv.own_privkey(srv_privk);
    srv.srv_info(info);

    memcpy( srv.data(),cli.data(),hello_len );
    assert( srv.init_hello(hello_len) == true );

    unsigned int olleh_len = 0;
    assert( srv.prepare_olleh(olleh_len) != 0 );

    if( dbg )
    {
      print_hex(">OLLEH",srv.data(),olleh_len);
    }

    memcpy( cli.data(),srv.data(),olleh_len );
    assert( cli.init_olleh(olleh_len) == true );

    /* check auth packets */
    memset( cli.rand(),'A',64 );
    cli.login("Scho:n Ubul");
    cli.pass("Futrinka utca");
    cli.session_key("Lukas csokolade");

    unsigned int auth_len = 0;
    assert( cli.prepare_uc_auth(auth_len) != 0 );

    if( dbg )
    {
      print_hex(">AUTH",cli.data(),auth_len);
    }

    memcpy( srv.data(),cli.data(),auth_len );
    assert( srv.init_uc_auth(auth_len) == true );

    /* check auth response: htua */
    memset( srv.rand(),'B',64 );
    memset( srv.salt(),'A',8 );

    unsigned int htua_len = 0;
    assert( srv.prepare_uc_htua(htua_len) != 0 );

    if( dbg )
    {
      print_hex(">HTUA",srv.data(),htua_len);
    }

    memcpy( cli.data(),srv.data(),htua_len );
    assert( cli.init_uc_htua(htua_len) == true );
  }

} // end of test_udp_pkt

using namespace test_udp_pkt;

int main()
{
  content(1);

  csl_common_print_results( "basic      ", csl_common_test_timer_v0(basic),"" );
  csl_common_print_results( "init0      ", csl_common_test_timer_v0(init0),"" );
  csl_common_print_results( "init1      ", csl_common_test_timer_v0(init1),"" );
  csl_common_print_results( "content    ", csl_common_test_timer_i1(content,0),"" );

  return 0;
}

/* EOF */
