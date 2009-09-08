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

#include "codesloop/common/dbl.hh"
#include "codesloop/common/int64.hh"
#include "codesloop/common/str.hh"
#include "codesloop/common/ustr.hh"
#include "codesloop/common/binry.hh"
#include "exc.hh"
#include "codesloop/common/xdrbuf.hh"

/**
  @file common/src/int64.cc
  @brief implementation of 64 bit integer type
 */

namespace csl
{
  namespace common
  {
    /* conversions to other types */
    bool int64::to_double(dbl & v) const
    {
      return v.from_integer(value_);
    }

    bool int64::to_string(str & v) const
    {
      return v.from_integer(value_);
    }

    bool int64::to_string(ustr & v) const
    {
      return v.from_integer(value_);
    }

    bool int64::to_string(std::string & v) const
    {
      ustr s;
      return (s.from_integer(value_) && s.to_string(v));
    }

    bool int64::to_binary(binry & v) const
    {
      return v.from_integer(value_);
    }

    bool int64::to_binary(unsigned char * v, size_t & sz) const
    {
      binry b;
      return (b.from_integer(value_) && b.to_binary(v,sz));
    }

    bool int64::to_binary(void * v, size_t & sz) const
    {
      binry b;
      return (b.from_integer(value_) && b.to_binary(v,sz));
    }

    bool int64::to_xdr(xdrbuf & b) const
    {
      int32_t high = static_cast<int32_t>(value_>>32);
      int32_t low  = static_cast<int32_t>(value_&0xffffffff);

      try
      {
        b << high;
        b << low;
        return true;
      }
      catch( exc e )
      {
        return false;
      }
    }

    /* conversions from other types */
    bool int64::from_double(const dbl & v)
    {
      return v.to_integer(value_);
    }

    bool int64::from_string(const str & v)
    {
      return v.to_integer(value_);
    }

    bool int64::from_string(const ustr & v)
    {
      return v.to_integer(value_);
    }

    bool int64::from_string(const std::string & v)
    {
      ustr s;
      return (s.from_string(v) && s.to_integer(value_));
    }

    bool int64::from_string(const char * v)
    {
      ustr s;
      return (s.from_string(v) && s.to_integer(value_));
    }

    bool int64::from_string(const wchar_t * v)
    {
      str s;
      return (s.from_string(v) && s.to_integer(value_));
    }

    bool int64::from_binary(const binry & v)
    {
      return v.to_integer(value_);
    }

    bool int64::from_binary(const unsigned char * v,size_t sz)
    {
      binry b;
      return (b.from_binary(v,sz) && b.to_integer(value_));
    }

    bool int64::from_binary(const void * v,size_t sz)
    {
      binry b;
      return (b.from_binary(v,sz) && b.to_integer(value_));
    }

    bool int64::from_xdr(xdrbuf & v)
    {
      int32_t high=0;
      int32_t low=0;

      try
      {
        v >> high;
        v >> low;

        value_ = ( (static_cast<long long>(high)<<32) + static_cast<long long>(low) );

        return true;
      }
      catch( exc e )
      {
        return false;
      }
    }

  };
};

/* EOF */
