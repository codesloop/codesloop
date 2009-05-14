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

#include "udp_pkt.hh"
#include "pbuf.hh"
#include "xdrbuf.hh"
#include "udp_hello.hh"
#include "common.h"

namespace csl
{
  using common::pbuf;
  using common::xdrbuf;

  namespace comm
  {
    bool udp_hello::init( const unsigned char * buf,
                          unsigned int size )
    {
      if( !buf || !size )          { THR(exc::rs_empty_buffer,exc::cm_udp_hello,false); }

      try
      {
        pbuf pb;
        pb.append(buf,size);
        xdrbuf xb(pb);

        int32_t packet_type;
        xb >> packet_type;

        if( packet_type != udp_pkt::hello_p )
        {
          THR(exc::rs_internal_error,exc::cm_udp_hello,false);
        }

        if( !public_key_.from_xdr(xb) )
        {
          THR(exc::rs_xdr_error,exc::cm_udp_hello,false);
        }
      }
      catch( common::exc e )
      {
        THR(exc::rs_xdr_error,exc::cm_udp_hello,false);
      }
      return true;
    }
  };
};

/* EOF */
