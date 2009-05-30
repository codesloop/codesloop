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

#include "crypt_pkt.hh"
#include "exc.hh"
#include "csl_sec.hh"
#include "common.h"
#include "tbuf.hh"
#include "umac_ae.h"
#include <openssl/rand.h>

/**
  @file crypt_pkt.cc
  @brief implementation of crypt_pkt
 */

using csl::common::tbuf;

namespace csl
{
  namespace sec
  {
    bool crypt_pkt::encrypt( const saltbuf_t & salt,
                             const keybuf_t & key,
                             headbuf_t & header,
                             databuf_t & data,
                             footbuf_t & footer )
    {
      if( salt.size() != 8 )    { THR(sec::exc::rs_salt_size,sec::exc::cm_crypt_pkt,false); }
      if( key.size() == 0 )     { THR(sec::exc::rs_null_key,sec::exc::cm_crypt_pkt,false); }
      if( data.size() > 65200 ) { THR(sec::exc::rs_too_big,sec::exc::cm_crypt_pkt,false); }

      umac_ae_ctx_t * ctx = (umac_ae_ctx_t *)::malloc(sizeof(umac_ae_ctx_t));

      if( !ctx ) return false;

      tbuf<saltbuf_t::preallocated_size+56>  salt0;
      tbuf<keybuf_t::preallocated_size+48>   key0;
      tbuf<65536>                            data0;
      tbuf<65536>                            res0;

      /* aligned buffers */
      char * salt2 = (char *)salt0.allocate(16+32+8+8); // alignment+padding+salt+MAC
      char * key2  = (char *)key.private_data();
      char * data2 = (char *)data0.allocate(data.size()+16+32+4); // alignment+padding+random4
      char * res2  = (char *)res0.allocate(data.size()+16+32+4);  // alignment+padding+random4

      /* align salt if needed */
      if( (unsigned long long)salt2 & ~(15ULL) )
      {
        salt2 += (16-(((unsigned long long)salt2)&15));
      }
      memcpy( salt2,salt.data(),salt.size() );

      /* align key if needed */
      if( (unsigned long long)key2 & ~(15ULL) )
      {
        char * key1  = (char *)key0.allocate(key.size()+48);
        key2  = key1  + (16-(((unsigned long long)key1)&15ULL));
        memcpy( key2,key.data(),key.size() );
      }

      /* align data if needed */
      if( (unsigned long long)data2 & ~(15ULL) )
      {
        data2 += (16-(((unsigned long long)data2)&15ULL));
      }

      if( RAND_bytes((unsigned char *)data2,4) != 1 )
      {
        /* fallback */
        if( RAND_pseudo_bytes((unsigned char *)data2,4) != 1 )
        {
          THR(sec::exc::rs_rand_failed,sec::exc::cm_crypt_pkt,false);
        }
      }
      memcpy( data2+4,data.data(),data.size() );

      /* align output data if needed */
      if( (unsigned long long)res2 & ~(15ULL) )
      {
        res2  += (16-(((unsigned long long)res2)&15));
      }

      /* copy input data */
      header.set((const unsigned char *)salt2,8);

      /* encrypt and calculate mac */
      umac_ae_set_key(key2, ctx);
      umac_ae_header(salt2, 8, ctx);
      umac_ae_encrypt(data2, res2, data.size()+4, salt2, ctx);
      umac_ae_finalize(salt2, ctx);
      umac_ae_done(ctx);

      /* copy out data */
      data.set((const unsigned char *)res2,data.size()+4);
      footer.set((const unsigned char *)salt2,8);

      free( ctx );
      return true;
    }

    bool crypt_pkt::decrypt( const keybuf_t & key,
                             const headbuf_t & header,
                             databuf_t & data,
                             const footbuf_t & footer )
    {
      if( header.size() != 8 )  { THR(sec::exc::rs_header_size,sec::exc::cm_crypt_pkt,false); }
      if( footer.size() != 8 )  { THR(sec::exc::rs_footer_size,sec::exc::cm_crypt_pkt,false); }
      if( key.size() == 0 )     { THR(sec::exc::rs_null_key,sec::exc::cm_crypt_pkt,false); }
      if( data.size() > 65200 ) { THR(sec::exc::rs_too_big,sec::exc::cm_crypt_pkt,false); }
      if( data.size() < 4 )     { THR(sec::exc::rs_null_data,sec::exc::cm_crypt_pkt,false); }

      umac_ae_ctx_t * ctx = (umac_ae_ctx_t *)::malloc(sizeof(umac_ae_ctx_t));

      if( !ctx ) return false;

      tbuf<saltbuf_t::preallocated_size+56>  salt0;
      tbuf<keybuf_t::preallocated_size+48>   key0;
      tbuf<65536>                            data0;
      tbuf<65536>                            res0;

      /* aligned buffers */
      char * salt2 = (char *)salt0.allocate(16+32+8+8); // alignment+padding+salt+MAC
      char * key2  = (char *)key.private_data();
      char * data2 = (char *)data.private_data();
      char * res2  = (char *)res0.allocate(data.size()+16+32); // alignment+padding

      /* align salt if needed */
      if( (unsigned long long)salt2 & ~(15ULL) )
      {
        salt2 += (16-(((unsigned long long)salt2)&15ULL));
      }
      memcpy( salt2,header.data(),8 );

      /* align key if needed */
      if( (unsigned long long)key2 & ~(15ULL) )
      {
        char * key1  = (char *)key0.allocate(key.size()+48);
        key2  = key1  + (16-(((unsigned long long)key1)&15ULL));
        memcpy( key2,key.data(),key.size() );
      }

      /* align data if needed */
      if( (unsigned long long)data2 & ~(15ULL) )
      {
        char * data1 = (char *)data0.allocate(data.size()+48);
        data2 = data1 + (16-(((unsigned long long)data1)&15ULL));
        memcpy( data2,data.data(),data.size() );
      }

      /* align output data if needed */
      if( (unsigned long long)res2 & ~(15ULL) )
      {
        res2  += (16-(((unsigned long long)res2)&15ULL));
      }

      /* decrypt and calculate mac */
      umac_ae_set_key(key2, ctx);
      umac_ae_header(salt2, 8, ctx);
      umac_ae_decrypt(data2, res2, data.size(), salt2, ctx);
      umac_ae_finalize(salt2, ctx);
      umac_ae_done(ctx);

      /* copy out data */
      bool ret = false;

      if( memcmp(footer.data(),salt2,8) == 0 )
      {
        if( data.size() > 4 ) { data.set((const unsigned char *)res2+4,data.size()-4); }
        else                  { data.reset(); }

        ret = true;
        free( ctx );
      }
      else
      {
        free( ctx );
        THR(sec::exc::rs_cksum,sec::exc::cm_crypt_pkt,false);
      }
      return ret;
    }
  };
};

/* EOF */
