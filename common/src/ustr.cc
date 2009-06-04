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

#include "ustr.hh"
#include "exc.hh"
#include "common.h"

/**
  @file csl_common/src/ustr.cc
  @brief implementation of simple UTF-8 string class
 */

namespace csl
{
  namespace common
  {
    void ustr::ensure_trailing_zero()
    {
      unsigned char c = 0;
      size_t       sz = buf_.size();

      if( buf_.size() == 0 )       { buf_.append( &c,1 ); }
      if( buf_.data()[sz-1] != 0 ) { buf_.append( &c,1 ); }
    }

    ustr& ustr::operator+=(const ustr& s)
    {
      size_t sz = buf_.size();

      if( sz > 0 && data()[sz-1] == 0 ) buf_.allocate( sz-1 );

      buf_.append( s.buffer() );

      ensure_trailing_zero();

      return *this;
    }

    ustr& ustr::operator+=(const char * s)
    {
      size_t sz = buf_.size();

      if( sz > 0 && data()[sz-1] == 0 ) buf_.allocate( sz-1 );

      buf_.append( (unsigned char *)s, (strlen(s)+1) );

      ensure_trailing_zero();

      return *this;
    }

    ustr ustr::substr(const size_t start, const size_t length) const
    {
      ustr s;
      size_t len = length;
      size_t sz = size();

      if ( start > sz )
        throw exc(exc::rs_invalid_param,exc::cm_str,L"out of range");

      // shrink length to fit in
      if ( sz < length + start ) len = sz - start;

      // copy string
      s.buf_.set( (unsigned char *)(data() + start), len );
      s.ensure_trailing_zero();

      return s;
    }

    size_t ustr::find(char c) const
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

    size_t ustr::rfind(char c) const
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

    size_t ustr::find(const ustr & s) const
    {
      char * p = strstr( data(), s.data() );
      size_t ret = npos;

      if ( p != NULL ) {
        ret = p - data();
      }

      return ret;
    }

    size_t ustr::find(const char * str) const
    {
      if( empty() ) return npos;
      if( !str )    return npos;

      const char * res = 0;

      if( (res = strstr(data(),str)) == NULL ) return npos;

      return (res-data());
    }

    char ustr::at(const size_t n) const
    {
      if ( n > nbytes() )
        throw exc(exc::rs_invalid_param,exc::cm_str);

      return data()[n];
    }

  };
};

/* EOF */
