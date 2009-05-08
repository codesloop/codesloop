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
   @file t__crypt_buf.cc
   @brief Tests to verify crypt_buf
*/

#include "crypt_buf.hh"
#include "test_timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

using csl::sec::crypt_buf;

/** @brief contains tests related to crypt_buf */
namespace test_crypt_buf {

void print_hex(char * prefix,unsigned char * hx,size_t len)
{
  printf("%s: ",prefix);
  for(size_t i=0;i<len;++i) printf("%.2X",hx[i]);
  printf("\n");
}


/**
   @test Crypto-streaming test 1
   @param l is an array that represents the stream block sizes

   This test verifies that multiple small blocks of possibly different sizes
   can be added, encrypted and decrypted. It also verifies that MAC matches.

   This test uses C string key.
*/
void test_crypt(size_t * l)
{
  unsigned char plain[200];
  unsigned char cryp[200];
  unsigned char ck[200];
  unsigned char mac0[200];
  unsigned char mac1[200];

  unsigned char * key = (unsigned char *)"adc83b19e793491b1c6ea0fd8b46cd9f32e592fc";

  memset( plain,0xCC,sizeof(plain) );
  size_t * k = l;

  crypt_buf cre;

  size_t ml = cre.get_mac_len();
  size_t hl = cre.get_header_len();

  assert( cre.init_crypt(plain,key,true) == true );
  memcpy( cryp,plain,sizeof(plain) );

  size_t off=hl;
  while( *k && off < (sizeof(plain)-ml) )
  {
    assert( cre.add_data(cryp+off,*k,true) == true );
    off += *k;
    ++k;

    crypt_buf crd;
    memcpy( ck,cryp,off );
    assert( crd.init_crypt(ck,key,false) == true );
    assert( crd.add_data(ck+hl,off-hl,false) == true );

    /* check the buffer -minus the header */
    assert( memcmp(ck+hl,plain+hl,off-hl)==0 );
  }

  {
    /* test validity */
    /* generate mac0 */
    assert( cre.finalize(mac0) == true );

    crypt_buf crd;
    memcpy( ck,cryp,sizeof(cryp) );
    assert( crd.init_crypt(ck,key,false) == true );
    assert( crd.add_data(ck+hl,off-hl,false) == true );

    assert( crd.finalize(mac1) == true );

    /* check the buffer -minus the header */
    assert( memcmp(ck+hl,plain+hl,off-hl)==0 );

    assert( memcmp(mac0,mac1,ml) == 0 );
  }

  {
    /* test invalid  */
    /* generate mac0 */
    assert( cre.finalize(mac0) == true );

    crypt_buf crd;
    memcpy( ck,cryp,sizeof(cryp) );

    /* try to foul mac */
    ck[hl+2] = 1;

    assert( crd.init_crypt(ck,key,false) == true );
    assert( crd.add_data(ck+hl,off-hl,false) == true );

    /* generate mac1 */
    assert( crd.finalize(mac1) == true );

    /* should NOT match */
    assert( memcmp(mac0,mac1,ml) != 0 );
  }
}

unsigned char speed_buff_[1024*1024*4];

/**
   @test Crypto speed test

   Encrypts 4MB of data using C string key/
*/
void test_speed()
{
  unsigned char * key = (unsigned char *)"adc83b19e793491b1c6ea0fd8b46cd9f32e592fc";
  crypt_buf cre;
  assert( cre.init_crypt(speed_buff_,key,true) == true );
  assert( cre.add_data(speed_buff_,sizeof(speed_buff_),true) == true );
}

/**
   @test Crypto speed test

   Encrypts 4MB of data using memory buffer key.
*/
void test_speed2()
{
  unsigned char key[20];

  memset( key,0xbd,sizeof(key) );

  crypt_buf cre;
  assert( cre.init_crypt(speed_buff_,key,sizeof(key),true) == true );
  assert( cre.add_data(speed_buff_,sizeof(speed_buff_),true) == true );
}

/**
   @test Crypto-streaming test 2
   @param l is an array that represents the stream block sizes

   This test verifies that multiple small blocks of possibly different sizes
   can be added, encrypted and decrypted. It also verifies that MAC matches.

   This test uses memory buffer key.
*/
void test_crypt2(size_t * l)
{
  unsigned char plain[200];
  unsigned char cryp[200];
  unsigned char ck[200];
  unsigned char mac0[200];
  unsigned char mac1[200];
  unsigned char key[20];

  memset( key,0xbd,sizeof(key) );
  memset( plain,0xCC,sizeof(plain) );

  size_t * k = l;

  crypt_buf cre;

  size_t ml = cre.get_mac_len();
  size_t hl = cre.get_header_len();

  assert( cre.init_crypt(plain,key,sizeof(key),true) == true );
  memcpy( cryp,plain,sizeof(plain) );

  size_t off=hl;
  while( *k && off < (sizeof(plain)-ml) )
  {
    assert( cre.add_data(cryp+off,*k,true) == true );
    off += *k;
    ++k;

    crypt_buf crd;
    memcpy( ck,cryp,off );
    assert( crd.init_crypt(ck,key,sizeof(key),false) == true );
    assert( crd.add_data(ck+hl,off-hl,false) == true );

    /* check the buffer -minus the header */
    assert( memcmp(ck+hl,plain+hl,off-hl)==0 );
  }

  {
    /* test validity */
    /* generate mac0 */
    assert( cre.finalize(mac0) == true );

    crypt_buf crd;
    memcpy( ck,cryp,sizeof(cryp) );
    assert( crd.init_crypt(ck,key,sizeof(key),false) == true );
    assert( crd.add_data(ck+hl,off-hl,false) == true );

    assert( crd.finalize(mac1) == true );

    /* check the buffer -minus the header */
    assert( memcmp(ck+hl,plain+hl,off-hl)==0 );

    assert( memcmp(mac0,mac1,ml) == 0 );
  }

  {
    /* test invalid  */
    /* generate mac0 */
    assert( cre.finalize(mac0) == true );

    crypt_buf crd;
    memcpy( ck,cryp,sizeof(cryp) );

    /* try to foul mac */
    ck[hl+2] = 1;

    assert( crd.init_crypt(ck,key,sizeof(key),false) == true );
    assert( crd.add_data(ck+hl,off-hl,false) == true );

    /* generate mac1 */
    assert( crd.finalize(mac1) == true );

    /* should NOT match */
    assert( memcmp(mac0,mac1,ml) != 0 );
  }
}


void t0() { size_t t[] = {5,0}; test_crypt( t ); }
void t1() { size_t t[] = {4,4,4,4,4,4,4,4,4,4,4,4,4,0}; test_crypt( t ); }
void t2() { size_t t[] = {8,8,8,8,8,8,8,8,8,8,8,8,8,0}; test_crypt( t ); }
void t3() { size_t t[] = {11,11,11,11,11,11,11,11,11,11,0}; test_crypt( t ); }
void t4() { size_t t[] = {19,19,19,19,19,19,19,19,19,19,0}; test_crypt( t ); }
void t5() { size_t t[] = {140,0}; test_crypt( t ); }

void z0() { size_t t[] = {5,0}; test_crypt2( t ); }
void z1() { size_t t[] = {4,4,4,4,4,4,4,4,4,4,4,4,4,0}; test_crypt2( t ); }
void z2() { size_t t[] = {8,8,8,8,8,8,8,8,8,8,8,8,8,0}; test_crypt2( t ); }
void z3() { size_t t[] = {11,11,11,11,11,11,11,11,11,11,0}; test_crypt2( t ); }
void z4() { size_t t[] = {19,19,19,19,19,19,19,19,19,19,0}; test_crypt2( t ); }
void z5() { size_t t[] = {140,0}; test_crypt2( t ); }

} // end of test_crypt_buf

using namespace test_crypt_buf;

int main()
{
  size_t t01[] = {30,0};
  size_t t02[] = {3,5,7,11,13,0};
  test_crypt( t01 );
  test_crypt( t02 );

  csl_common_print_results( "t0   ", csl_common_test_timer_v0(t0),"" );
  csl_common_print_results( "t1   ", csl_common_test_timer_v0(t1),"" );
  csl_common_print_results( "t2   ", csl_common_test_timer_v0(t2),"" );
  csl_common_print_results( "t3   ", csl_common_test_timer_v0(t3),"" );
  csl_common_print_results( "t4   ", csl_common_test_timer_v0(t4),"" );
  csl_common_print_results( "t5   ", csl_common_test_timer_v0(t5),"" );

  csl_common_print_results( "z0   ", csl_common_test_timer_v0(z0),"" );
  csl_common_print_results( "z1   ", csl_common_test_timer_v0(z1),"" );
  csl_common_print_results( "z2   ", csl_common_test_timer_v0(z2),"" );
  csl_common_print_results( "z3   ", csl_common_test_timer_v0(z3),"" );
  csl_common_print_results( "z4   ", csl_common_test_timer_v0(z4),"" );
  csl_common_print_results( "z5   ", csl_common_test_timer_v0(z5),"" );

  csl_common_print_results( "speed", csl_common_test_timer_v0(test_speed),"" );
  csl_common_print_results( "spee2", csl_common_test_timer_v0(test_speed2),"" );

  return 0;
}

/* EOF */
