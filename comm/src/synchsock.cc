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
#include "mutex.hh"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "common.h"
#include "synchsock.hh"

namespace csl
{
  using nthread::mutex;
  using nthread::scoped_mutex;

  namespace comm
  {
    bool synchsock::init(int sck)
    {
      socket_ = sck;

      struct sockaddr_in addr;
      bzero( &addr,sizeof(addr) );

      /* start a loopback UDP socket on a system chosen port */
      addr.sin_family       = AF_INET;
      addr.sin_addr.s_addr  = htonl(INADDR_LOOPBACK);
      addr.sin_port         = 0;

      if( siglstnr_ != -1 ) { shutdown(siglstnr_,2); close(siglstnr_); }
      if( sigsock_  != -1 ) { shutdown(sigsock_,2);  close(sigsock_);  }

      siglstnr_ = ::socket( AF_INET, SOCK_DGRAM, 0 );
      sigsock_  = ::socket( AF_INET, SOCK_DGRAM, 0 );

      if( siglstnr_ == -1 ) { THRC(exc::rs_socket_failed,exc::cm_synchsock,false); }
      if( sigsock_ == -1 )  { THRC(exc::rs_socket_failed,exc::cm_synchsock,false); }

      if( bind(siglstnr_,(struct sockaddr *)&addr, sizeof(addr)) )
      {
        ::close( siglstnr_ );
        THR(exc::rs_bind_failed,exc::cm_synchsock,false);
      }

      socklen_t len = sizeof(addr);

      if( ::connect(sigsock_, (struct sockaddr *)&addr, len) == -1 )
      {
          ::close(sigsock_);
          THRC(exc::rs_connect_failed,exc::cm_synchsock,false);
      }

      return true;
    }

    bool synchsock::wait_read(unsigned long ms)
    {
      struct timeval tv, *tvp;

      if( !ms ) tvp = 0;
      else
      {
        tvp = &tv;
        tv.tv_sec  = ms/1000;
        tv.tv_usec = (ms%1000)*1000;
      }

      fd_set fds;
      FD_ZERO( &fds );
      FD_SET( socket_, &fds );
      FD_SET( siglstnr_, &fds );

      int err = 0;

      {
        scoped_mutex m(mtx_);
        ::select( (socket_ > siglstnr_ ? (socket_+1) : (siglstnr_+1)), &fds, NULL, NULL, tvp );
      }

      if( err < 0 )
      {
        THRC(exc::rs_select_failed,exc::cm_synchsock,false);
      }
      else if( err == 0 )
      {
        /* timed out */
        return false;
      }
      else if( FD_ISSET( socket_, &fds ) )
      {
        /* the read descriptor is ready */

        /* check for the signal descriptor */
        if( FD_ISSET( siglstnr_, &fds ) )
        {
          char t;
          struct sockaddr_in addr;
          socklen_t len = sizeof(addr);

          if( ::recvfrom(siglstnr_, &t, 1, 0, (struct sockaddr *)&addr, &len) != 1 )
          {
            THRC(exc::rs_recv_failed,exc::cm_synchsock,false);
          }
        }

        return true;
      }
      else if( FD_ISSET( siglstnr_, &fds ) )
      {
        /* the read descriptor is not ready but a signal was received */
        char t;
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);

        if( ::recvfrom(siglstnr_, &t, 1, 0, (struct sockaddr *)&addr, &len) != 1 )
        {
          THRC(exc::rs_recv_failed,exc::cm_synchsock,false);
        }
        return false;
      }
      else
      {
        return false;
      }
    }

    bool synchsock::wait_write(unsigned long ms)
    {
      if( mtx_.is_locked() )
      {
        // other thread is doing something with it
        char t='?';
        if( ::send(sigsock_, &t, 1, 0 ) != 1 )
        {
          THRC(exc::rs_send_failed,exc::cm_synchsock,false);
        }
        //
        if( mtx_.lock(ms) == false )
        {
          return false;
        }
        else
        {
          mtx_.unlock();
          return true;
        }
      }
      else
      {
        return true;
      }
    }
  };
};

/* EOF */
