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
#include "udp.hh"
#include "mutex.hh"
#include "common.h"

namespace csl
{
  using namespace nthread;

  namespace comm
  {
    bool udp::recvr::start( const SAI & taddr,
                            unsigned int min_threads,
                            unsigned int max_threads,
                            unsigned int timeout_ms,
                            unsigned int attempts,
                            msg_handler & cb )
    {
      /* init thread pool and handlers */
      cb.set_msgs(msgs_);

      if( thread_pool_.init( min_threads, max_threads,
                             timeout_ms, attempts,
                             msgs_.ev_, cb ) == false ) return false;

      /* init udp */
      if( socket_ != -1 ) { ShutdownCloseSocket(socket_); }

      int sock = ::socket( AF_INET, SOCK_DGRAM, 0 );
      if( sock <= 0 ) { THR(exc::rs_socket_failed,exc::cm_udp_recvr,false); }

      /* copy addr to temporary */
      SAI addr;
      memcpy( &addr,&taddr,sizeof(taddr) );

      /* bind socket */
      if( ::bind(sock,(struct sockaddr *)&addr, sizeof(addr)) )
      {
        ShutdownCloseSocket( sock );
        THRC(exc::rs_bind_failed,exc::cm_udp_recvr,false);
      }

      /* check internal address */
      socklen_t len = sizeof(addr);

      if( ::getsockname(sock,(struct sockaddr *)&addr,&len) )
      {
        ShutdownCloseSocket( sock );
        THRC(exc::rs_getsockname_failed,exc::cm_udp_recvr,false);
      }

      /* copy the address back, in case of OS chosen port/address */
      memcpy( &addr_,&addr,len );
      socket_ = sock;

      return true;
    }

    void udp::recvr::operator()(void)
    {
      unsigned int timeout_ms = thread_pool_.timeout();

      /* packet loop */
      while( stop_me() == false )
      {
        /* wait for incoming packets */
        fd_set fds;
        FD_ZERO( &fds );
        FD_SET( socket_, &fds );
        struct timeval tv = { timeout_ms/1000, (timeout_ms%1000)*1000 };

        int err = ::select( socket_+1,&fds,NULL,NULL,&tv );
        int recvd = 0;

        if( err < 0 )       { THRNORET(exc::rs_select_failed,exc::cm_udp_recvr); break; }
        else if( err == 0 ) { continue; }

        /* temporary lock messages for getting an entry from it */
        msgs_.mtx_.lock();
        msg & m(msgs_.prepare());
        msgs_.mtx_.unlock();

        socklen_t len = sizeof(m.sender_);

        /* receive packet */
        recvd = recvfrom( socket_, (char *)m.data_, m.max_len(), 0, (struct sockaddr *)&(m.sender_), &len );

        if( recvd < 0 ) { THRNORET(exc::rs_recv_failed,exc::cm_udp_recvr); break; }
        else if( recvd == 0 )
        {
          m.size_ = 0;
          {
            scoped_mutex mm(msgs_.mtx_);
            msgs_.rollback( m );
          }
          continue;
        }
        else
        {
          m.size_ = recvd;
          {
            scoped_mutex mm(msgs_.mtx_);
            msgs_.commit( m );
          }
        }
      }
    }

    udp::recvr::~recvr()
    {
      if( socket_ > 0 ) ShutdownCloseSocket( socket_ );
      socket_ = -1;
    }

    udp::recvr::recvr() : socket_(-1), stop_me_(false), use_exc_(false)
    {
    }

    bool udp::recvr::use_exc()
    {
      bool ret = false;
      {
        scoped_mutex m(mtx_);
        ret = use_exc_;
      }
      return ret;
    }

    void udp::recvr::use_exc(bool yesno)
    {
      scoped_mutex m(mtx_);
      use_exc_ = yesno;
    }

    bool udp::recvr::stop_me()
    {
      bool ret = false;
      {
        scoped_mutex m(mtx_);
        ret = stop_me_;
      }
      return ret;
    }

    void udp::recvr::stop_me(bool yesno)
    {
      scoped_mutex m(mtx_);
      stop_me_ = yesno;
    }
  };
};

/* EOF */
