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

#ifndef _csl_common_ustr_hh_included_
#define _csl_common_ustr_hh_included_

/**
   @file ustr.hh
   @brief simple but efficient utf-8 based string class
   @todo document me
 */

#include "common.h"
#include "obj.hh"
#include "tbuf.hh"
#include "str.hh"
#include "var.hh"
#ifdef __cplusplus
#include <string>

namespace csl
{
  namespace common
  {
    /** @todo document me */
    class ustr : public csl::common::var
    {
      public:
        enum { buf_size = 128, npos = 0xFFFFFFFF };

        inline virtual int var_type() { return CSL_TYPE_USTR; }

        /** @brief constructor */
        inline ustr() : csl::common::var(), buf_((unsigned char)0) { }

        /** @brief destructor
         *  since there are not virtual functions, and we do not expect inherited
         *  classes from ustr the destructor is not virtual. this casues a bit
         *  faster initalization, because no vptr table is required, i guess
         **/
        ~ustr() {}

        /* ------------------------------------------------------------------------ *
        **    pbuf operations
        ** ------------------------------------------------------------------------ */

        /** @brief copy operator */
        inline ustr & operator=(const pbuf & other)
        {
          buf_ = other;
          ensure_trailing_zero();
          return *this;
        }

        /* ------------------------------------------------------------------------ *
        **    str operations
        ** ------------------------------------------------------------------------ */

        /** @brief copy constructor */
        explicit ustr(const str & other);

        /** @brief copy operator */
        ustr & operator=(const str & other);

        /** @brief append operator */
        ustr& operator+=(const str&);

        /** @brief is equal operator */
        inline bool operator==(const str& s) const
        {
          ustr rhs(s);
          return (*this == rhs);
        }

        /* ------------------------------------------------------------------------ *
        **    ustr operations
        ** ------------------------------------------------------------------------ */

        /** @brief copy constructor */
        inline ustr(const ustr& s) : csl::common::var(), buf_((unsigned char)0)
        {
          buf_ = s.buf_;
        }

        /** @brief copy operator */
        inline ustr& operator=(const ustr& s)
        {
          buf_ = s.buffer();
          return *this;
        }

        /** @brief append operator */
        ustr& operator+=(const ustr&);

        /** @brief append operator with two parameters */
        inline friend ustr operator+(const ustr& lhs, const ustr& rhs)
        {
          return ustr(lhs) += rhs;
        }

        /** @brief is equal operator */
        inline bool operator==(const ustr& s) const
        {
          return (::strncmp( data(), s.data(), nbytes()) == 0);
        }

        /** @brief find a string
         *  @param s is the string to be found
         *  @returns npos if not found or the position
         **/
        size_t find(const ustr & s) const;

        /** @brief substr
         *  @param start start from this position
         *  @param length take length bytes from origin str
         **/
        ustr substr(const size_t start, const size_t length) const;

        /* ------------------------------------------------------------------------ *
        **    char * operations
        ** ------------------------------------------------------------------------ */

        /** @brief copy constructor */
        inline explicit ustr(const char * us) : csl::common::var(), buf_((unsigned char)0)
        {
          if( !us ) return;
          // strlen only cares about trailing zero, so multibyte chars will not confuse here
          buf_.set((const unsigned char *)us,::strlen(us)+1);
        }

        /** @brief let equal operator */
        inline ustr& operator=(const char * us)
        {
          if( !us ) return *this;
          // strlen only cares about trailing zero, so multibyte chars will not confuse here
          buf_.set((const unsigned char *)us,::strlen(us)+1);
          return *this;
        }

        /** @brief append operator with two parameters */
        inline friend ustr operator+(const char * lhs, const ustr& rhs)
        {
          return ustr(lhs) += rhs;
        }

        /** @brief append operator with two parameters */
        inline friend ustr operator+(const ustr& lhs, const char * rhs)
        {
          return ustr(lhs) += rhs;
        }

        /** @brief append operator */
        ustr& operator+=(const char * s);

        /** @brief is equal operator */
        inline bool operator==(const char * s) const
        {
          return (::strncmp( data(), s, nbytes()) == 0);
        }

        /** @brief returns the background c str */
        inline const char * c_str() const
        {
          return( data() );
        }

        /** @brief assign string
         *  @param start is the start of the string
         *  @param lend is the end of the string
         **/
        inline ustr & assign(const char * start, const char * end)
        {
          buf_.set( (unsigned char *)start, end-start);
          return *this;
        }

        /** @brief find a wide character in the string
         *  @param s is the string to be found
         *  @returns npos if not found or the position
         **/
        size_t find(const char * s) const;

        /** @brief get data as char * */
        inline const char * data() const
        {
          return (const char *)buf_.data();
        }

        /* ------------------------------------------------------------------------ *
        **    wchar_t * operations
        ** ------------------------------------------------------------------------ */

        /* ------------------------------------------------------------------------ *
        **    std::string operations
        ** ------------------------------------------------------------------------ */

        /** @brief let equal operator */
        ustr& operator=(const std::string & s)
        {
          return operator=(s.c_str());
        }

        /* ------------------------------------------------------------------------ *
        **    char operations
        ** ------------------------------------------------------------------------ */

        /** @brief unchecked access to buffer */
        inline char operator[](const size_t n) const
        {
          return data()[n];
        }

        /** @brief check access to buffer */
        char at(const size_t n) const;

        /** @brief find a wide character in the string
         *  @param w is the character to be found
         *  @returns npos if not found or the position
         **/
        size_t find(char w) const;

        /** @brief reverse find a wide character in the string
         *  @param w is the character to be found
         *  @returns npos if not found or the position
         **/
        size_t rfind(char w) const;

        /* ------------------------------------------------------------------------ *
        **    wchar_t operations
        ** ------------------------------------------------------------------------ */

        /* ------------------------------------------------------------------------ */

        /** @brief resets ustr buffer */
        inline void clear()
        {
          reset();
        }

        /** @brief reset internal data */
        inline void reset()
        {
          buf_.reset();
          buf_.set( (const unsigned char *)("\0"), 1 );
        }

        /** @brief gets ustr size  */
        inline size_t size() const
        {
          /* I presume the trailing zero is already there */
          return (empty() ? 0 : nbytes()-1 );
        }

        /** @brief gets the total allocated bytes */
        inline size_t nbytes() const
        {
          return buf_.size();
        }

        /** @brief return the number of characters in the string, excluding the trailing zero */
        inline size_t nchars() const
        {
          // strlen() wouldn't do here, because of multibyte utf-8 characters
          return (empty() ? 0 : ::mbstowcs(NULL,data(),0));
        }

        /** @brief true if empty ustr ("") is defined */
        inline bool empty() const
        {
          return !buf_.has_data();
        }

        inline const tbuf<buf_size> & buffer() const
        {
          return buf_;
        }

        inline tbuf<buf_size> & buffer()
        {
          return buf_;
        }

        void ensure_trailing_zero();

        /* ------------------------------------------------------------------------ *
        **    conversion operations
        ** ------------------------------------------------------------------------ */

        /* conversions to other types */
        inline bool to_integer(int64 & v) const { return v.from_string(data()); }
        bool to_integer(long long & v) const;

        inline bool to_double(dbl & v) const { return v.from_string(data()); }
        bool to_double(double & v) const;

        inline bool to_string(str & v) const { return v.from_string(data()); }
        inline bool to_string(ustr & v) const { return v.from_string(data()); }
        bool to_string(std::string & v) const;

        inline bool to_binary(binry & v) const { return v.from_binary(buf_.data(),buf_.size()); }
        bool to_binary(unsigned char * v, size_t & sz) const;
        bool to_binary(void * v, size_t & sz) const;

        bool to_xdr(xdrbuf & b) const;
        inline bool to_var(var & v) const { return v.from_string(data()); }

        /* conversions from other types */
        inline bool from_integer(const int64 & v) { return v.to_string(*this); }
        bool from_integer(long long v);

        inline bool from_double(const dbl & v) { return v.to_string(*this); }
        bool from_double(double v);

        inline bool from_string(const str & v)         { *this = v; return true; }
        inline bool from_string(const ustr & v)        { *this = v; return true; }

        bool from_string(const std::string & v);
        bool from_string(const char * v);
        bool from_string(const wchar_t * v);

        inline bool from_binary(const binry & v) { return v.to_string(*this); }
        bool from_binary(const unsigned char * v,size_t sz);
        bool from_binary(const void * v,size_t sz);

        bool from_xdr(xdrbuf & v);
        bool from_var(const var & v) { return v.to_string(*this); }

      private:
        tbuf<buf_size>   buf_;
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_common_ustr_hh_included_ */
