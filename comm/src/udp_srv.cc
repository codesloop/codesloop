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

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include "udp_srv.hh"

namespace csl
{
  namespace comm
  {
    bool udp_srv::start()
    {
      server_thread_.set_entry(server_entry_);

      /* already started ( TODO may be finished too ) */
      if( server_thread_.is_started() ) return false;

      /* host name has been set ? */
      if( host_.empty() ) return false;

      /* port has been set ? */
      if( port_ == 0 ) return false;

      /* private_key has been set ? */
      if( private_key_.is_empty() ) return false;

      /* server_info_.public_key has been set ? */
      if( server_info_.public_key().is_empty() ) return false;

      /* is already running ? */
      if( server_entry_.socket_ != -1 ) return false;

      int sock = socket( AF_INET, SOCK_DGRAM, 0 );
      if( sock <= 0 ) return false;

      struct sockaddr_in srv_addr;
      bzero( &srv_addr,sizeof(srv_addr) );

      srv_addr.sin_family = AF_INET;
      srv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // TODO resolve hostname
      srv_addr.sin_port = htons(port_);

      if( bind(sock,(struct sockaddr *)&srv_addr, sizeof(srv_addr)) == -1 )
      {
        close( sock );
        return false;
      }

      server_entry_.socket_ = sock;

      /* launch thread */
      if( !server_thread_.start() ) return false;

      /* wait for thread to be started */
      return server_thread_.start_event().wait( 10000 );
    }

    void udp_srv::server_entry::operator()(void)
    {
      struct sockaddr_in cliaddr;
      socklen_t          len = sizeof(cliaddr);
      int                recvd;
      unsigned char      buffer[65536];

      if( socket_ <= 0 ) return;

      // TODO
      // wait packets
      while( true )
      {
        bzero( buffer,sizeof(buffer) );
        recvd = recvfrom( socket_, buffer, sizeof(buffer), 0, (struct sockaddr *)&cliaddr, &len );
        fprintf( stderr,"Received %d bytes [%s]\n",recvd,buffer);
        if( recvd <= 0 ) { perror("What: "); return; }
      }
    }

    udp_srv::server_entry::~server_entry()
    {
      if( socket_ > 0 ) close( socket_ );
      socket_ = -1;
    }
  };
};

/* EOF */
