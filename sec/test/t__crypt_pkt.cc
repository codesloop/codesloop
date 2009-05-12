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
   @file t__crypt_pkt.cc
   @brief Tests to verify crypt_pkt
 */

#include "pbuf.hh"
#include "zfile.hh"
#include "crypt_pkt.hh"
#include "crypt_buf.hh"
#include "test_timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

using csl::sec::crypt_pkt;
using csl::sec::crypt_buf;
using csl::common::zfile;
using csl::common::pbuf;

/** @brief contains tests related to crypt_pkt */
namespace test_crypt_pkt {

  void print_hex(char * prefix,const void * vp,size_t len)
  {
    const unsigned char * hx = (const unsigned char *)vp;
    printf("%s: ",prefix);
    for(size_t i=0;i<len;++i) printf("%.2X",hx[i]);
    printf("\n");
  }

  void baseline()
  {
    crypt_pkt pk;
  }

  void old_crypt(int dbg)
  {
    char buf[48+CSL_SEC_CRYPT_BUF_HEAD_LEN+CSL_SEC_CRYPT_BUF_MAC_LEN];
    char buf2[48+CSL_SEC_CRYPT_BUF_HEAD_LEN+CSL_SEC_CRYPT_BUF_MAC_LEN];

    memcpy(buf+8,"Hello World Hello World Hello World Hello World",48);
    memcpy(buf2+8,"Hello World Hello World Hello World Hello World",48);

    crypt_buf cre;
    assert( cre.init_crypt( (unsigned char *)buf,
           (const unsigned char *)"Hello World 012345678",
            22,
            true,
           (const unsigned char *)"012345678") == true );

    assert( cre.add_data((unsigned char *)buf+8,48,true) == true );
    assert( cre.finalize((unsigned char *)buf+56) == true );

    if( dbg )
    {
      print_hex("O HEAD: ",buf,8);
      print_hex("O D0:   ",buf2+8,48);
      print_hex("O DATA: ",buf+8,48);
      print_hex("O FOOT: ",buf+56,40);
    }
  }

  void new_crypt(int dbg)
  {
    crypt_pkt::saltbuf_t salt;
    crypt_pkt::keybuf_t  key;
    crypt_pkt::headbuf_t head;
    crypt_pkt::databuf_t data,d0;
    crypt_pkt::footbuf_t foot;

    salt.set((const unsigned char *)"012345678",8);
    key.set((const unsigned char *)"Hello World 012345678",22);
    data.set((const unsigned char *)"Hello World Hello World Hello World Hello World",48);
    d0 = data;

    crypt_pkt pk;
    assert( pk.encrypt( salt,key,head,data,foot ) == true );

    if( dbg )
    {
      print_hex("N HEAD: ",head.data(),head.size());
      print_hex("N D0:   ",d0.data(),  d0.size());
      print_hex("N DATA: ",data.data(),data.size());
      print_hex("N FOOT: ",foot.data(),foot.size());
    }
  }

  void bad_crypt(int dbg)
  {
    crypt_pkt::saltbuf_t salt;
    crypt_pkt::keybuf_t  key;
    crypt_pkt::headbuf_t head;
    crypt_pkt::databuf_t data,d0;
    crypt_pkt::footbuf_t foot;

    salt.set((const unsigned char *)"012345678",8);
    key.set((const unsigned char *)"Hello World 012345678",22);
    data.set((const unsigned char *)"Hello World Hello World Hello World Hello World",48);
    d0 = data;

    crypt_pkt pk;
    assert( pk.encrypt( salt,key,head,data,foot ) == true );

    if( dbg )
    {
      print_hex("N HEAD: ",head.data(),head.size());
      print_hex("N D0:   ",d0.data(),  d0.size());
      print_hex("N DATA: ",data.data(),data.size());
      print_hex("N FOOT: ",foot.data(),foot.size());
    }

    data.private_data()[4]=0xbd;
    data.private_data()[5]=0xbd;
    data.private_data()[6]=0xbd;
    data.private_data()[7]=0xbd;

    assert( pk.decrypt( key,head,data,foot ) == false );

    if( dbg )
    {
      print_hex("N HEAD: ",head.data(),head.size());
      print_hex("N D0:   ",d0.data(),  d0.size());
      print_hex("N DATA: ",data.data(),data.size());
      print_hex("N FOOT: ",foot.data(),foot.size());
    }
  }

  void old_crypt2(int dbg)
  {
    char buf[900+CSL_SEC_CRYPT_BUF_HEAD_LEN+CSL_SEC_CRYPT_BUF_MAC_LEN];
    char buf2[900+CSL_SEC_CRYPT_BUF_HEAD_LEN+CSL_SEC_CRYPT_BUF_MAC_LEN];

    memset(buf,'A',sizeof(buf));
    memset(buf2,'A',sizeof(buf2));

    crypt_buf cre;

    assert( cre.init_crypt( (unsigned char *)buf,
            (const unsigned char *)"Hello World 012345678",
             22,
             true,
             (const unsigned char *)"012345678") == true );

    assert( cre.add_data((unsigned char *)buf+8,900,true) == true );
    assert( cre.finalize((unsigned char *)buf+900+8) == true );

    crypt_buf crd;

    assert( crd.init_crypt( (unsigned char *)buf,
            (const unsigned char *)"Hello World 012345678",
            22,
            false,
            (const unsigned char *)"012345678") == true );

    assert( crd.add_data((unsigned char *)buf+8,900,false) == true );
    assert( crd.finalize((unsigned char *)buf+900+8) == true );

    assert( memcmp(buf+8,buf2+8,900) == 0 );

    if( dbg )
    {
      print_hex("B  : ",buf+8,900);
      print_hex("B2 : ",buf2+8,900);
    }
  }

  void new_crypt2(int dbg)
  {
    char buf[900];

    memset(buf,'A',sizeof(buf));

    crypt_pkt::saltbuf_t salt;
    crypt_pkt::keybuf_t  key;
    crypt_pkt::headbuf_t head;
    crypt_pkt::databuf_t data;
    crypt_pkt::footbuf_t foot;

    salt.set((const unsigned char *)"012345678",8);
    key.set((const unsigned char *)"Hello World 012345678",22);
    data.set((const unsigned char *)buf,900);

    crypt_pkt pk;

    assert( pk.encrypt( salt,key,head,data,foot ) == true );
    assert( pk.decrypt( key,head,data,foot ) == true );

    assert( memcmp(buf,data.data(),900) == 0 );

    if( dbg )
    {
      print_hex("B  : ",data.data(),data.size());
      print_hex("Bo : ",buf,900);
    }
  }

  struct rndata
  {
    size_t len_;
    const char * filename_;
  };

  static rndata random_files[] = {
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

  void random_test()
  {
    rndata * p = random_files;
    while( p->len_ )
    {
      zfile zf;
      assert( zf.read_file(p->filename_) == true );
      assert( zf.get_size() == p->len_ );
      pbuf pb;
      assert( zf.get_data(pb) == true );

      crypt_pkt::keybuf_t  key;
      crypt_pkt::saltbuf_t salt;
      crypt_pkt::headbuf_t head;
      crypt_pkt::databuf_t data;
      crypt_pkt::footbuf_t foot;

      key.set((const unsigned char *)"Hello World 012345678",22);
      pb.t_copy_to( salt,8 );
      pb.t_copy_to( head,8 );
      pb.t_copy_to( data );
      pb.t_copy_to( foot,16 );

      crypt_pkt pk;

      assert( pk.decrypt( key,head,data,foot ) == false );

      ++p;
    };
  }

} // end of test_crypt_pkt

using namespace test_crypt_pkt;

int main()
{
  bad_crypt(1);

  csl_common_print_results( "baseline      ", csl_common_test_timer_v0(baseline),"" );
  csl_common_print_results( "old_crypt     ", csl_common_test_timer_i1(old_crypt,0),"" );
  csl_common_print_results( "new_crypt     ", csl_common_test_timer_i1(new_crypt,0),"" );

  csl_common_print_results( "old_crypt2    ", csl_common_test_timer_i1(old_crypt2,0),"" );
  csl_common_print_results( "new_crypt2    ", csl_common_test_timer_i1(new_crypt2,0),"" );

  csl_common_print_results( "random_test   ", csl_common_test_timer_v0(random_test),"" );

  return 0;
}

/* EOF */
