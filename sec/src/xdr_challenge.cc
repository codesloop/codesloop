/*
Copyright (c) 2008,2009, David Beck

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

#include "xdr_challenge.hh"
#include "mpool.hh"
#include "zfile.hh"
#include <string>

/**
  @file xdr_challenge.cc
  @brief XDR challenge packet helper
 */


using namespace csl::common;

namespace csl
{
  namespace sec
  {
    bool xdr_challenge::to_xdr(xdrbuf & buf)
    {
      bool ret = true;
      try
      {
        unsigned int nkeys = 0;

        for( unsigned int i=0;i<max_keys_offered;++i )
        {
          if( keys_[i].has_data() ) ++nkeys;
        }

        buf << nkeys;

        for( uint32_t i=0;i<max_keys_offered;++i )
        {
          if( keys_[i].has_data() )
          {
            if( !(keys_[i].to_xdr(buf)) ) return false;
          }
        }

        unsigned int is_compressed = 1;
        buf << is_compressed;
        buf << cert_.get_zdata();
      }
      catch( csl::common::exc e )
      {
        ret = false;
      }
      return ret;
    }

    bool xdr_challenge::from_xdr(xdrbuf & buf)
    {
      bool ret = true;
      try
      {
        unsigned int nkeys = 0;

        buf >> nkeys;

        if( nkeys > max_keys_offered ) return false;

        for( uint32_t i=0;i<nkeys;++i )
        {
          if( !(keys_[i].from_xdr(buf)) ) return false;
        }

        unsigned int is_compressed = 1;
        buf >> is_compressed;

        pbuf pb;
        buf >> pb;
        cert_.put_zdata(pb);
      }
      catch( csl::common::exc e )
      {
        ret = false;
      }
      return ret;
    }

    xdr_challenge::xdr_challenge(const xdr_challenge & other) { *this = other; }

    xdr_challenge & xdr_challenge::operator=(const xdr_challenge & other)
    {
      for( uint32_t i=0;i<max_keys_offered;++i )
      {
        keys_[i] = other[i];
      }
      cert_ = other.cert_;
      return *this;
    }

    xdr_challenge::xdr_challenge() { }
    xdr_challenge::~xdr_challenge() { }
  }
}

/* EOF */
