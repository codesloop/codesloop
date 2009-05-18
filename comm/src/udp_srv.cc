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
#include "pbuf.hh"
#include "xdrbuf.hh"
#include "udp_srv.hh"
#include "udp_pkt.hh"
#include "udp_hello.hh"
#include "udp_olleh.hh"
#include "common.h"
#include <sys/types.h>
#include <string.h>


namespace csl
{
  namespace comm
  {
    using common::pbuf;
    using common::xdrbuf;

    void udp_srv::server_entry::operator()(void)
    {
      struct sockaddr_in   cliaddr;
      socklen_t            len = sizeof(cliaddr);
      int                  recvd;
      unsigned char        buffer[65536];

      /* hello reply : olleh */

      if( socket_ <= 0 ) return;

      // TODO
      // wait packets
      while( true )
      {
        recvd = recvfrom( socket_, (char *)buffer, sizeof(buffer), 0, (struct sockaddr *)&cliaddr, &len );
        fprintf( stderr,"Received %d bytes [%s]\n",recvd,buffer);

        try
        {
          if( recvd <= 0 ) { perror("What: "); return; }
          else if( recvd > 8 )
          {
            pbuf pb;
            pb.append( buffer,8 );
            xdrbuf xb(pb);

            int32_t packet_type;
            xb >> packet_type;

            switch( packet_type )
            {
              case udp_pkt::hello_p:
              { // TODO : this may go to a separate thread
                udp_hello hello_pkt;

                /* decode packet */
                if( hello_pkt.init(buffer,recvd) == false ) break;

                /* check peer key */
                if( srv_->valid_key_cb_ &&
                    (*(srv_->valid_key_cb_))(hello_pkt.public_key()) == false ) break;

                /* gather packet data */
                udp_srv_info   inf(srv_->server_info());
                bignum         private_key(srv_->private_key());
                string         peer_host(inet_ntoa(cliaddr.sin_addr));

                /* hello callback */
                if( srv_->hello_cb_ &&
                    (*(srv_->hello_cb_))( hello_pkt.public_key(),
                                          peer_host,
                                          cliaddr.sin_port,
                                          inf,
                                          private_key) == false )  break;

                /* compile packet */
                udp_olleh olleh_pkt;

                if( olleh_pkt.init(hello_pkt,inf,private_key) == false ) break;

                /* send data back */
                // TODO
              }

              case udp_pkt::unicast_auth_p:
              case udp_pkt::multicast_auth_p:
              case udp_pkt::data_p:
              case udp_pkt::ack_rand_p:
                //
              case udp_pkt::olleh_p:
              case udp_pkt::result_p:
              case udp_pkt::unicast_auth_resp_p:
              case udp_pkt::multicast_auth_resp_p:
              default:
                break; // TODO
            };
          }
        }
        catch( comm::exc e )
        {
          std::string es;
          e.to_string(es);
          fprintf(stderr,"E! %s\n",es.c_str());
        }
      }
    }

    udp_srv::server_entry::~server_entry()
    {
      if( socket_ > 0 ) ShutdownCloseSocket( socket_ );
      socket_ = -1;
    }

    bool udp_srv::start()
    {
      server_thread_.set_entry(server_entry_);

      /* already started ( TODO may be finished too ) */
      if( server_thread_.is_started() ) { THR(exc::rs_already_started,exc::cm_udp_srv,false); }

      /* host name has been set ? */
      if( host_.empty() ) { THR(exc::rs_host_not_set,exc::cm_udp_srv,false); }

      /* port has been set ? */
      if( port_ == 0 ) { THR(exc::rs_port_not_set,exc::cm_udp_srv,false); }

      /* private_key has been set ? */
      if( private_key_.is_empty() ) { THR(exc::rs_privkey_empty,exc::cm_udp_srv,false); }

      /* server_info_.public_key has been set ? */
      if( server_info_.public_key().is_empty() ) { THR(exc::rs_pubkey_empty,exc::cm_udp_srv,false); }

      /* is already running ? */
      if( server_entry_.socket_ != -1 ) { THR(exc::rs_already_started,exc::cm_udp_srv,false); }

      int sock = socket( AF_INET, SOCK_DGRAM, 0 );
      if( sock <= 0 ) { THR(exc::rs_socket_failed,exc::cm_udp_srv,false); }

      struct sockaddr_in srv_addr;
      memset( &srv_addr,0,sizeof(srv_addr) );

      srv_addr.sin_family = AF_INET;
      srv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // TODO resolve hostname
      srv_addr.sin_port = htons(port_);

      if( bind(sock,(struct sockaddr *)&srv_addr, sizeof(srv_addr)) )
      {
        ShutdownCloseSocket( sock );
        THR(exc::rs_bind_failed,exc::cm_udp_srv,false);
      }

      server_entry_.socket_ = sock;

      /* launch thread */
      if( !server_thread_.start() ) { THR(exc::rs_thread_start,exc::cm_udp_srv,false); }

      /* wait for thread to be started */
      return server_thread_.start_event().wait( 10000 );
    }
  };
};

/* EOF */
