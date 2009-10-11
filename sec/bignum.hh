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

#ifndef _csl_sec_bignum_hh_included_
#define _csl_sec_bignum_hh_included_

/**
  @file bignum.hh
  @brief big number helper w/ XDR features
 */

#include "codesloop/common/tbuf.hh"
#include "codesloop/common/xdrbuf.hh"
#ifdef __cplusplus

namespace csl
{
  namespace sec
  {
    using csl::common::xdrbuf;

    /**
    @brief this class represents a bignumber that is closely modelled after OpenSSL BIGNUMs

    this is a support class that helps serializing the OpenSSL BIGNUM data into XDR streams

    the design goal of the class is to be fast for the common case, which is storing ECDH keys.
    this should fit in the statically allocated buffer (preallocated_). if more space is needed,
    this class will allocate the needed space on the heap.

    the bignumber is a triple of (data,size,is_negative) information. that is the data is a pointer
    to the allocated space, the size is the number of bytes used, and the is_negative flag shows
    wether the bignmuber is negatie or not.
    */
    class bignum
    {
      public:
        typedef common::tbuf<128> buf_t;

        inline bignum() : is_negative_(false) {}               ///<constructor
        inline ~bignum() { reset(); }                          ///<destructor
        inline bignum(const bignum & other) { *this = other; } ///<copy constructor
        inline bignum & operator=(const bignum & other)        ///copy operator
        {
          buf_         = other.buf_;
          is_negative_ = other.is_negative_;
          return *this;
        }

        /** @brief clear internal data */
        inline void reset()
        {
          buf_.reset();
          is_negative_ = false;
        }

        /**
        @brief allocate sz bytes for storing the data
        @param sz is the number of bytes to be allocated
        @return a pointer to the allocated space

        if sz fits in the preallocated space, it will return that
        */
        inline unsigned char * allocate(uint64_t sz)
        {
          return buf_.allocate(sz);
        }

        /** @brief set the bignumber data */
        inline bool set(const unsigned char * dta, uint64_t sz)
        {
          return buf_.set(dta,sz);
        }

        /** @brief get the bignumber data */
        inline bool get(unsigned char * dta)
        {
          return buf_.get(dta);
        }

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

        void print() const;                                 ///<print debug data

        bool operator==(const bignum & other) const  ///<checks for equality
        {
          if( is_negative_ != other.is_negative_ ) return false;
          return (buf_ == other.buf_);
        }

        /* inline functions */
        inline bool is_empty() const        { return buf_.is_empty(); }   ///<checks if empty
        inline bool has_data() const        { return buf_.has_data(); }   ///<checks if has_data
        inline bool is_static() const       { return buf_.is_static(); }  ///<checks if statically allocated
        inline uint64_t size() const        { return buf_.size(); }       ///<returns the size of the allocated data
        inline void is_negative(bool yesno) { is_negative_ = yesno; }     ///<sets the negative flag
        inline bool is_negative() const     { return is_negative_; }      ///<gets the negative flag

        inline const unsigned char * data() const { return buf_.data(); } ///<return the allocated data

      private:
        buf_t           buf_;
        bool            is_negative_;        ///<set to true if negative
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_sec_bignum_hh_included_ */
