/*
Copyright (c) 2008,2009, Tamas Foldi

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
   @brief simple but efficient str class
   @todo document me
 */

#include <stdlib.h>
#include <string.h>
#include "obj.hh"
#include "exc.hh"
#include "tbuf.hh"
#ifdef __cplusplus

namespace csl
{
  namespace common
  {
    /** @todo document me */
    class str 
    {      
      public:
        enum { buf_size = 128 * sizeof(wchar_t) }; 

        /** @brief constructor */
        inline str()  { }
        
        /** @brief destructor 
        since there are not virtual functions, and we do not excpect inherited
        classes from str the destructor is not virtual. this casues a bit
        faster initalization, because no vptr table is required, i guess 
        */
        ~str() {}

        /** @brief copy constructor */
        inline str(const str& s) 
        {
          buf_ = s.buf_;
        }

        /** @brief copy constructor */
        inline str(const wchar_t * wcs)
        {
          buf_.set((unsigned char *)wcs,sizeof(wchar_t) * (wcslen(wcs)+1));
        }
        
        /** @brief let equal operator */
        inline str& operator=(const wchar_t * wcs)
        {
          buf_.set((unsigned char *)wcs,sizeof(wchar_t) * (wcslen(wcs)+1));
          return *this; 
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
        inline bool operator==(const wchar_t * s) const
        {
          return !wcscmp( data(), s);
        }        
        /** @brief is equal operator */
        inline bool operator==(const str& s) const
        {
          return !wcscmp( data(), s.data());
        }

        /** @brief resets str buffer */
        inline void clear() {
          buf_.reset();
        }

        /** @brief gets str size 
        */
        inline const size_t size() const
        {
          return empty() ? 0 : wcslen( data() );
        }

        /** @brief true if empty str ("") is defined */
        inline const bool empty() const
        {
          return !buf_.has_data();
        } 
        
        /** @brief returns the background c str */
        inline const wchar_t * c_str() const
        {
          return( data() ); // TODO: have a char * version
        } 
        
        /**@brief unchecked access to buffer */
        inline wchar_t operator[](const size_t n) const
        {
          return data()[n];
        }

        /** @brief check access to buffer */
        inline wchar_t at(const size_t n) const
        {
          if ( n > wcslen( data() ) )
            throw exc(exc::rs_invalid_param,exc::cm_str);

          return data()[n];
        }

        /** @brief get data as wchar_t */
        inline const wchar_t * data() const
        { 
          return (const wchar_t *)buf_.data(); 
        }

        inline const tbuf<buf_size> & buffer() const 
        {
          return buf_;
        }

        /**@brief substr 
           @param start start from this position
           @param length take length bytes from origin str
        */
        str substr(const size_t start, const size_t length) const;
        
      private:
        tbuf<buf_size> buf_;
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_common_str_hh_included_ */
