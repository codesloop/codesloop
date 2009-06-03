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

#include "xdr_response.hh"
#include "mpool.hh"

using namespace csl::common;

/**
  @file xdr_response.cc
  @brief XDR response packet helper
 */

namespace csl
{
  namespace sec
  {
    bool xdr_response::to_xdr(xdrbuf & buf)
    {
      bool ret = true;
      try
      {
        if( !key_.to_xdr( buf ) ) return false;

        buf << encrypted_creds_;

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

    bool xdr_response::from_xdr(xdrbuf & buf)
    {
      bool ret = true;
      try
      {
        if( !key_.from_xdr(buf) ) return false;

        buf >> encrypted_creds_;

        unsigned int is_compressed = 1;
        buf >> is_compressed;

        if( is_compressed != 0 && is_compressed != 1 ) return false;

        pbuf pb;
        buf >> pb;
        cert_.put_zdata( pb );
      }
      catch( csl::common::exc e )
      {
        ret = false;
      }
      return ret;
    }

    xdr_response & xdr_response::operator=(const xdr_response & other)
    {
      key_              = other.key_;
      encrypted_creds_  = other.encrypted_creds_;
      cert_             = other.cert_;
      return *this;
    }

    xdr_response::xdr_response() { }
    xdr_response::~xdr_response() { }

    xdr_response::xdr_response(const xdr_response & other) { *this = other; }
  }
}

/* EOF */
