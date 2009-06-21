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
   @file t__umac.cc
   @brief Tests to verify umac routines
 */

#include "umac_ae.h"
#include "tbuf.hh"
#include "test_timer.h"
#include "common.h"
#include <assert.h>

using namespace csl::common;

/** @brief contains tests related to umac routines */
namespace test_umac {

  void print_hex(const char * prefix,const void * vp,size_t len)
  {
    const unsigned char * hx = reinterpret_cast<const unsigned char *>(vp);
    printf("%s: ",prefix);
    for(size_t i=0;i<len;++i) printf("%.2X",hx[i]);
    printf("\n");
  }

  void simplest(int dbg)
  {
    umac_ae_ctx_t * ctx = reinterpret_cast<umac_ae_ctx_t *>(malloc(sizeof(umac_ae_ctx_t)));
    memset( ctx,0,sizeof(umac_ae_ctx_t) );

    tbuf<256> nonce_x0;
    tbuf<256> key_x0;
    tbuf<256> ct_x0;
    tbuf<256> m_x0;
    tbuf<256> tag_x0;

    size_t len = 32;

    char * key   = reinterpret_cast<char *> ( key_x0.allocate(16+48) );
    char * m     = reinterpret_cast<char *> ( m_x0.allocate(64+48) );
    char * ct    = reinterpret_cast<char *> ( ct_x0.allocate(64+48) );
    char * nonce = reinterpret_cast<char *> ( nonce_x0.allocate(8+48) );
    char * tag   = reinterpret_cast<char *> ( tag_x0.allocate(16+16) );

    key    += (16-((reinterpret_cast<unsigned long long>(key))&15));
    m      += (16-((reinterpret_cast<unsigned long long>(m))&15));
    ct     += (16-((reinterpret_cast<unsigned long long>(ct))&15));
    nonce  += (16-((reinterpret_cast<unsigned long long>(nonce))&15));
    tag    += (16-((reinterpret_cast<unsigned long long>(tag))&15));

    memcpy( key,"0123456789abcdef",16 );
    memcpy( m,"ABCDEFGHabcdefghABCDEFGHabcdefgh",32);
    memcpy( nonce,"_1_2_3_4",8 );

    umac_ae_set_key(key, ctx);
    umac_ae_encrypt(m, ct, len, nonce, ctx);
    umac_ae_finalize(tag, ctx);
    umac_ae_done(ctx);

    free(ctx);

    if( dbg )
    {
      print_hex("TAG: ",tag,16);
      print_hex("CT:  ",ct,32);
      print_hex("AT:  ",m,32);
    }
  }

} // end of test_umac

using namespace test_umac;

int main()
{
  csl_common_print_results( "simplest      ", csl_common_test_timer_i1(simplest,0),"" );
  simplest(1);

  return 0;
}

/* EOF */
