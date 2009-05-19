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
#include "udp_srv.hh"
#include "udp_srv_entry.hh"
#include "mutex.hh"
#include "common.h"
#include <sys/types.h>
#include <string.h>

namespace csl
{
  using nthread::mutex;
  using nthread::scoped_mutex;

  namespace comm
  {
    udp_srv_entry::udp_srv_entry(udp_srv & srv)
      : srv_(&srv), socket_(-1), stop_me_(false), use_exc_(srv.use_exc()) {}

    bool udp_srv_entry::stop_me()
    {
      bool ret;
      {
        scoped_mutex m(mtx_);
        ret = stop_me_;
      }
      return ret;
    }

    void udp_srv_entry::stop_me(bool yesno)
    {
      scoped_mutex m(mtx_);
      stop_me_ = yesno;
    }

    bool udp_srv_entry::start()
    {
      if( socket_ != -1 ) { ShutdownCloseSocket(socket_); }

      int sock = ::socket( AF_INET, SOCK_DGRAM, 0 );
      if( sock <= 0 ) { THR(exc::rs_socket_failed,exc::cm_udp_srv,false); }

      /* copy addr */
      SAI addr;
      memcpy( &addr,&addr_,sizeof(addr) );

      if( ::bind(sock,(struct sockaddr *)&addr, sizeof(addr)) )
      {
        ShutdownCloseSocket( sock );
        THRC(exc::rs_bind_failed,exc::cm_udp_srv,false);
      }

      /* check internal address */
      socklen_t len = sizeof(addr);

      if( ::getsockname(sock,(struct sockaddr *)&addr,&len) )
      {
        ShutdownCloseSocket( sock );
        THRC(exc::rs_getsockname_failed,exc::cm_udp_srv,false);
      }

      ::memcpy( &addr_,&addr,len );
      socket_ = sock;

      return true;
    }

    udp_srv_entry::~udp_srv_entry()
    {
      if( socket_ > 0 ) ShutdownCloseSocket( socket_ );
      socket_ = -1;
    }

    const udp_srv_entry::SAI & udp_srv_entry::addr() const
    {
      return addr_;
    }

    void udp_srv_entry::addr(const SAI & a)
    {
      addr_ = a;
    }

    int udp_srv_entry::socket() const
    {
      return socket_;
    }

    udp_srv & udp_srv_entry::srv()
    {
      return *srv_;
    }

    bool udp_srv_entry::use_exc() const
    {
      return use_exc_;
    }
  };
};

/* EOF */
