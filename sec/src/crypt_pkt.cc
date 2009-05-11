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
#include "csl_sec.hh"
#include "tbuf.hh"
#include "umac_ae.h"
#include <string.h>

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
      if( salt.size() != 8 )    return false;
      if( key.size() == 0 )     return false;
      if( data.size() > 65400 ) return false;

      umac_ae_ctx_t * ctx = (umac_ae_ctx_t *)::malloc(sizeof(umac_ae_ctx_t));

      if( !ctx ) return false;

      tbuf<saltbuf_t::preallocated_size+48>  salt0;
      tbuf<keybuf_t::preallocated_size+48>   key0;
      tbuf<headbuf_t::preallocated_size+48>  head0;
      tbuf<databuf_t::preallocated_size+48>  data0;
      tbuf<databuf_t::preallocated_size+48>  res0;
      tbuf<footbuf_t::preallocated_size+48>  foot0;

      /* aligned buffers */
      char * salt2 = (char *)salt.private_data();
      char * key2  = (char *)key.private_data();
      char * data2 = (char *)data.private_data();

      char * head2 = (char *)head0.allocate(8);
      char * res2  = (char *)res0.allocate(data.size()+48);
      char * foot2 = (char *)foot0.allocate(16);

      /* align salt if needed */
      if( (unsigned long long)salt2 & ~(15ULL) )
      {
        char * salt1 = (char *)salt0.allocate(salt.size()+48);
        salt2 = salt1 + (16-(((unsigned long long)salt1)&15));
        memcpy( salt2,salt.data(),salt.size() );
      }

      /* align key if needed */
      if( (unsigned long long)key2 & ~(15ULL) )
      {
        char * key1  = (char *)key0.allocate(key.size()+48);
        key2  = key1  + (16-(((unsigned long long)key1)&15));
        memcpy( key2,key.data(),key.size() );
      }

      /* align data if needed */
      if( (unsigned long long)data2 & ~(15ULL) )
      {
        char * data1 = (char *)data0.allocate(data.size()+48);
        data2 = data1 + (16-(((unsigned long long)data1)&15));
        memcpy( data2,data.data(),data.size() );
      }

      /* align head if needed */
      if( (unsigned long long)head2 & ~(15ULL) )
      {
        char * head1 = (char *)head0.allocate(header.size()+48);
        head2 = head1 + (16-(((unsigned long long)head1)&15));
      }

      /* align foot if needed */
      if( (unsigned long long)foot2 & ~(15ULL) )
      {
        char * foot1 = (char *)foot0.allocate(footer.size()+48);
        foot2 = foot1 + (16-(((unsigned long long)foot1)&15));
      }

      /* align output data if needed */
      if( (unsigned long long)res2 & ~(15ULL) )
      {
        char * res1  = (char *)res0.allocate(data.size()+48);
        res2  = res1  + (16-(((unsigned long long)res1)&15));
      }

      /* copy input data */
      memcpy( head2,salt.data(),salt.size() );
      memcpy( foot2,salt.data(),salt.size() );

      /* encrypt and calculate mac */
      umac_ae_set_key(key2, ctx);
      umac_ae_encrypt(data2, res2, data.size(), salt2, ctx);
      umac_ae_finalize(foot2+8, ctx);
      umac_ae_done(ctx);

      /* copy out data */
      header.set((const unsigned char *)head2,8);
      data.set((const unsigned char *)res2,data.size());
      footer.set((const unsigned char *)foot2,16);

      free( ctx );
      return true;
    }

    bool crypt_pkt::decrypt( const saltbuf_t & salt,
                             const keybuf_t & key,
                             const headbuf_t & header,
                             databuf_t & data,
                             const footbuf_t & footer )
    {
      if( salt.size() != 8 )    return false;
      if( key.size() == 0 )     return false;
      if( data.size() > 65400 ) return false;

      umac_ae_ctx_t * ctx = (umac_ae_ctx_t *)::malloc(sizeof(umac_ae_ctx_t));

      if( !ctx ) return false;

      tbuf<saltbuf_t::preallocated_size+48>  salt0;
      tbuf<keybuf_t::preallocated_size+48>   key0;
      tbuf<headbuf_t::preallocated_size+48>  head0;
      tbuf<databuf_t::preallocated_size+48>  data0;
      tbuf<databuf_t::preallocated_size+48>  res0;
      tbuf<footbuf_t::preallocated_size+48>  foot0;

      /* aligned buffers */
      char * salt2 = (char *)salt.private_data();
      char * key2  = (char *)key.private_data();
      char * data2 = (char *)data.private_data();
      char * head2 = (char *)head0.private_data();
      char * foot2 = (char *)foot0.private_data();

      char * res2  = (char *)res0.allocate(data.size()+48);

      /* align salt if needed */
      if( (unsigned long long)salt2 & ~(15ULL) )
      {
        char * salt1 = (char *)salt0.allocate(salt.size()+48);
        salt2 = salt1 + (16-(((unsigned long long)salt1)&15));
        memcpy( salt2,salt.data(),salt.size() );
      }

      /* align key if needed */
      if( (unsigned long long)key2 & ~(15ULL) )
      {
        char * key1  = (char *)key0.allocate(key.size()+48);
        key2  = key1  + (16-(((unsigned long long)key1)&15));
        memcpy( key2,key.data(),key.size() );
      }

      /* align data if needed */
      if( (unsigned long long)data2 & ~(15ULL) )
      {
        char * data1 = (char *)data0.allocate(data.size()+48);
        data2 = data1 + (16-(((unsigned long long)data1)&15));
        memcpy( data2,data.data(),data.size() );
      }

      /* align head if needed */
      if( (unsigned long long)head2 & ~(15ULL) )
      {
        char * head1 = (char *)head0.allocate(header.size()+48);
        head2 = head1 + (16-(((unsigned long long)head1)&15));
        memcpy( head2,salt.data(),salt.size() );
      }

      /* align foot if needed */
      if( (unsigned long long)foot2 & ~(15ULL) )
      {
        char * foot1 = (char *)foot0.allocate(footer.size()+48);
        foot2 = foot1 + (16-(((unsigned long long)foot1)&15));
        memcpy( foot2,salt.data(),salt.size() );
      }

      /* align output data if needed */
      if( (unsigned long long)res2 & ~(15ULL) )
      {
        char * res1  = (char *)res0.allocate(data.size()+48);
        res2  = res1  + (16-(((unsigned long long)res1)&15));
      }

      /* encrypt and calculate mac */
      umac_ae_set_key(key2, ctx);
      umac_ae_decrypt(data2, res2, data.size(), salt2, ctx);
      umac_ae_finalize(foot2+8, ctx);
      umac_ae_done(ctx);

      /* copy out data */
      // header.set((const unsigned char *)head2,8);
      data.set((const unsigned char *)res2,data.size());
      // TODO footer.set((const unsigned char *)foot2,16);
      // TODO check mac

      free( ctx );
      return true;
    }
  };
};

/* EOF */
