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

#include "codesloop/sec/bignum.hh"
#include "codesloop/common/csl_common.hh"

/**
  @file bignum.cc
  @brief big number helper w/ XDR features
 */

using csl::common::xdrbuf;

namespace csl
{
  namespace sec
  {
    bool bignum::to_xdr(xdrbuf & buf) const
    {
      bool ret = true;
      try
      {
        int32_t is_neg = (is_negative() == true ? 1 : 0);
        buf << is_neg;
        /* should be 0 or 1 */
        if( is_neg != 0 && is_neg != 1 ) return false;
        xdrbuf::bindata_t bin( data(), size() );
        buf << bin;
      }
      catch( csl::common::exc e )
      {
        ret = false;
      }
      return ret;
    }

    bool bignum::from_xdr(xdrbuf & buf)
    {
      bool ret = true;
      try
      {
        int32_t is_neg = 0;
        buf >> is_neg;
        uint64_t allocated=0;
        ret = buf.get_data( buf_, allocated, 2048 );
        if( ret ) is_negative((is_neg == 0 ? false : true));
        else      return false;
      }
      catch( csl::common::exc e )
      {
        ret = false;
      }
      return ret;
    }

    void bignum::print() const
    {
      const unsigned char * d = data();
      uint64_t len = size();
      PRINTF(L"BIGNUM[%d bytes]: neg[%d]: ", len, is_negative() );
      for( uint64_t i=0; i<len; ++i )
      {
        PRINTF( L"%02x", d[i] );
      }
      PRINTF(L"\n");
    }
  }
}

/* EOF */
