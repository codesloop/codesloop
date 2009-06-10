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

#include "dbl.hh"
#include "int64.hh"
#include "str.hh"
#include "ustr.hh"
#include "binry.hh"
#include "xdrbuf.hh"

/**
  @file csl_common/src/binry.cc
  @brief implementation of binary object type
 */

namespace csl
{
  namespace common
  {
    binry::binry() : var() { }

    binry::binry(const unsigned char * ptr,size_t sz) : var()
    {
      if( ptr && sz ) value_.set( ptr,sz );
    }

    /* conversions to other types */
    bool binry::to_integer(long long & v) const
    {
      if( value_.size() < sizeof(long long) ) { return false; }

      long long * p = (long long *)value_.data();
      v = *p;

      return true;
    }

    bool binry::to_double(double & v) const
    {
      if( value_.size() < sizeof(double) ) { return false; }

      double * p = (double *)value_.data();
      v = *p;

      return true;
    }

    bool binry::to_string(str & v) const
    {
      return v.from_binary(value_.data(),value_.size());
    }

    bool binry::to_string(ustr & v) const
    {
      return v.from_binary(value_.data(),value_.size());
    }

    bool binry::to_string(std::string & v) const
    {
      if( value_.size() == 0 )
      {
        v.clear();
      }
      else
      {
        size_t sz = value_.size();
        if( value_.data()[sz-1] == 0 ) --sz;
        v.assign( (const char *)value_.data(),(const char *)(value_.data()+sz) );
      }
      return true;
    }

    bool binry::to_binary(unsigned char * v, size_t & sz) const
    {
      if( !v ) return false;
      ::memcpy( v,value_.data(),value_.size() );
      sz = value_.size();
      return true;
    }

    bool binry::to_binary(void * v, size_t & sz) const
    {
      if( !v ) return false;
      ::memcpy( v,value_.data(),value_.size() );
      sz = value_.size();
      return true;
    }

    bool binry::to_xdr(xdrbuf & b) const
    {
      try
      {
        b << xdrbuf::bindata_t(value_.data(),value_.size());
        return true;
      }
      catch( exc e )
      {
        return false;
      }
    }

    /* conversions from other types */
    bool binry::from_integer(long long v)
    {
      long long * p = (long long *)value_.allocate(sizeof(long long));
      *p = v;
      return true;
    }

    bool binry::from_double(double v)
    {
      double * p = (double *)value_.allocate(sizeof(double));
      *p = v;
      return true;
    }

    bool binry::from_string(const str & v)
    {
      return v.to_binary(*this);
    }

    bool binry::from_string(const ustr & v)
    {
      return v.to_binary(*this);
    }

    bool binry::from_string(const std::string & v)
    {
      if( v.size() == 0 ) { value_.reset(); return true; }
      const char * p = v.c_str();
      return value_.set( (unsigned char *)p,v.size()+1 );
    }

    bool binry::from_string(const char * v)
    {
      if( !v ) return false;
      ustr s(v);
      return s.to_binary(*this);
    }

    bool binry::from_string(const wchar_t * v)
    {
      if( !v ) return false;
      str s(v);
      return s.to_binary(*this);
    }

    bool binry::from_binary(const binry & v)
    {
      value_ = v.value_;
      return true;
    }

    bool binry::from_binary(const unsigned char * v,size_t sz)
    {
      if( !v ) return false;
      if( !sz ) { value_.reset(); return true; }
      return value_.set(v,sz);
    }

    bool binry::from_binary(const void * v,size_t sz)
    {
      if( !v ) return false;
      if( !sz ) { value_.reset(); return true; }
      return value_.set((const unsigned char *)v,sz);
    }

    bool binry::from_xdr(xdrbuf & v)
    {
      try
      {
        unsigned int sz=0,max=0x3fffffff;
        bool r = v.get_data( value_,sz,max );
        if( sz == 0 ) value_.reset();
        else if( r == false ) return false;
        return true;
      }
      catch( exc e )
      {
        return false;
      }
    }

    bool binry::from_var(const var & v)
    {
      return v.to_binary(*this);
    }
  };
};

/* EOF */
