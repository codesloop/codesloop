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

#include "codesloop/comm/exc.hh"
#include "codesloop/comm/udp_recvr.hh"
#include "codesloop/nthread/mutex.hh"
#include "codesloop/common/common.h"

namespace csl
{
  using namespace nthread;

  namespace comm
  {
    namespace udp
    {
      bool recvr::start( unsigned int min_threads,
                         unsigned int max_threads,
                         unsigned int timeout_ms,
                         unsigned int attempts,
                         msg_handler & cb,
                         int tsock )
      {
        /* init thread pool and handlers */
        cb.set_msgs(msgs_);

        if( !thread_pool_.init( min_threads, max_threads,
             timeout_ms, attempts,
             msgs_.ev_, cb ) ) return false;

        /* copy addr to temporary */
        SAI addrv = this->addr();
        socklen_t len = sizeof(addrv);

        /* the caller may supply a preinited socket, so we check that first
           we assume that socket don't need to be bound */

        if( tsock != -1 )
        {
          /* check own socket */
          if( socket_ != -1 && socket_ != tsock ) { ShutdownCloseSocket(socket_); }

          int sock = tsock;

          /* check socket address */
          if( ::getsockname(sock,reinterpret_cast<struct sockaddr *>(&addrv),&len) )
          {
            ShutdownCloseSocket( sock );
            THRC(exc::rs_getsockname_failed,false);
          }

          /* copy the address */
          this->addr( addrv );
          socket_ = sock;

          return true;
        }
        else
        {
          /* init udp */
          if( socket_ != -1 ) { ShutdownCloseSocket(socket_); }

          int sock = ::socket( AF_INET, SOCK_DGRAM, 0 );
          if( sock <= 0 ) { THRC(exc::rs_socket_failed,false); }

          /* bind socket */
          if( ::bind(sock,reinterpret_cast<struct sockaddr *>(&addrv), sizeof(addrv)) )
          {
            ShutdownCloseSocket( sock );
            THRC(exc::rs_bind_failed,false);
          }

          /* check internal address */
          if( ::getsockname(sock,reinterpret_cast<struct sockaddr *>(&addrv),&len) )
          {
            ShutdownCloseSocket( sock );
            THRC(exc::rs_getsockname_failed,false);
          }

          /* copy the address back, in case of OS chosen port/address */
          this->addr( addrv );
          socket_ = sock;
        }

        return true;
      }

      void recvr::operator()(void)
      {
        unsigned long timeout_ms    = thread_pool_.timeout();
        unsigned long timeout_sec   = timeout_ms/1000;
        unsigned long timeout_usec  = (timeout_ms%1000)*1000;

        /* packet loop */
        while( stop_me() == false )
        {
          /* wait for incoming packets */
          fd_set fds;
          FD_ZERO( &fds );
          FD_SET( socket_, &fds );
          struct timeval tv = { timeout_sec, timeout_usec };

          /* wait for new packet to arrive */
          int err = ::select( socket_+1,&fds,NULL,NULL,&tv );
          int recvd = 0;

          if( err < 0 )       { THRNORET(exc::rs_select_failed); break; }
          else if( err == 0 ) { continue; }

          /* temporary lock messages for getting an entry from it */
          msg * tm = 0;
          {
            scoped_mutex mm(msgs_.mtx_);
            tm = &(msgs_.prepare());
          }
          msg & m(*tm);

          socklen_t len = sizeof(m.sender_);

          /* receive packet */
          recvd = ::recvfrom( socket_, reinterpret_cast<char *>(m.data_), m.max_len(), 0,
            reinterpret_cast<struct sockaddr *>(&(m.sender_)), &len );

          if( recvd < 0 ) { THRNORET(exc::rs_recv_failed); break; }
          else if( recvd == 0 )
          {
            m.size_ = 0;
            /* temporary lock messages for putting back unneccessary entry */
            {
              scoped_mutex mm(msgs_.mtx_);
              msgs_.rollback( m );
            }
            continue;
          }
          else
          {
            m.size_    = recvd;
            /* temporary lock messages for committing new message */
            {
              scoped_mutex mm(msgs_.mtx_);
            /*
              ** when commit() is called, the waiting threads in the thread pool will be
              ** signaled to check for the new message arrived
            */
              msgs_.commit( m );
            }
          }
        }

        if( thread_pool_.graceful_stop() == false )
        {
          thread_pool_.unpolite_stop();
        }
      }

      bool recvr::stop()
      {
        stop_me(true);
        if( thread_pool_.graceful_stop() == false )
        {
          return thread_pool_.unpolite_stop();
        }
        else
        {
          return true;
        }
      }

      recvr::~recvr()
      {
        if( socket_ > 0 ) ShutdownCloseSocket( socket_ );
        socket_ = -1;
      }

      recvr::recvr() : socket_(-1), stop_me_(false), debug_(false)
      {
      }

      bool recvr::stop_me()
      {
        bool ret = false;
        {
          scoped_mutex m(mtx_);
          ret = stop_me_;
        }
        return ret;
      }

      void recvr::stop_me(bool yesno)
      {
        scoped_mutex m(mtx_);
        stop_me_ = yesno;
      }

      SAI recvr::addr()
      {
        SAI ret;
        {
          scoped_mutex m(mtx_);
          ret = addr_;
        }
        return ret;
      }

      void recvr::addr(const SAI & a)
      {
        scoped_mutex m(mtx_);
        addr_ = a;
      }
    }
  };
};

/* EOF */
