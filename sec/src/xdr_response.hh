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

#ifndef _csl_sec_xdr_response_hh_included_
#define _csl_sec_xdr_response_hh_included_

/**
  @file xdr_response.hh
  @brief XDR response packet helper
 */

#include "ecdh_key.hh"
#include "pbuf.hh"
#include "zfile.hh"
#ifdef __cplusplus

namespace csl
{
  namespace sec
  {
    using csl::common::zfile;
    using csl::common::pbuf;

    /**
    @brief container class that contains the neccessary information for a challenge-response protocol

    this class contains the response part and the challenge part is in xdr_challenge. this class serializes
    the response ECDH key an optional certificate data that identifies the responder and an optional buffer
    that is encrypted by the calculated shared key. this buffer is expected to contain credential data: a login name 
    and a password.

    the whole packet is serialized into an XDR structure. this class has support for both serializing and deserializing
    the data.

    please note that the encryption of the credentials and checking the validity of the certificate is left to the user
     */
    class xdr_response
    {
      public:
        enum {
          max_response_len = 2048,  ///<max length of response data
          max_login_len = 64,       ///<max length of login string
          max_pass_len = 64,        ///<max length of password string
          max_creds_len = 128       ///<max length of the encrypted credential buffer
        };

        xdr_response();                                       ///<constructor
        ~xdr_response();                                      ///<destructor
        xdr_response(const xdr_response & other);             ///<copy constructor
        xdr_response & operator=(const xdr_response & other); ///<copy operator

        /**
        @brief serialize the instance to the given XDR buf
        @param buf is the XDR buffer that will contain the data
        @return true if successful
         */
        bool to_xdr(xdrbuf & buf);

        /**
        @brief deserialize the instance from the given XDR buf
        @param buf is the XDR buffer that contains the data
        @return true if successful
         */
        bool from_xdr(xdrbuf & buf);

        /* inline */
        inline ecdh_key & key()             { return key_; }  ///<get key
        inline const ecdh_key & key() const { return key_; }  ///<get key
        inline void key(const ecdh_key & k) { key_ = k; }     ///<set key

        inline pbuf & encrypted_creds()             { return encrypted_creds_; }  ///<get encrypted creds buffer
        inline const pbuf & encrypted_creds() const { return encrypted_creds_; }  ///<get encrypted creds buffer
        inline void encrypted_creds(const pbuf & b) { encrypted_creds_ = b; }     ///<set encrypted creds buffer

        inline zfile & cert()             { return cert_; }   ///<get certificate
        inline const zfile & cert() const { return cert_; }   ///<get certificate
        inline void cert(unsigned char * data, uint32_t len)  ///<set certificate
        {
          cert_.put_data(data,len);
        }

        inline bool operator==(const xdr_response & other) ///<checks for equality
        {
          if( !(key_ == other.key_) ) return false;
          if( !(cert_ == other.cert_) ) return false;
          if( encrypted_creds_.size() != other.encrypted_creds_.size() ) return false;
          return true;
        }

      private:
        ecdh_key     key_;               ///<response key
        pbuf         encrypted_creds_;   ///<encrypted credential buffer
        zfile        cert_;              ///<certificate data
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_sec_xdr_response_hh_included_ */
