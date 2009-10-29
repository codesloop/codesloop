/*
Copyright (c) 2008,2009, CodeSLoop Team

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

/**
   @file tcp_client.cc
   @brief @todo
 */

#ifndef DEBUG
#define DEBUG
#define DEBUG_ENABLE_INDENT
//#define DEBUG_VERBOSE
#endif /* DEBUG */

#include "codesloop/comm/tcp_client.hh"
#include "codesloop/comm/exc.hh"
#include "codesloop/common/logger.hh"

namespace csl
{
  namespace comm
  {
    namespace tcp
    {
      client::client() : use_exc_(false)
      {
        in_addr_t saddr = inet_addr("127.0.0.1");
        ::memcpy( &(own_addr_.sin_addr),&saddr,sizeof(saddr) );
        own_addr_.sin_port   = 0;
        own_addr_.sin_family = AF_INET;
        peer_addr_           = own_addr_;
      }

      bool client::init(SAI address)
      {
        ENTER_FUNCTION();
        CSL_DEBUGF( L"init(%s:%d)",inet_ntoa(address.sin_addr),ntohs(address.sin_port));

        int sock = ::socket( AF_INET, SOCK_STREAM, 0 );
        if( sock < 0 ) { THRC(exc::rs_socket_failed,false); }

        int err = ::connect( sock, reinterpret_cast<struct sockaddr *>(&address), sizeof(SAI) );
        if( err < 0 ) { CloseSocket(sock); THRC(exc::rs_connect_failed,false); }
        CSL_DEBUGF( L"connected to (%s:%d)",inet_ntoa(address.sin_addr),ntohs(address.sin_port));

        socklen_t slen = sizeof(SAI);
        err = getsockname( sock, reinterpret_cast<struct sockaddr *>(&own_addr_), &slen );
        if( err < 0 ) { CloseSocket(sock); THRC(exc::rs_getsockname_failed,false); }

        CSL_DEBUGF( L"own address is (%s:%d)",inet_ntoa(own_addr_.sin_addr),ntohs(own_addr_.sin_port));

        peer_addr_ = address;

        bfd_.init( sock );
        RETURN_FUNCTION( true );
      }
    }
  }
}

/* EOF */
