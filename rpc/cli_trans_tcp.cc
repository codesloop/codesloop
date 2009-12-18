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

#include <sys/time.h>

#include "codesloop/rpc/cli_trans_tcp.hh"
#include "codesloop/common/common.h"
#include "codesloop/comm/tcp_client.hh"
#include "codesloop/comm/exc.hh"

using namespace csl::comm;
using namespace csl::comm::tcp;

/**
  @file rpc/src/cli_trans_tcp.cc
  @brief implementation of codesloop interface descriptor
 */

namespace csl
{
  namespace rpc
  {

    void cli_trans_tcp::connect(const char * hostname, unsigned short port)
    {
      ENTER_FUNCTION();
      in_addr_t saddr = inet_addr(hostname);
      SAI peer;
      uint64_t server_time, client_time;
      struct timeval tv;
      struct timezone tz;


      ::memset( &peer,0,sizeof(peer) );
      ::memcpy( &(peer.sin_addr),&saddr,sizeof(saddr) );

      peer.sin_family  = AF_INET;
      peer.sin_port = htons( port );

      bool iret = client_.init( peer );

      if ( iret ) 
        CSL_DEBUGF( L"Client connected to %s:%d", hostname, port);
      else 
        THRNORET(csl::comm::exc::rs_connect_failed);

      ping( client_time, &server_time );
      gettimeofday(&tv,&tz);

      CSL_DEBUGF( L"Ping %fms, Pong: %fms", 
          double(server_time - client_time) / 1000,
          double((tv.tv_sec * 1000000 + tv.tv_usec) - client_time) / 1000
          );

      LEAVE_FUNCTION();
    }

    void cli_trans_tcp::wait(handle &)
    {
    }

    void cli_trans_tcp::send(handle & h, csl::common::pbuf * p )
    {
      uint8_t * data = new uint8_t[ p->size() ]; 

      p->copy_to( data, p->size());
      client_.write(data, p->size());

      delete data;
    }

  };
};

/* EOF */
