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

#ifndef _csl_common_xdrbuf_hh_included_
#define _csl_common_xdrbuf_hh_included_

/**
   @file xdrbuf.hh
   @brief XDR helper to reduce XDR dependecies
*/

#include "common.h"
#include "pbuf.hh"
#ifdef __cplusplus
#include <utility>

namespace csl
{
  namespace common
  {
    class str;
    class ustr;
    class var;
    class exc;

    /**
    @brief xdrbuf de/serializes data to a pbuf in XDR format

    the purpose of this class is to support the needs of csl::sec in order
    to free it from OpenAFS Rx and XDR dependencies

    later this may further improve to a more complete XDR serialization implementation
    but now it is more a limited one
    */
    class xdrbuf
    {
      public:
        /** @brief construct a pbuf dependent xdrbuf object */
        inline xdrbuf(pbuf & b)
        : use_exc_(true), b_(&b), it_(b.begin()), pos_(0) { }

        /** @brief copy constructor */
        inline xdrbuf(const xdrbuf & x)
        : use_exc_(x.use_exc_), b_(x.b_), it_(x.it_), pos_(x.pos_) { }

        /** @brief destructor */
        ~xdrbuf() {}

        /** @brief copy operator */
        inline xdrbuf & operator=(const xdrbuf & x)
        {
          b_       = x.b_;
          it_      = x.it_;
          pos_     = x.pos_;
          use_exc_ = x.use_exc_;
          return *this;
        }

        /** @brief helper to pass a (ptr,size) pair to operators */
        typedef std::pair<const unsigned char *,unsigned int> bindata_t;


        /**
        @brief serialize val to pbuf
        @param val is the value to be serialized
        @return reference to xdrbuf
        @throw common::exc

        puts a 32 bit integer to pbuf
        */
        xdrbuf & operator<<(int32_t val);

        /**
        @brief serialize val to pbuf
        @param val is the value to be serialized
        @return reference to xdrbuf
        @throw common::exc

        puts a 32 bit integer to stream
        */
        xdrbuf & operator<<(uint32_t val);

        /**
        @brief serialize val to pbuf
        @param val is the value to be serialized
        @return reference to xdrbuf
        @throw common::exc

        @li puts a 32 bit integer to stream as size
        @li puts size bytes of val C string to pbuf
        @li puts optional padding 1-3 bytes
        */
        xdrbuf & operator<<(const char * val);

        /**
        @brief serialize val to variable
        @param val is the value to be serialized
        @return reference to xdrbuf
        @throw common::exc

        calls the matching xdr function based on
        variable's implementation
        */
        xdrbuf & operator<<(const common::var & val);

        /**
        @brief serialize val to pbuf
        @param val is the value to be serialized
        @return reference to xdrbuf
        @throw common::exc

        @li puts a 32 bit integer to stream as size
        @li puts size bytes of val string to pbuf
        @li puts optional padding 1-3 bytes
        */
        xdrbuf & operator<<(const common::str & val);

        /**
        @brief serialize val to pbuf
        @param val is the value to be serialized
        @return reference to xdrbuf
        @throw common::exc

        @li puts a 32 bit integer to stream as size
        @li puts size bytes of val string to pbuf
        @li puts optional padding 1-3 bytes
         */
        xdrbuf & operator<<(const common::ustr & val);

        /**
        @brief serialize val to pbuf
        @param val is the value to be serialized
        @return reference to xdrbuf
        @throw common::exc

        @li puts a 32 bit integer to stream as size (val.second)
        @li puts (val.second) bytes from val to pbuf (val.first)
        @li puts optional padding 1-3 bytes
        */
        xdrbuf & operator<<(const bindata_t & val);

        /**
        @brief serialize val to pbuf
        @param val is the value to be serialized
        @return reference to xdrbuf
        @throw common::exc

        @li puts a 32 bit integer to stream as size (val.size())
        @li puts (val.size()) bytes from val to pbuf
        @li puts optional padding 1-3 bytes
        */
        xdrbuf & operator<<(const pbuf & val);

        /**
        @brief deserialize val from pbuf
        @param val is the value to be deserialized
        @return reference to xdrbuf
        @throw common::exc

        @li reads a 32 bit integer from stream to val
        */
        xdrbuf & operator>>(int32_t & val);

        /**
        @brief deserialize val from pbuf
        @param val is the value to be deserialized
        @return reference to xdrbuf
        @throw common::exc

        @li reads a 32 bit integer from stream to val
        */
        xdrbuf & operator>>(uint32_t & val);

        /**
        @brief deserialize val from variable
        @param val is the value to be deserialized
        @return reference to xdrbuf
        @throw common::exc
         */
        xdrbuf & operator>>(common::var & val);

        /**
        @brief deserialize val from pbuf
        @param val is the value to be deserialized
        @return reference to xdrbuf
        @throw common::exc

        @li reads a 32 bit integer from stream as size
        @li reads size bytes from pbuf to val
        @li align internal pointer with 1-3 optional padding bytes
         */
        xdrbuf & operator>>(common::str & val);

        /**
        @brief deserialize val from pbuf
        @param val is the value to be deserialized
        @return reference to xdrbuf
        @throw common::exc

        @li reads a 32 bit integer from stream as size
        @li reads size bytes from pbuf to val
        @li align internal pointer with 1-3 optional padding bytes
         */
        xdrbuf & operator>>(common::ustr & val);

        /**
        @brief deserialize val from pbuf
        @param val is the value to be deserialized
        @return reference to xdrbuf
        @throw common::exc

        @li reads a 32 bit integer from stream as size
        @li reads size bytes from pbuf to val
        @li align internal pointer with 1-3 optional padding bytes
        */
        xdrbuf & operator>>(pbuf & val);

        /**
        @brief deserialize data to 'where'
        @param where is a location where to put data
        @param size is the expected length to be read
        @return the successful data size read
        @throw common::exc

        @note this function reads from the stream 'asis' without decoding any
        size information. to decode that with the size use the 3 parameter form of get_data

        @li where is expected to be allocated by the caller
        @li then reads size bytes from the stream (asis)
        @li align internal pointer with 1-3 optional padding bytes
        */
        unsigned int get_data( unsigned char * where,
                               unsigned int size );

        /**
        @brief deserialize data to 'where'
        @param where is a location where to put data
        @param size is a reference to the read size
        @param max_size is the maximum size to be read
        @return true if successful
        @throw common::exc

        @li where is expected to be allocated by the caller
        @li first reads 32 bit integer from stream as sizei
        @li then reads min(max_size,sizei) bytes from stream
        @li align internal pointer with 1-3 optional padding bytes
        @li puts the read size to 'size'
        */
        bool get_data( unsigned char * where,
                       unsigned int & size,
                       unsigned int max_size );

        /**
        @brief deserialize data to 'where'
        @param where is a location where to put data
        @param size is a reference to the read size
        @param max_size is the maximum size to be read
        @return true if successful
        @throw common::exc

        @li where is expected to be allocated by the caller
        @li first reads 32 bit integer from stream as sizei
        @li then reads min(max_size,sizei) bytes from stream
        @li align internal pointer with 1-3 optional padding bytes
        @li puts the read size to 'size'
        */
        template <typename T>
        bool get_data( T & t,
                       unsigned int & size,
                       unsigned int max_size )
        {
          pbuf::iterator oldit = it_;
          unsigned int oldpos  = pos_;
          uint32_t sz;
          (*this) >> sz;
          it_  = oldit;
          pos_ = oldpos;
          if( sz <= 0 || sz > max_size ) return false;
          return get_data( reinterpret_cast<unsigned char *>(t.allocate(sz)), size, max_size );
        }

        /** @brief steps forward in the stream by n bytes (plus padding) */
        bool forward(unsigned int n);

        /** @brief Specifies whether param should throw common::exc exceptions
            @param yesno is the desired value to be set

            the default value for use_exc() is true, so it throws exceptions by default */
        inline void use_exc(bool yesno) { use_exc_ = yesno; }

        /** @brief Returns the current value of use_exc
            @return true if exc exceptions are used */
        inline bool use_exc() const { return use_exc_; }

        /** @brief compares two xdrbuf items (without comparing iterators) */
        inline bool operator==(const xdrbuf & other) const
        {
          return (use_exc_ == other.use_exc_ && b_ == other.b_);
        }

        /** @brief sets the iterator position to the beginning of the stream */
        void rewind();

        /** @brief returns the number of bytes consumed from the stream */
        unsigned long position();

      private:
        bool use_exc_;
        pbuf * b_;
        pbuf::iterator it_;
        unsigned int pos_;

        xdrbuf() : use_exc_(true), b_(0), it_(0,0), pos_(0) {}
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_common_xdrbuf_hh_included_ */
