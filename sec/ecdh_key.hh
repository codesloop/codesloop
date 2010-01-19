/*
Copyright (c) 2008,2009,2010, CodeSLoop Team

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

#ifndef _csl_sec_ecdh_key_hh_included_
#define _csl_sec_ecdh_key_hh_included_

/**
  @file ecdh_key.hh
  @brief ECDH key helper w/ XDR features
 */

#include "codesloop/sec/bignum.hh"
#include "codesloop/common/xdrbuf.hh"
#include "codesloop/common/ustr.hh"
#ifdef __cplusplus

namespace csl
{
  namespace sec
  {
    using csl::common::xdrbuf;
    using csl::common::pbuf;

    /**
    @brief basic EDCH key handling

    this class is able to generate an ECDH keypair and also to generate
    a shared key

    the class itself stores a public (x,y) coordinate pair together with the algorithm name
    this (x,y,algname) triple represents the public key, so this class only stores the public key

    the private key is stored in an external bignum

    the cryptography part is based on OpenSSL ECDH routines
    */
    class ecdh_key
    {
      public:
        ecdh_key();                                    ///<constructor
        ~ecdh_key();                                   ///<destructor
        ecdh_key(const ecdh_key & other);              ///<copy constructor
        ecdh_key & operator=(const ecdh_key & other);  ///<copy operator

        /**
        @brief serialize the instance to the given XDR buf
        @param buf is the XDR buffer that will contain the data
        @return true if successful
        */
        bool to_xdr(xdrbuf & buf) const;

        /**
        @brief deserialize the instance from the given XDR buf
        @param buf is the XDR buffer that contains the data
        @return true if successful
        */
        bool from_xdr(xdrbuf & buf);

        /**
        @brief sets the data
        @param x is the x coordinate of the public key
        @param y is the y coordinate of the public key
        @param algname is the algorithm used to generate the coordinates

        @note that this class only stores the public key as coordinates. the private
        key is expected to be in an external bignum instance
        */
        void set(const common::ustr & algname, const bignum & x, const bignum & y);

        /** @brief returns the algorithm strength in bits */
        static unsigned int algorithm_strength(const common::ustr & algname);

        /** @brief returns the algorithm strength in bits */
        unsigned int strength() const;

        /**
        @brief generates an ECDH keypair based on the algname already set
        @param private_key is the bignum where the private key will be stored
        @return true if successful

        the public key part is stored internally as the (x,y,algname) triple, where
        x and y are the public key represented by coordinates and algname is the
        algoritm name used to generate the keys
        */
        bool gen_keypair(bignum & private_key);

        /**
        @brief generates the shared key based on the internal public key and the given private_key
        @param peer_private_key is the private key
        @param shared_key is the string where the SHA1 hex digest of the shared key will be stored

        the philosophy of this function is this: the peer who wants to generate a shared key
        must have its private key before. than it receives the public key of the other peer, which will
        be stored in this instance. based on its own private key and the peers public key, it generates
        a shared key that may be used for encryption.
        */
        bool gen_sha1hex_shared_key(const bignum & peer_private_key, common::ustr & shared_key) const;

        /**
        @brief generates the shared key based on the internal public key and the given private_key
        @param peer_private_key is the private key
        @param shared_key is a binary buffer where the shared key will be put

        the philosophy of this function is this: the peer who wants to generate a shared key
        must have its private key before. than it receives the public key of the other peer, which will
        be stored in this instance. based on its own private key and the peers public key, it generates
        a shared key that may be used for encryption.
         */
        bool gen_shared_key(const bignum & peer_private_key, pbuf & shared_key) const;


        /** @brief print debug info */
        void print() const;

        /* inline functions */
        inline void x(const bignum & v)              { x_ = v; }        ///<sets x
        inline void y(const bignum & v)              { y_ = v; }        ///<sets y
        inline void algname(const common::ustr & v)  { algname_ = v; }  ///<sets algname
        inline void algname(const char * v)          { algname_ = v; }  ///<sets algname

        inline bignum & x()              { return x_; }         ///<gets x
        inline bignum & y()              { return y_; }         ///<gets y
        inline common::ustr & algname()  { return algname_; }   ///<gets algname

        inline const bignum & x() const              { return x_; }       ///<gets x
        inline const bignum & y() const              { return y_; }       ///<gets y
        inline const common::ustr & algname() const  { return algname_; } ///<gets algname

        /** @brief chacks if properly filled with data */
        inline bool has_data() const { return (x_.is_empty()==false && y_.is_empty()==false && algname_.size()>0); }

        inline bool is_empty() const { return (has_data()==false); } ///<checks if empty

        /** @brief compares for equality */
        inline bool operator==(const ecdh_key & other) const
        {
          if( !(algname_ == other.algname_) ) return false;
          if( !(x_ == other.x_) )             return false;
          if( !(y_ == other.y_) )             return false;
          return true;
        }

      private:
        common::ustr   algname_;   ///<the algorithm name used to generate the key
        bignum         x_;         ///<x coordinate part of the public key
        bignum         y_;         ///<y coordinate part of the public key
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_sec_ecdh_key_hh_included_ */
