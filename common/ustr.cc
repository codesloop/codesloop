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

#include "codesloop/common/ustr.hh"
#include "codesloop/common/str.hh"
#include "codesloop/common/exc.hh"
#include "codesloop/common/common.h"
#include "codesloop/common/xdrbuf.hh"

/**
  @file common/src/ustr.cc
  @brief implementation of simple UTF-8 string class
 */

namespace csl
{
  namespace common
  {
    ustr::ustr(const str & other) : csl::common::var(), buf_( static_cast<unsigned char>(0) )
    {
      uint64_t sz = other.nbytes()-1;

      /* over allocating, but this saves our arse when utf-8 results more then 1 character */
      char * b = reinterpret_cast<char *>(buf_.allocate( sz ));

      if( sz>0 && b!=0 )
      {
        uint64_t szz = ::wcstombs( b, other.data(), static_cast<size_t>(sz) );

        if( szz == static_cast<uint64_t>(-1) )
        {
          buf_.reset();
        }
        else
        {
          buf_.allocate( szz );
        }
      }

      ensure_trailing_zero();
    }

    ustr& ustr::operator+=(const str& other)
    {
      *this += ustr(other);
      return *this;
    }

    ustr & ustr::operator=(const str & other)
    {
      uint64_t sz = other.nbytes()-1;

      /* over allocating, but this saves our arse when utf-8 results more then 1 character */
      char * b = reinterpret_cast<char *>(buf_.allocate( sz ));

      if( sz && b )
      {
        uint64_t szz = ::wcstombs( b, other.data(), static_cast<size_t>(sz) );

        if( szz == static_cast<uint64_t>(-1) )
        {
          buf_.reset();
        }
        else
        {
          buf_.allocate( szz );
        }
      }

      ensure_trailing_zero();

      return *this;
    }

    void ustr::ensure_trailing_zero()
    {
      unsigned char c = 0;
      uint64_t     sz = buf_.size();

      if( buf_.size() == 0 )       { buf_.append( &c,1 ); }
      if( buf_.data()[sz-1] != 0 ) { buf_.append( &c,1 ); }
    }

    ustr& ustr::operator+=(const ustr& s)
    {
      uint64_t sz = buf_.size();

      if( sz > 0 && data()[sz-1] == 0 ) buf_.allocate( sz-1 );

      buf_.append( s.buffer() );

      ensure_trailing_zero();

      return *this;
    }

    ustr& ustr::operator+=(const char * s)
    {
      uint64_t sz = buf_.size();

      if( sz > 0 && data()[sz-1] == 0 ) buf_.allocate( sz-1 );

      // strlen only cares about trailing zero, so multibyte chars will not confuse here
      buf_.append( reinterpret_cast<const unsigned char *>(s), (::strlen(s)+1) );

      ensure_trailing_zero();

      return *this;
    }

    ustr ustr::substr(const uint64_t start, const uint64_t length) const
    {
      ustr s;
      uint64_t len = length;
      uint64_t sz = size();

      if ( start > sz )
        throw exc(exc::rs_invalid_param,L"out of range");

      // shrink length to fit in
      if ( sz < length + start ) len = sz - start;

      // copy string
      s.buf_.set( reinterpret_cast<const unsigned char *>(data() + start), len );
      s.ensure_trailing_zero();

      return s;
    }

    uint64_t ustr::find(char c) const
    {
      uint64_t ret = npos;
      uint64_t len = size();

      for ( uint64_t pos = 0; pos < len ; pos++ )
      {
        if ( (*this)[pos] == c ) {
          ret = pos;
          break;
        }
      }

      return ret;
    }

    uint64_t ustr::rfind(char c) const
    {
      uint64_t ret = npos;
      uint64_t len = size();

      for ( uint64_t pos = len-1; pos >= 0 ; --pos )
      {
        if ( (*this)[pos] == c ) {
          ret = pos;
          break;
        }
      }

      return ret;
    }

    uint64_t ustr::find(const ustr & s) const
    {
      char * p = strstr( data(), s.data() );
      uint64_t ret = npos;

      if ( p != NULL ) {
        ret = p - data();
      }

      return ret;
    }

    uint64_t ustr::find(const char * str) const
    {
      if( empty() ) return npos;
      if( !str )    return npos;

      const char * res = 0;

      if( (res = strstr(data(),str)) == NULL ) return npos;

      return (res-data());
    }

    char ustr::at(const uint64_t n) const
    {
      if ( n > nbytes() )
        THR(exc::rs_invalid_param,0);

      return data()[n];
    }

    /* conversions to other types */
    bool ustr::to_integer(int64_t & v) const
    {
      v = ATOLL(data());
      return true;
    }

    bool ustr::to_double(double & v) const
    {
      v = atof(data());
      return true;
    }

    bool ustr::to_string(std::string & v) const
    {
      v = data();
      return true;
    }

    bool ustr::to_binary(unsigned char * v, uint64_t & sz) const
    {
      if( !v ) { sz = 0; return false; }
      ::memcpy( v, data(), static_cast<size_t>(nbytes()) );
      sz = nbytes();
      return true;
    }

    bool ustr::to_binary(void * v, uint64_t & sz) const
    {
      if( !v ) { sz = 0; return false; }
      ::memcpy( v, data(), static_cast<size_t>(nbytes()) );
      sz = nbytes();
      return true;
    }

    bool ustr::to_xdr(xdrbuf & b) const
    {
      try
      {
        b << (*this);
        return true;
      }
      catch( exc e )
      {
        return false;
      }
    }

    /* conversions from other types */
    bool ustr::from_integer(int64_t v)
    {
      char * p = reinterpret_cast<char *>(buf_.allocate(buf_size-1));
      int ret = SNPRINTF(p,(buf_size-1),"%lld",v);
      return (buf_.allocate( ret+1 ) != 0);
    }

    bool ustr::from_double(double v)
    {
      char * p = reinterpret_cast<char *>(buf_.allocate(buf_size-1));
      int ret = SNPRINTF(p,(buf_size-1),"%.12f",v);
      return (buf_.allocate( ret+1 ) != 0);
    }

    bool ustr::from_string(const std::string & v)
    {
      if( !v.size() ) { reset(); }
      else            { *this = v; }
      return true;
    }

    bool ustr::from_string(const char * v)
    {
      if( !v ) { reset(); }
      else     { *this = v; }
      return true;
    }

    bool ustr::from_string(const wchar_t * v)
    {
      if( !v ) { reset(); }
      else     { *this = v; }
      return true;
    }

    bool ustr::from_binary(const unsigned char * v,uint64_t sz)
    {
      if( !v || !sz )
      {
        reset();
      }
      else
      {
        buf_.set( v, sz );
        ensure_trailing_zero();
      }
      return true;
    }

    bool ustr::from_binary(const void * v,uint64_t sz)
    {
      if( !v || !sz )
      {
        reset();
      }
      else
      {
        buf_.set( reinterpret_cast<const unsigned char *>(v), sz );
        ensure_trailing_zero();
      }
      return true;
    }

    bool ustr::from_xdr(xdrbuf & v)
    {
      try
      {
        v >> (*this);
        return true;
      }
      catch( exc e )
      {
        return false;
      }
    }

    int64 ustr::crc64() const
    {
      int64 ret;
      int i, j;
      uint64_t crc = 0x0000000000000000ULL, part;
      static int init = 0;
      static uint64_t CRCTable[256];
      const unsigned char * seq = ucharp_data();

      if (!init)
      {
          init = 1;
          for (i = 0; i < 256; i++)
          {
              part = i;
              for (j = 0; j < 8; j++)
              {
                  if (part & 1)
                      part = (part >> 1) ^ 0xd800000000000000ULL;
                  else
                      part >>= 1;
              }
              CRCTable[i] = part;
          }
      }

      while (*seq)
          crc = CRCTable[(crc ^ *seq++) & 0xff] ^ (crc >> 8);

      return int64(static_cast<int64_t>(crc));
    }
  };
};

/* EOF */
