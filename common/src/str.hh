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
   @todo document me
 */

#include "common.h"
#include "obj.hh"
#include "tbuf.hh"
#ifdef __cplusplus
#include <string>

namespace csl
{
  namespace common
  {
    class ustr;

    /** @todo document me */
    class str : public csl::common::obj
    {
      public:
        enum { buf_size = 128 * sizeof(wchar_t), npos = 0xFFFFFFFF };

        /** @brief reset internal data */
        inline void reset() { buf_.reset(); }

        /** @brief constructor */
        inline str() : csl::common::obj(), buf_((wchar_t)L'\0') { }

        /** @brief destructor
        *   since there are not virtual functions, and we do not expect inherited
        *   classes from str the destructor is not virtual. this casues a bit
        *   faster initalization, because no vptr table is required, i guess
        **/
        ~str() {}

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
        inline str(const str& s) : csl::common::obj(), buf_((wchar_t)L'\0')
        {
          buf_ = s.buf_;
        }

        /** @brief let equal operator */
        inline str& operator=(const str& s)
        {
          buf_ = s.buffer();
          return *this;
        }

        /** @brief append operator */
        str& operator+=(const str&);

        /** @brief append operator with two parameters */
        inline friend str operator+(const str& lhs, const str& rhs)
        {
          return str(lhs) += rhs;
        }

        /** @brief is equal operator */
        inline bool operator==(const str& s) const
        {
          return !wcscmp( data(), s.data());
        }

        /** @brief find a string
         *  @param s is the string to be found
         *  @returns npos if not found or the position
         **/
        size_t find(const str & s) const;

        /** @brief substr
         *  @param start start from this position
         *  @param length take length bytes from origin str
         **/
        str substr(const size_t start, const size_t length) const;

        /* ------------------------------------------------------------------------ *
        **    ustr operations
        ** ------------------------------------------------------------------------ */

        /** @brief copy constructor */
        str(const ustr & other);

        /** @brief copy operator */
        str & operator=(const ustr & other);

        /** @brief append operator */
        str& operator+=(const ustr&);

        /* ------------------------------------------------------------------------ *
        **    char * operations
        ** ------------------------------------------------------------------------ */

        /** @brief copy constructor */
        str(const char *);

        /** @brief copy operator */
        str& operator=(const char *);

        /* ------------------------------------------------------------------------ *
        **    wchar_t * operations
        ** ------------------------------------------------------------------------ */

        /** @brief copy constructor */
        inline str(const wchar_t * wcs) : csl::common::obj(), buf_((wchar_t)L'\0')
        {
          if( !wcs ) return;
          buf_.set((unsigned char *)wcs,sizeof(wchar_t) * (wcslen(wcs)+1));
        }

        /** @brief copy operator */
        inline str& operator=(const wchar_t * wcs)
        {
          if( !wcs ) return *this;
          buf_.set((unsigned char *)wcs,sizeof(wchar_t) * (wcslen(wcs)+1));
          return *this;
        }

        /** @brief append operator */
        str& operator+=(const wchar_t * str);

        /** @brief is equal operator */
        inline bool operator==(const wchar_t * s) const
        {
          return !wcscmp( data(), s);
        }

        /** @brief returns the background c str */
        inline const wchar_t * c_str() const
        {
          return( data() ); // TODO: have a char * version
        }

        /** @brief assign string
         *  @param start is the start of the string
         *  @param lend is the end of the string
         **/
        inline str & assign(const wchar_t * start, const wchar_t * end)
        {
          buf_.set( (unsigned char *)start, (const char *)end-(const char *)start);
          return *this;
        }

        /** @brief find a wide character in the string
         *  @param s is the string to be found
         *  @returns npos if not found or the position
         **/
        size_t find(const wchar_t * s) const;

        /** @brief get data as wchar_t */
        inline const wchar_t * data() const
        {
          return (const wchar_t *)buf_.data();
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
          return data()[n];
        }

        /** @brief check access to buffer */
        wchar_t at(const size_t n) const;

        /** @brief find a wide character in the string
         *  @param w is the character to be found
         *  @returns npos if not found or the position
         **/
        size_t find(wchar_t w) const;

        /** @brief reverse find a wide character in the string
         *  @param w is the character to be found
         *  @returns npos if not found or the position
         **/
        size_t rfind(wchar_t w) const;

        /* ------------------------------------------------------------------------ */

        /** @brief resets str buffer */
        inline void clear() {
          buf_.reset();
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
          return empty() ? 0 : wcstombs(NULL, data(), 0);
        }

        /** @brief true if empty str ("") is defined */
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

      private:
        tbuf<buf_size>   buf_;
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_common_str_hh_included_ */
