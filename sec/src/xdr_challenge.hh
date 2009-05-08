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

#ifndef _csl_sec_xdr_challenge_hh_included_
#define _csl_sec_xdr_challenge_hh_included_

/**
  @file xdr_challenge.hh
  @brief XDR challenge packet helper
 */

#include "zfile.hh"
#include "xdrbuf.hh"
#include "ecdh_key.hh"
#ifdef __cplusplus

namespace csl
{
  namespace sec
  {
    using csl::common::xdrbuf;
    using csl::common::zfile;

    /**
    @brief container class that contains the neccessary information for a challenge-response protocol

    this class contains the challenge part and the response part is in xdr_response. this class serializes
    at most 3 ecdh_keys from which the peer may choose which to use. and an optional certificate data that identifies
    the challenger. the certificate itself will be compressed by zfile to save packet space.

    the whole packet is serialized into an XDR structure. this class has support for both serializing and deserializing
    the data.

    please note that checking the validity of the certificate is left to the user
    */
    class xdr_challenge
    {
      public:
        enum {
          max_keys_offered = 3,    ///<at most this number of keys may be sent in the challenge packet
          max_challenge_len = 2048 ///<the maximum size of the packet
        };

        xdr_challenge();                                        ///<constructor
        ~xdr_challenge();                                       ///<destructor
        xdr_challenge(const xdr_challenge & other);             ///<copy constructor
        xdr_challenge & operator=(const xdr_challenge & other); ///<copy operator

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

        /* inline functions */
        inline zfile & cert()             { return cert_; }  ///<get the certificate
        inline const zfile & cert() const { return cert_; }  ///<get the certificate

        inline void cert(unsigned char * data, uint32_t len) ///<set the certificate
        {
          cert_.put_data(data,len); 
          cert_.get_zsize(); /* enforce compression */
        }

        /**
        @brief get the key at i
        @param i is the array index 0..n-1 of which key to be returned
        */
        inline const ecdh_key & operator[](uint32_t i) const { return keys_[(i >= max_keys_offered ? 0 : i)]; }

        /**
        @brief set the given key
        @param k is the key to be set
        @param i is the array index 0..n-1 of which key to be returned
        */
        inline bool set(const ecdh_key & k, uint32_t i)
        {
          if( i>=max_keys_offered ) return false;
          keys_[i] = k;
          return true;
        }

        /** @brief checks for equiality */
        inline bool operator==(const xdr_challenge & other) const
        {
          for( unsigned int i=0;i<max_keys_offered;++i )
          {
            if( !(keys_[i] == other.keys_[i]) ) return false;
          }
          if( !(cert_ == other.cert_) ) return false;
          else                          return true;
        }

      private:
        ecdh_key  keys_[max_keys_offered];
        zfile     cert_;
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_sec_xdr_challenge_hh_included_ */
