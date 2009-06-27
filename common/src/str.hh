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
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Credits: some techniques and code pieces are stolen from Christian 
         Stigen Larsen http://csl.sublevel3.org/programming/my_str/
*/

#ifndef _csl_common_str_hh_included_
#define _csl_common_str_hh_included_

/**
   @file str.hh
   @brief simple but efficient wide string class
 */

#include "common.h"
#include "obj.hh"
#include "tbuf.hh"
#include "var.hh"
#include "binry.hh"
#ifdef __cplusplus
#include <string>

namespace csl
{
  namespace common
  {
    class ustr;

    /**
    @brief wide charcter string class

    this class is part of the common::var family and implements all neccesary abstract
    functions that are mainly related to conversions to and from other variable classes.

    strings are stored as a sequence of wchar_t characters. the class ensures that a trailing
    zero character is always present for traditional C-String compatibility.

    other functions, such as comparison and copy operators, copy constructors are present
     */
    class str : public csl::common::var
    {
      public:
        enum {
          buf_size = 128 * sizeof(wchar_t), ///<amount of preallocated memory in bytes
          npos = 0xFFFFFFFF,                 ///<constant for find: not found
          var_type_v = CSL_TYPE_STR           ///<variable type
        };
        
        typedef const wchar_t * value_t;
        inline value_t value() const { return c_str(); }

        inline virtual int var_type() const { return var_type_v; } ///<value type helps distinguish from other var types

        /** @brief constructor */
        inline str() : csl::common::var(), buf_(L'\0') { }

        /** @brief destructor */
        virtual ~str() {}

        /* ------------------------------------------------------------------------ *
        **    pbuf operations
        ** ------------------------------------------------------------------------ */

        /** @brief copy operator */
        inline str & operator=(const pbuf & other)
        {
          buf_ = other;
          ensure_trailing_zero();
          return *this;
        }

        /* ------------------------------------------------------------------------ *
        **    str operations
        ** ------------------------------------------------------------------------ */

        /** @brief copy constructor */
        inline str(const str& s) : csl::common::var(), buf_(L'\0')
        {
          buf_ = s.buf_;
        }

        /** @brief copy operator */
        inline str& operator=(const str& s)
        {
          buf_ = s.buf_;
          return *this;
        }

        /** @brief append operator */
        str& operator+=(const str&);

        /**
        @brief append operator with two parameters
        @param lhs is the left-hand-side of add
        @param rhs is the right-hand-side of add

        adds lhs+rhs and returns a fresh new result string
         */
        inline friend str operator+(const str& lhs, const str& rhs)
        {
          return str(lhs) += rhs;
        }

        /** @brief is equal operator */
        inline bool operator==(const str& s) const
        {
          int ret = wcscmp( data(), s.data() );
          return (ret == 0);
        }

        /**
        @brief find a substring of string
        @param s is the substring to be found
        @returns npos if not found or the position
         */
        size_t find(const str & s) const;

        /**
        @brief extracts a substring from a given position
        @param start start from this position
        @param length is the amount to be extracted
         */
        str substr(const size_t start, const size_t length) const;

        /* ------------------------------------------------------------------------ *
        **    ustr operations
        ** ------------------------------------------------------------------------ */

        /** @brief copy constructor */
        explicit str(const ustr & other);

        /** @brief copy operator */
        str & operator=(const ustr & other);

        /** @brief append operator */
        str& operator+=(const ustr&);

        /** @brief is equal operator */
        inline bool operator==(const ustr & s) const
        {
          str rhs(s);
          return (*this == rhs);
        }

        /* ------------------------------------------------------------------------ *
        **    char * operations
        ** ------------------------------------------------------------------------ */

        /** @brief copy constructor */
        explicit str(const char *);

        /** @brief copy operator */
        str& operator=(const char *);

        /** @brief is equal operator */
        inline bool operator==(const char * s) const
        {
          str rhs(s);
          return (*this == rhs);
        }

        /* ------------------------------------------------------------------------ *
        **    wchar_t * operations
        ** ------------------------------------------------------------------------ */

        /** @brief copy constructor */
        inline str(const wchar_t * wcs) : csl::common::var(), buf_(L'\0')
        {
          if( !wcs ) return;
          // wcslen only cares about trailing zero, so combining characters will not confuse here
          buf_.set( reinterpret_cast<const unsigned char *>(wcs),
                    sizeof(wchar_t) * (::wcslen(wcs)+1));
        }

        /** @brief copy operator */
        inline str& operator=(const wchar_t * wcs)
        {
          if( !wcs ) return *this;
          // wcslen only cares about trailing zero, so combining characters will not confuse here
          buf_.set( reinterpret_cast<const unsigned char *>(wcs),
                    sizeof(wchar_t) * (::wcslen(wcs)+1) );

          return *this;
        }

        /** @brief append operator */
        str& operator+=(const wchar_t * str);

        /**
        @brief append operator with two parameters
        @param lhs is the left-hand-side of add
        @param rhs is the right-hand-side of add

        adds lhs+rhs and returns a fresh new result string
         */
        inline friend str operator+(const wchar_t * lhs, const str& rhs)
        {
          return str(lhs) += rhs;
        }

        /**
        @brief append operator with two parameters
        @param lhs is the left-hand-side of add
        @param rhs is the right-hand-side of add

        adds lhs+rhs and returns a fresh new result string
         */
        inline friend str operator+(const str& lhs, const wchar_t * rhs)
        {
          return str(lhs) += rhs;
        }
        
        /** @brief is equal operator */
        inline bool operator==(const wchar_t * s) const
        {
          int ret = wcscmp( data(), s );
          return (ret == 0);
        }

        /** @brief returns the background c str */
        inline const wchar_t * c_str() const
        {
          return( data() ); // TODO: have a char * version
        }

        /**
        @brief assign string
        @param start is the start of the string
        @param lend is the end of the string
         */
        inline str & assign(const wchar_t * start, const wchar_t * end)
        {
          buf_.set( reinterpret_cast<const unsigned char *>(start),
                    reinterpret_cast<const char *>(end) - 
                    reinterpret_cast<const char *>(start) );
          return *this;
        }

        /**
        @brief find a wide character in the string
        @param s is the string to be found
        @returns npos if not found or the position
         */
        size_t find(const wchar_t * s) const;

        /** @brief get data as wchar_t */
        inline const wchar_t * data() const
        {
          return reinterpret_cast<const wchar_t *>(buf_.data());
        }

        /* ------------------------------------------------------------------------ *
        **    std::string operations
        ** ------------------------------------------------------------------------ */

        /** @brief let equal operator */
        str& operator=(const std::string & s)
        {
          return operator=(s.c_str());
        }

        /* ------------------------------------------------------------------------ *
        **    char operations
        ** ------------------------------------------------------------------------ */

        /* ------------------------------------------------------------------------ *
        **    wchar_t operations
        ** ------------------------------------------------------------------------ */

        /** @brief unchecked access to buffer */
        inline wchar_t operator[](const size_t n) const
        {
          return (data())[n];
        }

        /** @brief check access to buffer */
        wchar_t at(const size_t n) const;

        /**
        @brief find a wide character in the string
        @param w is the character to be found
        @returns npos if not found or the position
         */
        size_t find(wchar_t w) const;

        /**
        @brief reverse find a wide character in the string
        @param w is the character to be found
        @returns npos if not found or the position
         */
        size_t rfind(wchar_t w) const;

        /* ------------------------------------------------------------------------ */

        /** @brief resets str buffer */
        inline void clear()
        {
          reset();
        }

        /** @brief reset internal data */
        inline void reset()
        {
          buf_.reset();
          buf_.set( reinterpret_cast<const unsigned char *>(L"\0"), sizeof(wchar_t) );
        }

        /** @brief gets str size  */
        inline size_t size() const
        {
          /* I presume the trailing zero is already there */
          return (empty() ? 0 : (nbytes()/sizeof(wchar_t))-1 );
        }

        /** @brief gets the total allocated bytes */
        inline size_t nbytes() const
        {
          return buf_.size();
        }

        /** @brief return the number of characters in the string, excluding the trailing zero */
        inline size_t nchars() const
        {
          // wcstombs should take care of 'combining characters' too
          return empty() ? 0 : ::wcstombs(NULL, data(), 0);
        }

        /**
        @brief checks if string has data
        @return true if the string is empty
         */
        inline bool empty() const
        {
          return (buf_.size() <= 1);
        }

        /** @brief returns the internal buffer that stores the string */
        inline const tbuf<buf_size> & buffer() const
        {
          return buf_;
        }

        /** @brief returns the internal buffer that stores the string */
        inline tbuf<buf_size> & buffer()
        {
          return buf_;
        }

        /**
        @brief ensures that the string has a trailing zero

        this is mainly called internally. member functions use this, but users are free
        to call as well. member functions are expected to make sure that a trailing zero
        exists so it makes little sense for users, to call this function.
         */
        void ensure_trailing_zero();

        /** @brief returns a const pointer to internal data */
        inline const unsigned char * ucharp_data() const { return buf_.data(); }
        
        /** @brief returns the size of the variable data */
        inline size_t var_size() const { return buf_.size(); }

        /* ------------------------------------------------------------------------ *
        **    conversion operations
        ** ------------------------------------------------------------------------ */

        /* conversions to other types */

        /**
        @brief convert to common::int64
        @param v is where to put the data
        @return true if successful

        this function delegates the conversion to common::int64 class
         */
        inline bool to_integer(int64 & v) const { return v.from_string(data()); }

        /**
        @brief convert to long long
        @param v is where to put the data
        @return true if successful

        The function assumes that the internal string contains a max 64 bit integer in a string form.
         */
        bool to_integer(long long & v) const;

        /**
        @brief convert to common::dbl
        @param v is where to put the data
        @return true if successful

        this function delegates the conversion to common::dbl class
         */
        inline bool to_double(dbl & v) const { return v.from_string(data()); }

        /**
        @brief convert to double
        @param v is where to put the data
        @return true if successful

        The function assumes that the internal string contains a floating point value in a string form.
         */
        bool to_double(double & v) const;

        /**
        @brief convert to common::str
        @param v is where to put the data
        @return true if successful

        this function delegates the conversion to common::str class
         */
        inline bool to_string(str & v) const { v.buf_ = buf_; return true; }

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

        This function treats the internal data as a (wchar_t) string. This representation
        is converted to char* representation. This function checks wether the string is terminated
        by 0. If so, that character is not copied.
         */
        bool to_string(std::string & v) const;

        /**
        @brief convert to common::binry
        @param v is where to put the data
        @return true if successful

        this function delegates the conversion to the other common::binry instance (this class)
         */
        inline bool to_binary(binry & v) const { return v.from_binary(buf_.data(),buf_.size()); }

        /**
        @brief convert to ptr,size
        @param v points to a memory region where to store the data
        @param sz will contain the number of bytes copied
        @return true if successful

        this function assumes that v has enough space. sz will be updated to indicate, how
        many bytes were stored. this function will place the trailing zero as well. to check
        how much space is needed use the nbytes() function.
         */
        bool to_binary(unsigned char * v, size_t & sz) const;

        /**
        @brief convert to ptr,size
        @param v points to a memory region where to store the data
        @param sz will contain the number of bytes copied
        @return true if successful

        this function assumes that v has enough space. sz will be updated to indicate, how
        many bytes were stored. this function will place the trailing zero as well. to check
        how much space is needed use the nbytes() function.
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

        on linux wchar_t is usually 4 bytes long, thus no padding will happen.

        traling zero character will not be placed into the XDR stream.
         */
        bool to_xdr(xdrbuf & b) const;

        /**
        @brief polymorphic call to store the data into common::var type
        @param v is where to place the data
        @return true if successful

        this function delegates the conversion to v
         */
        inline bool to_var(var & v) const { return v.from_string(data()); }

        /* conversions from other types */
        /**
        @brief convert an common::int64
        @param v is the value to be read in
        @return true if successful

        this function delegates the conversion task to the common::int64 class
         */
        inline bool from_integer(const int64 & v ) { return v.to_string(*this); }

        /**
        @brief convert a long long
        @param v is the value to be stored
        @return true if successful

        the result of this conversion is a formatted character string. conversion
        specification used is: "%lld"
         */
        bool from_integer(long long v);

        /**
        @brief convert a common::dbl
        @param v is the value to be read in

        this function delegates the conversion task to the common::dbl class
         */
        inline bool from_double(const dbl & v) { return v.to_string(*this); }

        /**
        @brief convert a double
        @param v is the value to be stored
        @return true if successful

        the result of this conversion is a formatted character string. conversion
        specification used is: "%.12f"
         */
        bool from_double(double v);

        /**
        @brief convert a common::str
        @param v is the value to be read in
        @return true if successful

        this function uses the internal copy operator
         */
        inline bool from_string(const str & v)   { buf_ = v.buf_; return true; }

        /**
        @brief convert a common::ustr
        @param v is the value to be read in
        @return true if successful

        this function uses the internal copy operator for conversion
         */
        inline bool from_string(const ustr & v)  { *this = v; return true; }

        /**
        @brief convert a std::string
        @param v is the value to be read in
        @return true if successful

        this function uses the internal copy operator for conversion
         */
        bool from_string(const std::string & v);

        /**
        @brief convert a char* string
        @param v is the value to be read in
        @return true if successful

        this function uses the internal copy operator for conversion
         */
        bool from_string(const char * v);

        /**
        @brief convert a wchar_t* string
        @param v is the value to be read in
        @return true if successful

        this function uses the internal copy operator for conversion
         */
        bool from_string(const wchar_t * v);

        /**
        @brief convert a common::binry
        @param v is the value to be read in
        @return true if successful

        this function delegates the conversion task to the common::binry class
         */
        inline bool from_binary(const binry & v) { return v.to_string(*this); }

        /**
        @brief convert a memory region
        @param v is the pointer to the start of the memory region
        @param sz is the number of bytes to be read
        @return true if successful

        the given memory region will be copied in. this function assumes that the memory
        region contains a wchar_t* string. after copying it ensures that the string will
        contain a trailing zero.
         */
        bool from_binary(const unsigned char * v,size_t sz);

        /**
        @brief convert a memory region
        @param v is the pointer to the start of the memory region
        @param sz is the number of bytes to be read
        @return true if successful

        the given memory region will be copied in. this function assumes that the memory
        region contains a wchar_t* string. after copying it ensures that the string will
        contain a trailing zero.
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
        inline bool from_var(const var & v) { return v.to_string(*this); }
        
        /** 
        @brief serialize contents of objects
        @param buf archiver class to/from serialize
        @throw common::exc
        */
        virtual inline void serialize(arch & buf) { buf.serialize(*this); }
      private:
        tbuf<buf_size>   buf_;
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_common_str_hh_included_ */
