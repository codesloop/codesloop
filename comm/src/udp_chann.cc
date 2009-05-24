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
#include "udp_chann.hh"
#include "csl_sec.h"
#include "common.h"

namespace csl
{
  namespace comm
  {
    bool udp_chann::send( )
    {
      return this->send( buf_ );
    }

    bool udp_chann::recv( unsigned int timeout_ms )
    {
      return this->recv( buf_,timeout_ms );
    }

    bool udp_chann::recv( udp_pkt::b1024_t & dta, unsigned int timeout_ms )
    {
      if( sock_ <= 0 )             { THR(exc::rs_not_inited,exc::cm_udp_chann,false);    }
      if( can_receive() == false ) { THR(exc::rs_recv_disabled,exc::cm_udp_chann,false); }

      /* wait for incoming packets */
      fd_set fds;
      FD_ZERO( &fds );
      FD_SET( sock_, &fds );

      struct timeval tv = { timeout_ms/1000, (timeout_ms%1000)*1000 };

      int err = ::select( sock_+1,&fds,NULL,NULL,&tv );

      if( err < 0 )       { THRC(exc::rs_select_failed,exc::cm_udp_chann,false); }
      else if( err == 0 ) { return false; }

      if( is_connected_ )
      {
        if( (err=::recv( sock_,  (char *)pkt_.data(), pkt_.maxlen(), 0 )) <= 0 )
        {
          THRC(exc::rs_send_failed,exc::cm_udp_chann,false);
        }
      }
      else
      {
        SAI         peer_addr;
        socklen_t   len = sizeof(peer_addr);

        if( (err=::recvfrom( sock_, (char *)pkt_.data(), pkt_.maxlen(), 0,
             (struct sockaddr *)&peer_addr, &len )) <= 0 )
        {
          THRC(exc::rs_send_failed,exc::cm_udp_chann,false);
        }

        peer_addr_ = peer_addr;
      }

      /* process incoming data packet */
      if( pkt_.init_data(err,dta) == false )
      {
        THR(exc::rs_pkt_error,exc::cm_udp_chann,false);
      }

      peer_salt_ = *(pkt_.newsalt());
      return true;
    }

    bool udp_chann::send( const udp_pkt::b1024_t & buffer )
    {
      unsigned long long newsalt = 0;
      csl_sec_gen_rand( &newsalt, sizeof(newsalt) );

      memcpy( pkt_.salt(),&peer_salt_,pkt_.maxsalt() );
      memcpy( pkt_.newsalt(),&newsalt,pkt_.maxsalt() );

      unsigned int pkt_size = 0;
      unsigned char * data = pkt_.prepare_data( buffer, pkt_size );

      if( data == NULL || pkt_size == 0 ) { THR(exc::rs_pkt_error,exc::cm_udp_chann,false); }

      int err = 0;

      if( is_connected_ )
      {
        if( (err=::send( sock_, data, pkt_size, 0 )) != (int)pkt_size )
        {
          THRC(exc::rs_send_failed,exc::cm_udp_chann,false);
        }
      }
      else
      {
        if( (err=::sendto( sock_, data, pkt_size, 0, (struct sockaddr *)&peer_addr_, sizeof(peer_addr_) )) != (int)pkt_size )
        {
          THRC(exc::rs_send_failed,exc::cm_udp_chann,false);
        }
      }
      peer_salt_ = newsalt;
      return true;
    }

    bool udp_chann::init( const udp_pkt & pk,
                          int sock,
                          SAI & peer_addr,
                          unsigned long long my_salt,
                          unsigned long long peer_salt )
    {
      /* internals */
      sock_       = sock;
      my_salt_    = my_salt;
      peer_salt_  = peer_salt;
      peer_addr_  = peer_addr;

      /* packet helper */
      pkt_.session_key(pk.session_key());
      pkt_.use_exc(use_exc());

      return true;
    }

    udp_chann::udp_chann()
    : use_exc_(true), sock_(-1), can_receive_(true), is_connected_(true), my_salt_(0), peer_salt_(0)
    {
    }
  };
};

/* EOF */
