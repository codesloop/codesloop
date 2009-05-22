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

      return true;
    }

    udp_chann::udp_chann()
    : use_exc_(true), sock_(-1), can_receive_(true), is_connected_(true), my_salt_(0), peer_salt_(0)
    {
    }
  };
};

/* EOF */
