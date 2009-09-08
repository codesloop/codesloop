/*
Copyright (c) 2008,2009, CodeSLoop Team

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

#include "codesloop/sec/crypt_buf.hh"
#include "codesloop/sec/csl_sec.h"
#include <openssl/blowfish.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include "codesloop/common/str.hh"
#include "codesloop/common/common.h"

/**
  @file crypt_buf.cc
  @brief implementation of crypt_buf
*/

namespace csl
{
  namespace sec
  {
    /** @brief Private implementation of crypt_buf */
    struct crypt_buf::impl
    {
      /* variables */
      int             bf_num_;
      unsigned char   tail_[CSL_SEC_CRYPT_BUF_MAC_LEN];
      unsigned char   initvec_[SHA_DIGEST_LENGTH];
      BF_KEY          key_;

      /* initialization */
      impl()
        : bf_num_(0)
      {
        memset( tail_,0,sizeof(tail_) );
        memset( initvec_,0,sizeof(initvec_) );
        memset( &key_,0,sizeof(BF_KEY) );
      }

      /* internal */

      /* interface */
      size_t get_header_len() { return CSL_SEC_CRYPT_BUF_HEAD_LEN; }
      size_t get_mac_len()    { return CSL_SEC_CRYPT_BUF_MAC_LEN; }

      bool init_crypt( unsigned char * buf,
                       const char * key,
                       bool encrypt,
                       const unsigned char * rndata )
      {
        if( !key ) return false;
        return init_crypt( buf, reinterpret_cast<const unsigned char *>(key), strlen(key), encrypt, rndata );
      }

      bool init_crypt( unsigned char * buf,
                       const unsigned char * key,
                       size_t keylen,
                       bool encrypt,
                       const unsigned char * rndata )
      {
        if( !key || !buf ) { return false; } /* invalid values */

        size_t l = (keylen/4)*4;

        if( l<12 ) { return false; } /* at least 96 bits needed */
        if( l>56 ) { return false; } /* max 448 bits can be used */

        BF_set_key(&key_,l,key);
        SHA1(key, l, initvec_ );

        if( encrypt )
        {
          /* init random value */
          if( rndata == 0 )
          {
            if( RAND_bytes(buf,get_header_len()) != 1 )
            {
              if( RAND_pseudo_bytes(buf,get_header_len()) != 1 )
              {
                return false;
              }
            }
          }
          else
          {
            ::memcpy(buf,rndata,get_header_len());
          }

          SHA1(buf, get_header_len(), tail_);
          BF_cfb64_encrypt(buf,buf,get_header_len(),&key_,initvec_,&bf_num_,BF_ENCRYPT);
        }
        else
        {
          BF_cfb64_encrypt(buf,buf,get_header_len(),&key_,initvec_,&bf_num_,BF_DECRYPT);
          SHA1(buf, get_header_len(), tail_);
        }

        return true;
      }

      bool add_data(unsigned char * buf, size_t len, bool encrypt)
      {
        BF_cfb64_encrypt(buf,buf,len,&key_,initvec_,&bf_num_,(encrypt==true ? BF_ENCRYPT : BF_DECRYPT));
        return true;
      }

      bool finalize(unsigned char * outbuff)
      {
        memcpy(outbuff,tail_,get_mac_len());
        BF_cfb64_encrypt(outbuff,outbuff,get_mac_len(),&key_,initvec_,&bf_num_,BF_ENCRYPT);
        return true;
      }

    };

    /* public interface */
    size_t crypt_buf::get_header_len() { return impl_->get_header_len(); }
    size_t crypt_buf::get_mac_len()    { return impl_->get_mac_len();    }

    bool
    crypt_buf::init_crypt( unsigned char * buf,
                           const char * key,
                           bool encrypt,
                           const unsigned char * rndata )
    {
      return impl_->init_crypt( buf, key, encrypt, rndata );
    }

    bool
    crypt_buf::init_crypt( unsigned char * buf,
                           const unsigned char * key,
                           size_t keylen,
                           bool encrypt,
                           const unsigned char * rndata )
    {
      return impl_->init_crypt( buf, key, keylen, encrypt, rndata );
    }

    bool
    crypt_buf::add_data(unsigned char * buf, size_t len, bool encrypt)
    {
      return impl_->add_data( buf, len, encrypt );
    }

    bool
    crypt_buf::finalize( unsigned char * outbuff )
    {
      return impl_->finalize( outbuff );
    }

    crypt_buf::crypt_buf()
      : impl_(new impl) {}

    crypt_buf::~crypt_buf() {}

    /* private functions, copying not allowed */
    crypt_buf::crypt_buf(const crypt_buf & other)
      : impl_( reinterpret_cast<impl *>(0) ) {throw common::str("should never be called"); }

    crypt_buf &
    crypt_buf::operator=(const crypt_buf & other)
    {
      return *this;
    }
  };
};

/* EOF */
