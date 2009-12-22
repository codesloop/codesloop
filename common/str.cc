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

#include "codesloop/common/str.hh"
#include "codesloop/common/ustr.hh"
#include "codesloop/common/exc.hh"
#include "codesloop/common/common.h"
#include "codesloop/common/xdrbuf.hh"
#include "codesloop/common/logger.hh"


/**
  @file common/src/str.cc
  @brief implementation of simple string class
 */

namespace csl
{
  namespace common
  {
    namespace
    {
      static unsigned long long CRCTable[256];

      static int initalize_CRCTable()
      {
        int i, j;
        unsigned long long part;

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

        return 0;
      }
    }

    str::str(const ustr & other) : csl::common::var(), buf_( L'\0' )
    {
      uint64_t sz = other.size();

      wchar_t * b = reinterpret_cast<wchar_t *>(buf_.allocate( sz * sizeof(wchar_t) ));

      if( sz && b )
      {
        uint64_t szz = ::mbstowcs( b, other.data(), static_cast<size_t>(sz) );
        // fix length, because utf-8 may occupy more then one character that may be converted
        // to a single wchar_t, for this reason we may need to shrink the buffer
        buf_.allocate( szz * sizeof(wchar_t) );
      }

      ensure_trailing_zero();
    }

    str& str::operator+=(const ustr& other)
    {
      *this += str(other);
      return *this;
    }

    str & str::operator=(const ustr & other)
    {
      uint64_t sz = other.size();

      wchar_t * b = reinterpret_cast<wchar_t *>(buf_.allocate( sz * sizeof(wchar_t) ));

      if( sz && b )
      {
        uint64_t szz = ::mbstowcs( b, other.data(), static_cast<size_t>(sz) );

        // fix length, because utf-8 may occupy more then one character that may be converted
        // to a single wchar_t, for this reason we may need to shrink the buffer

        if( szz == static_cast<uint64_t>(-1) )
        {
          buf_.reset();
        }
        else
        {
          buf_.allocate( szz * sizeof(wchar_t) );
        }
      }

      ensure_trailing_zero();

      return *this;
    }

    void str::ensure_trailing_zero()
    {

      wchar_t  c = 0;
      uint64_t  sz = (nbytes()/sizeof(wchar_t))-1;

      if( buf_.size() == 0 )
      {
        buf_.append( reinterpret_cast<const unsigned char *>(&c),sizeof(wchar_t) );
      }
      else if( data()[sz] != 0 )
      {
        buf_.append( reinterpret_cast<const unsigned char *>(&c),sizeof(wchar_t) );
      }
    }

    str& str::operator+=(const str& s)
    {
      uint64_t  sz = size();

      if( nbytes() > 0 && data()[sz] == 0 )
      {
        // exclude the trailing zero character
        buf_.allocate( sz * sizeof(wchar_t) );
      }

      buf_.append( s.buffer() );

      ensure_trailing_zero();

      return *this;
    }

    str& str::operator+=(const wchar_t * s)
    {
      uint64_t  sz = size();

      if( nbytes() > 0 && data()[sz] == 0 )
      {
        // exclude the trailing zero character
        buf_.allocate( sz * sizeof(wchar_t) );
      }

      buf_.append( reinterpret_cast<const unsigned char *>(s), sizeof(wchar_t) * (::wcslen(s)+1) );

      ensure_trailing_zero();

      return *this;
    }

    str str::substr(const uint64_t start, const uint64_t length) const
    {
      str s;
      uint64_t len = length;
      uint64_t sz = size();

      if ( start > sz )
      {
        THRR(exc::rs_invalid_param,L"out of range",s);
      }

      // shrink length to fit in
      if ( sz < length + start ) len = sz - start;

      // copy string
      s.buf_.set( reinterpret_cast<const unsigned char *>(data() + start), (len*sizeof(wchar_t)) );
      s.ensure_trailing_zero();

      return s;
    }

    str::str(const char * st) : csl::common::var(), buf_( L'\0' )
    {
      if( !st ) return;

      uint64_t len = ::strlen(st)+1;
      uint64_t ssz = 0;

      wchar_t * nptr = reinterpret_cast<wchar_t *>(buf_.allocate( len * sizeof(wchar_t) ));

      if ( (ssz = ::mbstowcs( nptr, st, static_cast<size_t>(len) )) != uint64_t(-1) )
      {
        // may need to shrink, when utf-8 chars occupy more than one character
        buf_.allocate( ssz * sizeof(wchar_t) );
        ensure_trailing_zero();
      }
      else
      {
        buf_.reset();
        ensure_trailing_zero();
        THRNORET(exc::rs_conv_error);
      }
    }

    str& str::operator=(const char * st)
    {
      if( !st ) return *this;

      uint64_t len =  strlen(st)+1;
      uint64_t ssz = 0;

      wchar_t * nptr = reinterpret_cast<wchar_t *>(buf_.allocate( len * sizeof(wchar_t) ));

      if ( (ssz = ::mbstowcs( nptr, st, static_cast<size_t>(len) )) != uint64_t(-1) )
      {
        // may need to shrink, when utf-8 chars occupy more than one character
        buf_.allocate( ssz * sizeof(wchar_t) );
        ensure_trailing_zero();
      }
      else
      {
        buf_.reset();
        ensure_trailing_zero();
        THRC(exc::rs_conv_error,*this);
      }
      return *this;
    }

    uint64_t str::find(wchar_t c) const
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

    uint64_t str::rfind(wchar_t c) const
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

    uint64_t str::find(const str & s) const
    {
      wchar_t * p = ::wcsstr( data(), s.data() );
      uint64_t ret = npos;

      if ( p != NULL ) {
        ret = p - data();
      }

      return ret;
    }

    uint64_t str::find(const wchar_t * strv) const
    {
      if( empty() )  return npos;
      if( !strv )    return npos;

      const wchar_t * res = 0;

      if( (res = ::wcsstr(data(), strv)) == NULL ) return npos;

      return (res-data());
    }

    wchar_t str::at(const uint64_t n) const
    {
      if ( n > ::wcslen( data() ) )
      {
        THR(exc::rs_invalid_param,0);
      }

      return data()[n];
    }

    /* conversions to other types */
    bool str::to_integer(long long & v) const
    {
      wchar_t * endp = 0;
      v = WCSTOLL( data(), &endp, 0 );
      return (endp != 0);
    }

    bool str::to_double(double & v) const
    {
      wchar_t * endp = 0;
      v = WCSTOD( data(), &endp );
      return true;
    }

    bool str::to_string(ustr & v) const
    {
      v = *this;
      return true;
    }

    bool str::to_string(std::string & v) const
    {
      if( nbytes() == 1 )
      {
        v.clear();
        return true;
      }
      ustr us(*this);
      v = us.c_str();
      return true;
    }

    bool str::to_binary(unsigned char * v, uint64_t & sz) const
    {
      if( !v ) { sz = 0; return false; }
      ::memcpy( v, data(), static_cast<size_t>(nbytes()) );
      sz = nbytes();
      return true;
    }

    bool str::to_binary(void * v, uint64_t & sz) const
    {
      if( !v ) { sz = 0; return false; }
      ::memcpy( v, data(), static_cast<size_t>(nbytes()) );
      sz = nbytes();
      return true;
    }

    bool str::to_xdr(xdrbuf & b) const
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
    bool str::from_integer(long long v)
    {
      wchar_t * p = reinterpret_cast<wchar_t *>(buf_.allocate(buf_size-1));
      int ret = SWPRINTF(p,(buf_size-1),L"%lld",v);
      return (buf_.allocate( (ret+1)*sizeof(wchar_t) ) != 0);
    }

    bool str::from_double(double v)
    {
      wchar_t * p = reinterpret_cast<wchar_t *>(buf_.allocate(buf_size-1));
      int ret = SWPRINTF(p,(buf_size-1),L"%.12f",v);
      return (buf_.allocate( (ret+1)*sizeof(wchar_t) ) != 0);
    }

    bool str::from_string(const std::string & v)
    {
      if( !v.size() ) { reset(); }
      else            { *this = v; }
      return true;
    }

    bool str::from_string(const char * v)
    {
      if( !v ) { reset(); }
      else     { *this = v; }
      return true;
    }

    bool str::from_string(const wchar_t * v)
    {
      if( !v ) { reset(); }
      else     { *this = v; }
      return true;
    }

    bool str::from_binary(const unsigned char * v,uint64_t sz)
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

    bool str::from_binary(const void * v,uint64_t sz)
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

    bool str::from_xdr(xdrbuf & v)
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

    int64 str::crc64() const
    {
      int64 ret;
      unsigned long long crc = 0x0000000000000000ULL;
      const unsigned char * seq = ucharp_data();

      while (*seq)
          crc = CRCTable[(crc ^ *seq++) & 0xff] ^ (crc >> 8);

      return int64(crc);
    }

  };
};

AUTOEXEC( csl, common, initalize_crc_table, initalize_CRCTable );

/* EOF */
