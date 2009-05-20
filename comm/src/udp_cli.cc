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

    udp_cli::udp_cli() : sock_(-1)
    {
      memset( &addr_,0,sizeof(addr_) );
      addr_.sin_family        = AF_INET;
      addr_.sin_addr.s_addr   = htonl(INADDR_LOOPBACK);
    }

    bool udp_cli::init()
    {
      if( sock_ > 0 ) return true;

      /* public_key has been set ? */
      if( public_key_.is_empty() ) { THR(exc::rs_pubkey_empty,exc::cm_udp_cli,false); }

      int sock = ::socket( AF_INET, SOCK_DGRAM, 0 );
      if( sock <= 0 ) { THRC(exc::rs_socket_failed,exc::cm_udp_cli,false); }

      socklen_t len = sizeof(addr_);

      if( ::connect(sock, (struct sockaddr *)&addr_, len) == -1 )
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

      udp_pkt pkt;
      pkt.own_privkey(private_key_);
      pkt.own_pubkey(public_key_);

      /* prepare hello packet */
      unsigned int hello_len = 0;
      unsigned char * hello = pkt.prepare_hello(hello_len);

      if( !hello_len || !hello ) { THR(exc::rs_pkt_error,exc::cm_udp_cli,false); }

      if( (err=::send( sock_, (const char *)hello, hello_len , 0 )) != (int)hello_len )
      {
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
        err = ::recv(sock_,(char *)pkt.data(), pkt.maxlen(), 0);
        //
        if( err > 0 )
        {
          if( pkt.init_olleh( err ) == false )
          {
            THR(exc::rs_pkt_error,exc::cm_udp_cli,false);
          }

          server_info_ = pkt.srv_info();
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
