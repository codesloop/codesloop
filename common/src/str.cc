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

#include "str.hh"
#include "exc.hh"
#include "common.h"

/**
  @file csl_common/src/str.cc
  @brief implementation of simple string class
 */

namespace csl
{
  namespace common
  {
    void str::ensure_trailing_zero()
    {
      buf_.allocate( size() * sizeof(wchar_t) );

      wchar_t c = L'\0';

      buf_.append( (unsigned char *)&c,sizeof(c) );
    }

    str& str::operator+=(const str& s)
    {
      // remove trailing NULL character
      buf_.allocate( size() * sizeof(wchar_t) );

      // append new string
      buf_.append( s.buffer() );

      return *this;
    }

    str& str::operator+=(const wchar_t * s)
    {
      // remove trailing NULL character
      buf_.allocate( size() * sizeof(wchar_t) );

      // append new string
      buf_.append( (unsigned char *)s, sizeof(wchar_t) * (wcslen(s)+1) );

      return *this;
    }

    str str::substr(const size_t start, const size_t length) const
    {
      str s;
      size_t len = length; 
      wchar_t t = 0;

      if ( start > size() )
        throw exc(exc::rs_invalid_param,exc::cm_str,L"out of range");

      // shrink length to fit in
      if ( size() < length + start )
        len = size() - start;

      // copy string
      s.buf_.set( (unsigned char *)(data() + start), (len) * sizeof(wchar_t) );
      // terminate string
      s.buf_.append( (unsigned char *)&t, sizeof(wchar_t) );

      return s;
    }

    str::str(const char * mbstring) : obj()
    {
      size_t len =  strlen(mbstring)+1;

      wchar_t * nptr = (wchar_t *)buf_.allocate( len * sizeof(wchar_t) );

      if ( mbstowcs( nptr, mbstring, len ) != size_t(-1) )
      {
        // wcstring includes trailing zero char
        nptr[len-1] = L'\0';
      }
    }

    str& str::operator=(const char * mbstring )
    {
      size_t len =  strlen(mbstring)+1;

      wchar_t * nptr = (wchar_t *)buf_.allocate( len * sizeof(wchar_t) );

      if ( mbstowcs( nptr, mbstring, len ) != size_t(-1) )
      {
        // wcstring includes trailing zero char
        nptr[len-1] = L'\0';
      }

      return *this;
    }

    size_t str::find(wchar_t c) const
    {
      size_t ret = npos;
      size_t len = size();

      for ( size_t pos = 0; pos < len ; pos++ ) 
      {
        if ( (*this)[pos] == c ) {
          ret = pos;
          break;
        }
      }

      return ret;
    }

    size_t str::rfind(wchar_t c) const
    {
      size_t ret = npos;
      size_t len = size();

      for ( size_t pos = len-1; pos >= 0 ; --pos )
      {
        if ( (*this)[pos] == c ) {
          ret = pos;
          break;
        }
      }

      return ret;
    }

    size_t str::find(const str & s) const
    {
      wchar_t * p = wcsstr( data(), s.data() );
      size_t ret = npos;

      if ( p != NULL ) {
        ret = p - data();
      }

      return ret;
    }

    size_t str::find(const wchar_t * str) const
    {
      if( empty() ) return npos;
      if( !str )    return npos;

      const wchar_t * res = 0;

      if( (res = wcsstr(data(),str)) == NULL ) return npos;

      return (res-data());
    }

    wchar_t str::at(const size_t n) const
    {
      if ( n > wcslen( data() ) )
        throw exc(exc::rs_invalid_param,exc::cm_str);

      return data()[n];
    }

  };
};

/* EOF */
