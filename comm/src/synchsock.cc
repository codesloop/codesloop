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

      bzero( &addr_,sizeof(addr_) );

      /* start a loopback UDP socket on a system chosen port */
      addr_.sin_family       = AF_INET;
      addr_.sin_addr.s_addr  = htonl(INADDR_LOOPBACK);
      addr_.sin_port         = 0;

      if( siglstnr_ != -1 ) { ::shutdown(siglstnr_,2); ::close(siglstnr_); }
      if( sigsock_  != -1 ) { ::shutdown(sigsock_,2);  ::close(sigsock_);  }

      siglstnr_ = ::socket( AF_INET, SOCK_STREAM, 0 );
      sigsock_  = ::socket( AF_INET, SOCK_STREAM, 0 );

      if( siglstnr_ == -1 ) { THRC(exc::rs_socket_failed,exc::cm_synchsock,false); }
      if( sigsock_ == -1 )  { THRC(exc::rs_socket_failed,exc::cm_synchsock,false); }

      if( ::bind(siglstnr_,(struct sockaddr *)&addr_, sizeof(addr_)) )
      {
        ::close( siglstnr_ ); siglstnr_ = -1;
        THR(exc::rs_bind_failed,exc::cm_synchsock,false);
      }

      socklen_t len = sizeof(addr_);
      getsockname( siglstnr_,(struct sockaddr *)&addr_,&len );

      if( ::listen(siglstnr_,10) )
      {
        ::close( siglstnr_ ); siglstnr_ = -1;
        THR(exc::rs_bind_failed,exc::cm_synchsock,false);
      }

      return true;
    }

    synchsock::~synchsock()
    {
      wait_write();
      {
        scoped_mutex m(mtx_);
        shutdown( siglstnr_, 2 );
        shutdown( sigsock_, 2 );
        ::close( siglstnr_ );
        ::close( sigsock_ );
        siglstnr_ = -1;
        sigsock_  = -1;
      }
    }

    bool synchsock::unlock()
    {
      return false;
    }

    bool synchsock::wait_read(unsigned long ms)
    {
      bool ret = false;

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

      int max = socket_;
      if( siglstnr_ > max ) max = siglstnr_;

      if( sigcli_ > 0 )
      {
        FD_SET( sigcli_, &fds );
        if( sigcli_ > max )  max = sigcli_;
      }

      int err = 0;

      printf("Start wait [%d]\n",mtx_.is_locked());
      ev_.clear_available();
      err = ::select( max+1, &fds, NULL, NULL, tvp );
      printf("End wait [%d] ret=[%d]\n",mtx_.is_locked(),err);

      if( err < 0 )
      {
        /* select error */
        THRC(exc::rs_select_failed,exc::cm_synchsock,false);
      }
      else if( err == 0 )
      {
        /* timed out */
        return false;
      }

      if( sigcli_ > 0 && FD_ISSET( sigcli_, &fds ) )
      {
        /* notify waiting writer */
        ev_.notify();

        /* notification received from the writer */
        char t;
        printf("start recv\n");
        if( (err=::recv( sigcli_, &t, 1, 0 )) != 1 )
        {
          THRC(exc::rs_recv_failed,exc::cm_synchsock,false);
        }
        printf("end recv: %d\n",err);
      }

      if( FD_ISSET( siglstnr_, &fds ) )
      {
        /* notify waiting writer */
        ev_.notify();

        /* new connection to be accepted */
        struct sockaddr_in cla;
        socklen_t sl = sizeof(cla);
        bzero( &cla,sizeof(cla) );

        printf("start accept\n");
        {
          scoped_mutex m(mtx_);
          sigcli_ = ::accept( siglstnr_,(struct sockaddr *)&cla,&sl );
        }

        printf("end accept: %d\n",sigcli_);

        if( sigcli_ <= 0 )
        {
          THRC(exc::rs_accept_failed,exc::cm_synchsock,false);
        }
      }

      if( FD_ISSET( socket_, &fds ) )
      {
        printf("data arrived\n");
        /* the read descriptor is ready */
        ret = true;
      }

      SleepSeconds(0);
      return ret;
    }

    bool synchsock::wait_write(unsigned long ms)
    {
      int err = 0;

      if( ev_.available_count() > 0 )
      {
        return ev_.wait(ms);
      }
      else
      {
        printf("sigcli: %d.\n",sigcli_);

        if( sigcli_ == -1 )
        {
          printf("start connect\n");

          if( ::connect(sigsock_, (struct sockaddr *)&addr_, sizeof(addr_)) == -1 )
          {
            ::close( sigsock_ );
            THRC(exc::rs_connect_failed,exc::cm_synchsock,false);
          }
          else
          {
            printf("connected.\n");
          }
        }
        else
        {
          char t='?';
          printf("start send\n");

          if( (err=::send(sigsock_, &t, 1, 0 )) != 1 )
          {
            printf("Send returned: %d\n",err);
            perror("S");
            THRC(exc::rs_send_failed,exc::cm_synchsock,false);
          }
          else
          {
            printf("sent.\n");
          }
        }
        return ev_.wait(ms);
      }
    }
  };
};

/* EOF */
