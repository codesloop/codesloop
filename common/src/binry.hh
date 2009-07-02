/*
Copyright (c) 2008,2009, Tamas Foldi, David Beck

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
THEORY OF LIABILITY, WHETHER IN CONTRACT, objICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _csl_common_binry_hh_included_
#define _csl_common_binry_hh_included_

/**
   @file binry.hh
   @brief binary type to be used in interfaces
 */

#include "var.hh"
#include "dbl.hh"
#include "tbuf.hh"
#include "arch.hh"
#ifdef __cplusplus

namespace csl
{
  namespace common
  {
    class str;
    class ustr;
    class xdrbuf;

    /**
    @brief Binary buffer object that is aligned with the other children of common::var

    the binary buffer is represented by tbuf, which is essentially a pair of pointer plus a size.
    the pointer may point to a configurable amount of preallocated memory region. buf_size tells
    how many bytes were preallocated. this preallocation is used for performance reason.
    */
    class binry : public csl::common::var
    {
      public:
        enum { buf_size = 1024, var_type_v = CSL_TYPE_BIN };
        typedef tbuf<buf_size> buf_t; ///<The internal buffer type that stores the data
        typedef const buf_t & value_t;

      private:
        buf_t value_; ///<The member variable that stores the data

      public:
        binry(); ///<default constructor

        /** @brief copy operator */
        inline binry & operator=(const binry & other)
        {
          value_ = other.value_;
          return *this;
        }
      
        /**
        @brief initializing constructor
        @param ptr is a memory location
        @param sz is the amount of memory to be copied in
        */
        binry(const unsigned char * ptr,size_t sz);

        virtual inline ~binry() {} ///<destructor
        inline value_t value() const { return value_; } ///<constant reference to the internal buffer
        inline int var_type() const { return var_type_v; } ///<value type helps distinguish from other var types
        inline void reset() { value_.reset(); } ///<reset the internal buffer
        
        /** @brief returns a const pointer to internal data */
        inline const unsigned char * ucharp_data() const { return value_.data(); }
        
        /** @brief returns the size of the variable data */
        inline size_t var_size() const { return value_.size(); }

        /* conversions to other types */
        /**
        @brief convert to common::int64
        @param v is where to put the data
        @return true if successful

        this function delegates the conversion to common::int64 class
         */
        inline bool to_integer(int64 & v) const { return v.from_binary(value_.data(),value_.size()); }

        /**
        @brief convert to long long
        @param v is where to put the data
        @return true if successful

        This function treats the internal data as binary representation of long long. This representation
        is then copied to v byte-by-byte. No real conversion is done.
         */
        bool to_integer(long long & v) const;

        /**
        @brief convert to common::dbl
        @param v is where to put the data
        @return true if successful

        this function delegates the conversion to common::dbl class
         */
        inline bool to_double(dbl & v) const { return v.from_binary(value_.data(),value_.size()); }

        /**
        @brief convert to double
        @param v is where to put the data
        @return true if successful

        This function treats the internal data as binary representation of double. This representation
        is then copied to v byte-by-byte. No real conversion is done.
         */
        bool to_double(double & v) const;

        /**
        @brief convert to common::str
        @param v is where to put the data
        @return true if successful

        this function delegates the conversion to common::str class
         */
        bool to_string(str & v) const;

        /**
        @brief convert to common::ustr
        @param v is where to put the data
        @return true if successful

        this function delegates the conversion to common::ustr class
         */
        bool to_string(ustr & v) const;

        /**
        @brief convert to std::string
        @param v is where to put the data
        @return true if successful

        This function treats the internal data as a (char) string. This representation
        is then copied to v byte-by-byte. This function checks wether the string is terminated
        by 0. If so, that character is not copied.
         */
        bool to_string(std::string & v) const;

        /**
        @brief convert to common::binry
        @param v is where to put the data
        @return true if successful

        this function delegates the conversion to the other common::binry instance (this class)
         */
        inline bool to_binary(binry & v) const { return v.from_binary(value_.data(),value_.size()); }

        /**
        @brief convert to ptr,size
        @param v points to a memory region where to store the data
        @param sz will contain the number of bytes copied
        @return true if successful

        this function assumes that v has enough space. sz will be updated to indicate, how
        many bytes were stored
         */
        bool to_binary(unsigned char * v, size_t & sz) const;

        /**
        @brief convert to ptr,size
        @param v points to a memory region where to store the data
        @param sz will contain the number of bytes copied
        @return true if successful

        this function assumes that v has enough space. sz will be updated to indicate, how
        many bytes were stored
         */
        bool to_binary(void * v, size_t & sz) const;

        /**
        @brief stores the content of this instance to an XDR stream
        @param b is the XDR stream to be used
        @return true if successful

        note that usual XDR conversion will apply as any buffer to be stored in an XDR stream.
        that is a 32bit integer is stored first that tells how many bytes will follow.
        then the buffer content is stored. and finally 1-3 optional padding bytes may follow
        depending on the buffer size. XDR values are always stored in a multiple of 4 chunks.
        the first 32bit integer value will not include the padding.
         */
        bool to_xdr(xdrbuf & b) const;

        /**
        @brief polymorphic call to store the data into common::var type
        @param v is where to place the data
        @return true if successful

        this function delegates the conversion to v
         */
        inline bool to_var(var & v) const { return v.from_binary(value_.data(),value_.size()); }

        /* conversions from other types */
        /**
        @brief convert an common::int64
        @param v is the value to be read in
        @return true if successful

        this function delegates the conversion task to the common::int64 class
         */
        inline bool from_integer(const int64 & v) { return v.to_binary(*this); }

        /**
        @brief convert a long long
        @param v is the value to be stored
        @return true if successful

        the result of this conversion is a bit-by-bit representation of v in the memory buffer.
        there is no real conversion to happen.
         */
        bool from_integer(long long v);

        /**
        @brief convert a common::dbl
        @param v is the value to be read in

        this function delegates the conversion task to the common::dbl class
         */
        inline bool from_double(const dbl & v)  { return v.to_binary(*this); }

        /**
        @brief convert a double
        @param v is the value to be stored
        @return true if successful

        the result of this conversion is a bit-by-bit representation of v in the memory buffer.
        there is no real conversion to happen.
         */
        bool from_double(double v);

        /**
        @brief convert a common::str
        @param v is the value to be read in
        @return true if successful

        this function delegates the conversion task to the common::str class
         */
        bool from_string(const str & v);

        /**
        @brief convert a common::ustr
        @param v is the value to be read in
        @return true if successful

        this function delegates the conversion task to the common::ustr class
         */
        bool from_string(const ustr & v);

        /**
        @brief convert a std::string
        @param v is the value to be read in
        @return true if successful

        this function stores the content of the string read by c_str() and
        it will store the trailing 0 byte too
         */
        bool from_string(const std::string & v);

        /**
        @brief convert a char*
        @param v is the value to be read in
        @return true if successful

        this function delegates the conversion task to the common::ustr class
         */
        bool from_string(const char * v);

        /**
        @brief convert a wchar_t*
        @param v is the value to be read in
        @return true if successful

        this function delegates the conversion task to the common::str class
         */
        bool from_string(const wchar_t * v);

        /**
        @brief convert a binry
        @param v is the value to be read in
        @return true if successful

        v.value_ will be stored in value_, thus make an equivalent copy of v
         */
        bool from_binary(const binry & v);

        /**
        @brief convert a memory region
        @param v is the pointer to the start of the memory region
        @param sz is the number of bytes to be read
        @return true if successful

        the given memory region will be copied in
         */
        bool from_binary(const unsigned char * v,size_t sz);

        /**
        @brief convert a memory region
        @param v is the pointer to the start of the memory region
        @param sz is the number of bytes to be read
        @return true if successful

        the given memory region will be copied in
         */
        bool from_binary(const void * v,size_t sz);

        /**
        @brief read the content of this instance from an XDR stream
        @param b is the XDR stream to be used
        @return true if successful

        note that usual XDR conversion will apply as any buffer to be stored in an XDR stream.
        that is a 32bit integer is stored first that tells how many bytes will follow.
        then the buffer content is stored. and finally 1-3 optional padding bytes may follow
        depending on the buffer size. XDR values are always stored in a multiple of 4 chunks.
        the first 32bit integer value will not include the padding.
         */
        bool from_xdr(xdrbuf & v);

        /**
        @brief polymorphic call to let v store its value in this buffer
        @param v is where to read the data from
        @return true if successful

        this function delegates the conversion to v
         */
        bool from_var(const var & v);

        /**
        @brief serialize contents of objects
        @param buf archiver class to/from serialize
        @throw common::exc
        */
        virtual inline void serialize(arch & buf) { buf.serialize(*this); }
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_common_binry_hh_included_ */
