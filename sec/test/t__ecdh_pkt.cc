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
   @file t__ecdh_pkt.cc
   @brief Tests to verify ecdh_pkt
*/

#include "ecdh_pkt.hh"
#include "test_timer.h"
#include "zfile.hh"
#include "mpool.hh"
#include "csl_sec.h"
#include "common.h"
#include "str.hh"
#include <assert.h>

using namespace csl::sec;
using csl::common::zfile;
using csl::common::mpool;

/** @brief contains tests related to ecdh_pkt */
namespace test_ecdh_pkt {

const char * algs_subset[] = {
  "prime192v3", // rel speed  7 : >80b
  "secp224r1",  // rel speed 10 : >80b
  "prime256v1", // rel speed 13 : 128b
  "secp384r1",  // rel speed 33 : 192b
  "secp521r1",  // rel speed 65 : 256b
  NULL };

/**
    @test Check how zfile compresses a certificate
*/
void test_zfile( )
{
  zfile zf;
  zf.custom_zlib_allocator(true);
  zf.init_custom_memory(true); /* to make valgrind happy */

  assert( zf.read_file("server/server.crt") );
  assert( zf.get_size() > 500 );
  assert( zf.get_zsize() > 10 );

  printf("Compress cert [server/server.crt]: %ld => %ld\n", (unsigned long)zf.get_size(), (unsigned long)zf.get_zsize() );
}

/**
    @test Check that add_algorithms() really works
*/
void test_alg( )
{
  common::str dbg;
  ecdh_pkt ph;
  assert( ph.add_algorithms(algs_subset) == true );
}

/**
    @test Verify add_keys_to_challenge()
*/
void test_addkeys( )
{
  common::str dbg;
  ecdh_pkt ph;
  assert( ph.add_algorithms(algs_subset) == true );
  assert( ph.add_keys_to_challenge(CSL_EC_STRENGTH_MIN, CSL_EC_STRENGTH_MAX) == true );
}

/**
    @test Verify get_xdr_challenge()
*/
void test_addkey( int which )
{
  unsigned char * xdta;
  size_t sz;
  ecdh_pkt ph;
  assert( ph.add_algorithms(algs_subset) == true );
  assert( ph.add_keys_to_challenge(which, which) == true );
  assert( ph.get_xdr_challenge(xdta,sz) == true );
}

/**
    @test Verify get_xdr_challenge() and print debug info
*/
void test_addkey_dbg( int from, int to )
{
  common::str dbg;
  ecdh_pkt ph;
  unsigned char * xdta;
  size_t sz;
  assert( ph.add_algorithms(algs_subset) == true );
  assert( ph.add_keys_to_challenge(from, to) == true );
  assert( ph.get_xdr_challenge(xdta,sz) == true );
  // result in: ecdh_test_addkey_dbg.xdrbin
}

/**
    @test Verify add_cert_file_to_challenge() and get_xdr_challenge()
*/
void test_add_cert( int dbgme )
{
  common::str dbg;
  ecdh_pkt ph;
  unsigned char * xdta;
  size_t sz1=0,sz2=0;

  assert( ph.add_algorithms(algs_subset) == true );
  assert( ph.add_keys_to_challenge(CSL_EC_STRENGTH_MIN, CSL_EC_STRENGTH_MAX) == true );
  assert( ph.get_xdr_challenge(xdta,sz1) == true );
  assert( ph.add_cert_file_to_challenge("server/server.crt") == true );
  assert( ph.get_xdr_challenge(xdta,sz2) == true );

  if( dbgme )
  {
    printf("keys+cert: XDR: [%ld bytes => %ld bytes]\n%s",(unsigned long)sz1,(unsigned long)sz2,dbg.c_str() );
    // result in: ecdh_test_add_cert.xdrbin
  }
}

/**
    @test Verify get_xdr_challenge() and set_xdr_challenge()
*/
void test_set_xdr( )
{
  ecdh_pkt ph,ph2;
  unsigned char * xdta;
  size_t sz=0;

  assert( ph.add_algorithms(algs_subset) == true );
  assert( ph.add_keys_to_challenge(CSL_EC_STRENGTH_MIN, CSL_EC_STRENGTH_MAX) == true );
  assert( ph.add_cert_file_to_challenge("server/server.crt") == true );
  assert( ph.get_xdr_challenge(xdta,sz) == true );

  assert( ph2.set_xdr_challenge(xdta,sz) == true );
}

/**
    @test Verify session key generation and verify that they match at both sides
*/
void test_session_key( int dbgme )
{
  ecdh_pkt ph_srv,ph_cli;
  unsigned char *xdta,*xdtb;
  size_t sz=0,s2=0;

  assert( ph_srv.add_algorithms(algs_subset) == true );
  assert( ph_srv.add_keys_to_challenge(CSL_EC_STRENGTH_MIN, CSL_EC_STRENGTH_MAX) == true );
  assert( ph_srv.add_cert_file_to_challenge("server/server.crt") == true );
  assert( ph_srv.get_xdr_challenge(xdta,sz) == true );

  assert( ph_cli.add_algorithms(algs_subset) == true );
  assert( ph_cli.set_xdr_challenge(xdta,sz) == true );
  assert( ph_cli.add_cert_file_to_response("client/client.crt") == true );

  assert( ph_cli.add_key_to_response(CSL_EC_STRENGTH_MIN,CSL_EC_STRENGTH_MAX ) == true );
  assert( ph_cli.get_xdr_response(xdtb,s2) == true );

  assert( ph_srv.set_xdr_response(xdtb,s2) == true );

  unsigned char * ses_srv = ph_srv.get_session_key();
  unsigned char * ses_cli = ph_cli.get_session_key();

  assert( ses_srv != NULL );
  assert( ses_cli != NULL );

  assert( strlen((char *)ses_srv) == SHA1_HEX_DIGEST_STR_LENGTH-1 );
  assert( strlen((char *)ses_cli) == SHA1_HEX_DIGEST_STR_LENGTH-1 );

  assert( strcmp((char *)ses_srv,(char *)ses_cli) == 0 );
}

/**
    @test Verify client credetials at both sides
*/
void test_creds( int dbgme )
{
  ecdh_pkt ph_srv,ph_cli;
  unsigned char *xdta,*xdtb;
  size_t sz=0,s2=0;

  assert( ph_srv.add_algorithms(algs_subset) == true );
  assert( ph_srv.add_keys_to_challenge(CSL_EC_STRENGTH_MIN, CSL_EC_STRENGTH_MAX) == true );
  assert( ph_srv.add_cert_file_to_challenge("server/server.crt") == true );
  assert( ph_srv.get_xdr_challenge(xdta,sz) == true );

  assert( ph_cli.add_algorithms(algs_subset) == true );
  assert( ph_cli.set_xdr_challenge(xdta,sz) == true );
  assert( ph_cli.add_cert_file_to_response("client/client.crt") == true );

  assert( ph_cli.add_key_to_response(CSL_EC_STRENGTH_MIN,CSL_EC_STRENGTH_MAX ) == true );
  assert( ph_cli.add_creds_to_response("Login","Pass") == true );
  assert( ph_cli.get_xdr_response(xdtb,s2) == true );

  assert( ph_srv.set_xdr_response(xdtb,s2) == true );

  assert( strcmp(ph_srv.get_peer_login(),"Login")==0 );
  assert( strcmp(ph_srv.get_peer_password(),"Pass")==0 );

  if( dbgme )
  {
    zfile zfc,zfs;
    zfs.put_data( xdta,sz );
    zfs.write_file( "ecdh_test_creds_srv.xdrbin" );

    zfc.put_data( xdtb, s2 );
    zfc.write_file( "ecdh_test_creds_cli.xdrbin" );
  }
}

/**
    @test Check client credetials and certificate
*/
void test_certs( int dbgme )
{
  unsigned char client_cert[16000];
  unsigned char server_cert[16000];

  ecdh_pkt ph_srv,ph_cli;
  unsigned char *xdta,*xdtb;
  size_t sz=0,s2=0;
  zfile zc,zs;
  size_t scert_sz,ccert_sz;
  size_t scert_sz2,ccert_sz2;

  zc.read_file("client/client.crt");
  ccert_sz = zc.get_size();
  zc.get_data( client_cert );

  zs.read_file("server/server.crt");
  scert_sz = zs.get_size();
  zs.get_data( server_cert );

  assert( ph_srv.add_algorithms(algs_subset) == true );
  assert( ph_srv.add_keys_to_challenge(CSL_EC_STRENGTH_MIN, CSL_EC_STRENGTH_MAX) == true );
  assert( ph_srv.add_cert_file_to_challenge("server/server.crt") == true );
  assert( ph_srv.get_xdr_challenge(xdta,sz) == true );

  assert( ph_cli.add_algorithms(algs_subset) == true );
  assert( ph_cli.set_xdr_challenge(xdta,sz) == true );
  assert( ph_cli.add_cert_file_to_response("client/client.crt") == true );

  assert( ph_cli.add_key_to_response(CSL_EC_STRENGTH_MIN,CSL_EC_STRENGTH_MAX ) == true );
  assert( ph_cli.add_creds_to_response("Login","Pass") == true );
  assert( ph_cli.get_xdr_response(xdtb,s2) == true );

  assert( ph_srv.set_xdr_response(xdtb,s2) == true );

  assert( strcmp(ph_srv.get_peer_login(),"Login")==0 );
  assert( strcmp(ph_srv.get_peer_password(),"Pass")==0 );

  assert( memcmp( ph_srv.get_peer_cert(ccert_sz2), client_cert, ccert_sz ) == 0 );
  assert( memcmp( ph_cli.get_peer_cert(scert_sz2), server_cert, scert_sz ) == 0 );
  assert( ccert_sz == ccert_sz2 );
  assert( scert_sz == scert_sz2 );
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
  void test_random()
  {
    rndata * p = random_files;
    mpool<> mp;

    while( p->len_ )
    {
      zfile zf;
      assert( zf.read_file(p->filename_) == true );
      assert( zf.get_size() == p->len_ );
      unsigned char * dta = (unsigned char *)mp.allocate(p->len_);
      assert( zf.get_data(dta) == true );

      ecdh_pkt ph_srv,ph_cli;

      assert( ph_srv.set_xdr_response(dta,p->len_) == false );
      assert( ph_cli.set_xdr_challenge(dta,p->len_) == false );

      ++p;
    };
  }

} // end of test_ecdh_pkt

using namespace test_ecdh_pkt;

int main()
{
  csl_common_print_results( "test_alg         ",
          csl_common_test_timer_v0(test_alg), "" );
  csl_common_print_results( "test_addkeys     ",
          csl_common_test_timer_v0(test_addkeys), "" );
  csl_common_print_results( "test_addkey[192] ",
          csl_common_test_timer_i1(test_addkey,192), "" );
  csl_common_print_results( "test_addkey[224] ",
          csl_common_test_timer_i1(test_addkey,224), "" );
  csl_common_print_results( "test_addkey[256] ",
          csl_common_test_timer_i1(test_addkey,256), "" );
  csl_common_print_results( "test_addkey[384] ",
          csl_common_test_timer_i1(test_addkey,384), "" );
  csl_common_print_results( "test_addkey[521] ",
          csl_common_test_timer_i1(test_addkey,521), "" );
  csl_common_print_results( "test_add_cert    ",
          csl_common_test_timer_i1(test_add_cert,0), "" );
  csl_common_print_results( "test_set_xdr     ",
          csl_common_test_timer_v0(test_set_xdr), "" );
  csl_common_print_results( "test_session_key ",
          csl_common_test_timer_i1(test_session_key,0), "" );
  csl_common_print_results( "test_creds       ",
          csl_common_test_timer_i1(test_creds,0), "" );
  csl_common_print_results( "test_certs       ",
          csl_common_test_timer_i1(test_certs,0), "" );

  csl_common_print_results( "test_random      ",
          csl_common_test_timer_v0(test_random), "" );

  test_creds(1);
  test_addkey_dbg(256,521);
  test_add_cert(1);
  test_zfile();
  test_session_key(1);
  return 0;
}

/* EOF */
