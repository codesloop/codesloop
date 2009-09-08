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
   @file t__mt_udp.cc
   @brief Tests to verify multithreading and udp socket interworking
 */

#include "initcomm.hh"
#include "thread.hh"
#include "event.hh"
#include "mutex.hh"
#include "test_timer.h"
#include "common.h"
#include <assert.h>
#include <list>

using namespace csl::nthread;
using namespace csl::comm;

/** @brief contains tests related to comm interface */
namespace test_mt_udp {

  class entry : public thread::callback
  {
    protected:
      bool stop_me_;
      int  socket_;

    public:
      virtual void operator()(void) = 0;

      entry() : stop_me_(false), socket_(-1)
      {
        socket_ = ::socket( AF_INET, SOCK_DGRAM, 0 );
        assert( socket_ > 0 );
      }

      virtual ~entry()
      {
        if( socket_ > 0 ) { ShutdownCloseSocket(socket_); }
        socket_ = -1;
      }

      virtual inline void stop() { stop_me_ = true; }
  };

  class writer : public thread::callback
  {
    public:
      bool    stop_me_;
      int     socket_;
      int     sent_;
      mutex   mtx_;
      event   ev_;

      typedef struct sockaddr_in SA;
      typedef std::list<SA> salist_t;

      salist_t sl_;

      writer() : stop_me_(false), socket_(-1), sent_(0) {}

      virtual void operator()(void)
      {
        while( stop_me_ == false )
        {
          if( ev_.wait(100) )
          {
            bool do_send = false;
            SA sa;
            {
              scoped_mutex m(mtx_);
              if( sl_.size() > 0 )
              {
                sa = sl_.front();
                sl_.pop_front();
                do_send = true;
              }
            }

            char t='?';
            if( do_send && ::sendto( socket_,&t,1,0,reinterpret_cast<const struct sockaddr *>(&sa),sizeof(sa) ) == 1 )
            {
              ++sent_;
            }
          }
        }
      }

      void add(struct sockaddr_in & addr)
      {
        {
          scoped_mutex m(mtx_);
          sl_.push_back(addr);
        }
        ev_.notify();
      }
  };

  class reader : public thread::callback
  {
    public:
      bool     stop_me_;
      int      socket_;
      writer * w_;
      int      recvd_;
      int      timeout_;

      reader() : stop_me_(false), socket_(-1), w_(0), recvd_(0), timeout_(0) {}

      virtual void operator()(void)
      {
        while( stop_me_ == false && socket_ > 0 )
        {
          char t;
          struct sockaddr_in addr;
          socklen_t len=sizeof(addr);
          memset( &addr,0,sizeof(addr) );

          fd_set fds;
          FD_ZERO( &fds );
          FD_SET( socket_,&fds );

          struct timeval tv = { 0,100000 };

          int err = 0;

          if( (err=::select(socket_+1,&fds,NULL,NULL,&tv)) > 0 )
          {
            if( (err=::recvfrom( socket_,&t,1,0,reinterpret_cast<struct sockaddr *>(&addr),&len )) == 1 )
            {
              ++recvd_;
              w_->add( addr );
            }
            else
            {
              /* recv error */
              break;
            }
          }
          else if( err == 0 )
          {
            ++timeout_;
          }
          else
          {
            /* select error */
            break;
          }
        }
      }
  };

  class server : public entry
  {
    public:
      void init()
      {
        memset( &addr_,0,sizeof(addr_) );
        addr_.sin_family       = AF_INET;
        addr_.sin_addr.s_addr  = htonl(INADDR_LOOPBACK);
        addr_.sin_port         = 0;

        socklen_t len = sizeof(addr_);

        assert( ::bind(socket_,reinterpret_cast<struct sockaddr *>(&addr_), sizeof(addr_)) == 0 );
        assert( ::getsockname(socket_,reinterpret_cast<struct sockaddr *>(&addr_),&len) == 0 );
      }

      virtual struct sockaddr_in & addr() { return addr_; }

      virtual void operator()(void)
      {
        reader_entry_.socket_ = socket_;
        writer_entry_.socket_ = socket_;
        reader_entry_.w_      = &writer_entry_;

        reader_thread_.set_entry( reader_entry_ );
        writer_thread_.set_entry( writer_entry_ );

        reader_thread_.start();
        writer_thread_.start();

        while( stop_me_ == false )
        {
          SleepSeconds(1);
        }

        printf("Server received: %d\n",reader_entry_.recvd_);
        printf("Server sent:     %d\n",writer_entry_.sent_);
        printf("Server timeout:  %d\n",reader_entry_.timeout_);

        writer_entry_.stop_me_ = true;
        reader_entry_.stop_me_ = true;

        printf("0, Writer\n");
        writer_entry_.ev_.notify();
        printf("1, Writer\n");
        writer_thread_.exit_event().wait(2000);
        printf("2, Reader\n");
        if( reader_thread_.exit_event().wait(2000) == false )
        {
           printf("X, Closing sockets\n");
           ShutdownCloseSocket( socket_ );
           socket_ = -1;
        }
        printf("3, End\n");
      }

    private:
      struct sockaddr_in addr_;
      thread reader_thread_;
      thread writer_thread_;
      reader reader_entry_;
      writer writer_entry_;
  };

  class client : public entry
  {
    public:
      void init(struct sockaddr_in & addr)
      {
        memcpy( &addr_,&addr,sizeof(addr) );

        assert( ::connect( socket_, reinterpret_cast<struct sockaddr *>(&addr_),sizeof(addr_) ) == 0 );
      }

      virtual void operator()(void)
      {
        while( stop_me_ == false )
        {
          char t='0';
          int err;

          fd_set fds;
          FD_ZERO( &fds );
          FD_SET( socket_,&fds );

          struct timeval tv = { 0,600000 };

          if( (err=send( socket_,&t,1,0 )) == 1 ) { ++sent_; }
          else
          {
            printf("Send error.\n");
            break;
          }

          if( (err=::select(socket_+1,&fds,NULL,NULL,&tv)) > 0 )
          {
            if( (err=::recv( socket_,&t,1,0 )) == 1 )
            {
              ++recvd_;
            }
            else
            {
              /* recv error */
              break;
            }
          }
          else if( err == 0 )
          {
            ++timeout_;
          }
          else
          {
            printf("Select error.\n");
            break;
          }
        }
        printf("%p client sent:     %d\n",this,sent_);
        printf("%p client received: %d\n",this,recvd_);
        printf("%p client timeout:  %d\n",this,timeout_);
      }

      client() : sent_(0), recvd_(0), timeout_(0) {}

    private:
      struct sockaddr_in addr_;
      int sent_;
      int recvd_;
      int timeout_;
  };

  void basic()
  {
    server server_entry;
    client client_entry1;
    client client_entry2;
    client client_entry3;
    client client_entry4;

    thread srv,cli1,cli2,cli3,cli4;

    /* initialize threads */
    server_entry.init();
    client_entry1.init(server_entry.addr());
    client_entry2.init(server_entry.addr());
    client_entry3.init(server_entry.addr());
    client_entry4.init(server_entry.addr());

    srv.set_entry( server_entry );
    cli1.set_entry( client_entry1 );
    cli2.set_entry( client_entry2 );
    cli3.set_entry( client_entry3 );
    cli4.set_entry( client_entry4 );

    /* launch threads */
    srv.start();
    cli1.start();
    cli2.start();
    cli3.start();
    cli4.start();

    /* sleep */
    printf("This is a long test (30secs)\n");
    SleepSeconds(30);

    client_entry1.stop();
    client_entry2.stop();
    client_entry3.stop();
    client_entry4.stop();
    server_entry.stop();

    assert( cli1.exit_event().wait(30000) == true );
    assert( cli2.exit_event().wait(30000) == true );
    assert( cli3.exit_event().wait(30000) == true );
    assert( cli4.exit_event().wait(30000) == true );
    assert( srv.exit_event().wait(30000) == true );
  }

} // end of test_mt_udp

using namespace test_mt_udp;

int main()
{
  initcomm w;
  basic();
  return 0;
}

/* EOF */
