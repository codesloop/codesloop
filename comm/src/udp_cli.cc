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

#include "exc.hh"
#include "udp_pkt.hh"
#include "xdrbuf.hh"
#include "pbuf.hh"
#include "udp_cli.hh"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "common.h"

namespace csl
{
  using common::pbuf;
  using common::xdrbuf;

  namespace comm
  {

    bool udp_cli::init()
    {
      if( sock_ > 0 ) return true;

      /* host name has been set ? */
      if( host_.empty() ) { THR(exc::rs_host_not_set,exc::cm_udp_cli,false); }

      /* port has been set ? */
      if( port_ == 0 ) { THR(exc::rs_port_not_set,exc::cm_udp_cli,false); }

      int sock = ::socket( AF_INET, SOCK_DGRAM, 0 );
      if( sock <= 0 ) { THRC(exc::rs_socket_failed,exc::cm_udp_cli,false); }

      struct sockaddr_in srv_addr;
      memset( &srv_addr,0,sizeof(srv_addr) );

      srv_addr.sin_family = AF_INET;
      srv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // TODO resolve hostname
      srv_addr.sin_port = htons(port_);

      socklen_t len = sizeof(srv_addr);

      if( ::connect(sock, (struct sockaddr *)&srv_addr, len) == -1 )
      {
        ShutdownCloseSocket(sock);
        THRC(exc::rs_connect_failed,exc::cm_udp_cli,false);
      }

      sock_ = sock;
      return true;
    }

    bool udp_cli::hello( unsigned int timeout_ms )
    {
      int err = 0;

      if( !init() ) return false;

      /* public_key has been set ? */
      if( public_key_.is_empty() ) { THR(exc::rs_pubkey_empty,exc::cm_udp_cli,false); }

      pbuf pb;
      xdrbuf xb(pb);

      try
      {
        /* compile packet */
        xb << (int32_t)udp_pkt::hello_p;
        if( !public_key_.to_xdr(xb) ) { THR(exc::rs_xdr_error,exc::cm_udp_cli,false); }
      }
      catch( common::exc e)
      {
        THR(exc::rs_xdr_error,exc::cm_udp_cli,false);
      }

      /* should not happen */
      if( pb.size() == 0 ) { THR(exc::rs_internal_error,exc::cm_udp_cli,false); }

      pbuf::const_iterator it = pb.const_begin();

      if( (err=::send( sock_, (const char *)(*it)->data_, (*it)->size_ , 0 )) != (int)(*it)->size_ )
      {
        perror("write");
        fprintf(stderr,"send(%d, '%d bytes') => %d\n",sock_,(*it)->size_,err);
        THRC(exc::rs_send_failed,exc::cm_udp_cli,false);
      }

      fd_set rfds;
      struct timeval tv = { 0,0 };
      struct timeval * ptv = 0;

      if( timeout_ms )
      {
        ptv = &tv;
        tv.tv_sec  = timeout_ms/1000;
        tv.tv_usec = (timeout_ms%1000)*1000;
      }

      FD_ZERO(&rfds);
      FD_SET(sock_,&rfds);

      err = ::select(sock_+1,&rfds,NULL,NULL,ptv);

      if( err > 0 )
      {
        unsigned char tmp[65536];
        err = ::recv(sock_,(char *)tmp,sizeof(tmp),0);
        //
        if( err > 0 )
        {
           // TODO
          return true;
        }
        else
        {
          THRC(exc::rs_recv_failed,exc::cm_udp_cli,false);
        }
      }
      else if( err == 0 )
      {
        /* timed out */
        THR(exc::rs_timeout,exc::cm_udp_cli,false);
      }
      else
      {
        /* error */
        THRC(exc::rs_select_failed,exc::cm_udp_cli,false);
      }
    }

    bool udp_cli::start( unsigned int timeout_ms )
    {
      // TODO
      return false;
    }

    bool udp_cli::send( pbuf & pb, bool synched, unsigned int timeout_ms )
    {
      // TODO
      return false;
    }

    bool udp_cli::recv( pbuf & pb, unsigned int timeout_ms )
    {
      // TODO
      return false;
    }

  };
};

/* EOF */
