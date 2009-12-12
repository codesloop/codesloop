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

#include "codesloop/rpc/srv_trans_tcp.hh"
#include "codesloop/common/common.h"

/**
  @file rpc/src/srv_trans_tcp.cc
  @brief implementation of codesloop interface descriptor
 */

namespace csl
{
  namespace rpc
  {
    void srv_trans_tcp::listen(const char * hostname, unsigned short port )
    {
      ENTER_FUNCTION();
      in_addr_t   saddr = inet_addr(hostname);
      SAI         addr;

      ::memset( &addr,0,sizeof(addr) );
      ::memcpy( &(addr.sin_addr),&saddr,sizeof(saddr) );

      addr.sin_family  = AF_INET;
      addr.sin_port    = htons(port);

      lstnr l;
      l.init(*this, addr);
      l.start();
      CSL_DEBUGF( L"the listener has been started. wait 7 secs for connections" );
      assert( l.exit_event().wait(7000) == false );
      CSL_DEBUGF( L"the listener will be stopped on purpose after 7 seconds" );

      l.stop();

      LEAVE_FUNCTION();
    }

    bool srv_trans_tcp::on_connected( connid_t id,
                               const SAI & sai,
                               bfd & buf_fd )
    {
      ENTER_FUNCTION();
      bool ret = true;

      CSL_DEBUGF( L"on_connected(id:%lld, sai:(%s:%d), bfd)",
                   id, inet_ntoa(sai.sin_addr), ntohs(sai.sin_port));

      RETURN_FUNCTION(ret);
    }

    bool srv_trans_tcp::on_data_arrival( connid_t id,
                                  const SAI & sai,
                                  bfd & buf_fd )
    {
      ENTER_FUNCTION();
      bool ret = true;
      csl::common::pbuf buf;
      csl::common::read_res res;

      CSL_DEBUGF( L"on_data_arrival(id:%lld, sai:(%s:%d), bfd)",
                   id, inet_ntoa(sai.sin_addr), ntohs(sai.sin_port));

      if(buf_fd.size()>0) 
      {
        buf_fd.read_buf( res, buf_fd.size() );
        buf.append(  res.data(),  res.bytes() ) ;

        despatch(buf);
      }

      RETURN_FUNCTION(ret);
    }

    void srv_trans_tcp::on_disconnected( connid_t id,
                                  const SAI & sai )
    {
      ENTER_FUNCTION();
      CSL_DEBUGF( L"on_disconnected(id:%lld, sai:(%s:%d))",
                   id, inet_ntoa(sai.sin_addr), ntohs(sai.sin_port) );
      LEAVE_FUNCTION();
    }

  };
};

/* EOF */
