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
#include "csl_sec.hh"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include "common.h"

namespace csl
{
  using common::pbuf;
  using common::xdrbuf;

  namespace comm
  {
    udp_cli::udp_cli() : hello_sock_(-1), auth_sock_(-1), data_sock_(-1)
    {
      memset( &hello_addr_,0,sizeof(hello_addr_) );
      hello_addr_.sin_family        = AF_INET;
      hello_addr_.sin_addr.s_addr   = htonl(INADDR_LOOPBACK);

      auth_addr_ = data_addr_ = hello_addr_;
    }

    udp_cli::~udp_cli()
    {
      if( hello_sock_ > 0 ) ShutdownCloseSocket( hello_sock_ );
      if( auth_sock_ > 0  ) ShutdownCloseSocket( auth_sock_ );
      if( data_sock_ > 0 )  ShutdownCloseSocket( data_sock_ );

      hello_sock_ = auth_sock_ = data_sock_ = -1;
    }

    namespace
    {
      int init_sock(udp_cli::SAI & addr)
      {
        int sock = ::socket( AF_INET, SOCK_DGRAM, 0 );
        if( sock <= 0 ) { return sock; }

        socklen_t len = sizeof(addr);

        if( ::connect(sock, (struct sockaddr *)&addr, len) == -1 )
        {
          ShutdownCloseSocket(sock);
          return -1;
        }
        return sock;
      }
    }

    bool udp_cli::init()
    {
      if( hello_sock_ > 0 && auth_sock_ > 0 && data_sock_ > 0 ) return true;

      /* public_key has been set ? */
      if( public_key().is_empty() ) { THR(exc::rs_pubkey_empty,exc::cm_udp_cli,false); }

      hello_sock_ = init_sock( hello_addr_ );

      if( hello_sock_ <= 0 ) { THRC(exc::rs_socket_failed,exc::cm_udp_cli,false); }

      auth_sock_  = init_sock( auth_addr_ );

      if( auth_sock_ <= 0 ) { THRC(exc::rs_socket_failed,exc::cm_udp_cli,false); }

      data_sock_  = init_sock( data_addr_ );

      if( data_sock_ <= 0 ) { THRC(exc::rs_socket_failed,exc::cm_udp_cli,false); }

      return true;
    }

    bool udp_cli::hello( unsigned int timeout_ms )
    {
      int err = 0;

      if( !init() ) return false;

      /* prepare hello packet */
      unsigned int hello_len = 0;
      unsigned char * hello = pkt_.prepare_hello(hello_len);

      if( !hello_len || !hello ) { THR(exc::rs_pkt_error,exc::cm_udp_cli,false); }

      if( (err=::send( hello_sock_, (const char *)hello, hello_len , 0 )) != (int)hello_len )
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
      FD_SET(hello_sock_,&rfds);

      err = ::select(hello_sock_+1,&rfds,NULL,NULL,ptv);

      if( err > 0 )
      {
        err = ::recv(hello_sock_,(char *)pkt_.data(), pkt_.maxlen(), 0);
        //
        if( err > 0 )
        {
          if( pkt_.init_olleh( err ) == false )
          {
            THR(exc::rs_pkt_error,exc::cm_udp_cli,false);
          }
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

    namespace
    {
      static void gen_sess_key(std::string & k)
      {
        char s[64];
        csl_sec_gen_rand(s,sizeof(s));
        char   res[SHA1_HEX_DIGEST_STR_LENGTH];
        size_t ol = 0;
        csl_sec_sha1_conv( &s, sizeof(s), res, &ol );
        k = res;
      }
    }

    bool udp_cli::start( unsigned int timeout_ms )
    {
      int err = 0;

      if( !init() ) return false;

      if( server_info().public_key().is_empty() ) { THR(exc::rs_hello_nocall,exc::cm_udp_cli,false); }

      gen_sess_key(session_key_);
      csl_sec_gen_rand(&client_rand_,sizeof(client_rand_));

      if( server_info().need_login() && pkt_.login().size() == 0 ) { THR(exc::rs_need_login,exc::cm_udp_cli,false); }
      if( server_info().need_pass()  && pkt_.pass().size() == 0 )  { THR(exc::rs_need_pass,exc::cm_udp_cli,false); }

      pkt_.session_key(session_key_);
      memcpy( pkt_.rand(),&client_rand_,sizeof(client_rand_) );

      /* prepare auth packet */
      unsigned int auth_len = 0;
      unsigned char * auth = pkt_.prepare_uc_auth(auth_len);

      if( !auth_len || !auth ) { THR(exc::rs_pkt_error,exc::cm_udp_cli,false); }

      if( (err=::send( auth_sock_, (const char *)auth, auth_len , 0 )) != (int)auth_len )
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
      FD_SET(auth_sock_,&rfds);

      err = ::select(auth_sock_+1,&rfds,NULL,NULL,ptv);

      if( err > 0 )
      {
        err = ::recv(auth_sock_,(char *)pkt_.data(), pkt_.maxlen(), 0);
        //
        if( err > 0 )
        {
          if( pkt_.init_uc_htua( err ) == false )
          {
            THR(exc::rs_pkt_error,exc::cm_udp_cli,false);
          }

          memcpy(&server_rand_,pkt_.rand(),sizeof(server_rand_));
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

    bool udp_cli::send( unsigned char * data, unsigned int sz )
    {
      if( sz > pkt_.maxlen() )   { THR(exc::rs_too_big,exc::cm_udp_cli,false);    }
      if( !data || !sz )         { THR(exc::rs_null_param,exc::cm_udp_cli,false); }
      if( data_sock_ <= 0 )      { THR(exc::rs_not_inited,exc::cm_udp_cli,false); }

      // TODO

      return false;
    }

    bool udp_cli::recv( unsigned char * data, unsigned int & sz, unsigned int timeout_ms )
    {
      // TODO
      return false;
    }

  };
};

/* EOF */
