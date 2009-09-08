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
   @file t__tcp_libev.cc
   @brief @todo
 */

//#if 0
#ifndef DEBUG
#define DEBUG
#define DEBUG_ENABLE_INDENT
//#define DEBUG_VERBOSE
#endif /* DEBUG */
//#endif

#include "codesloop/common/inpvec.hh"
#include "codesloop/comm/sai.hh"
#include "libev/evwrap.h"
#include "codesloop/comm/wsa.hh"
#include "codesloop/nthread/mutex.hh"
#include "codesloop/common/logger.hh"
#include "codesloop/common/common.h"
#include <assert.h>

using namespace csl::comm;
using namespace csl::common;
using namespace csl::nthread;

/** @brief @todo */
namespace test_tcp_libev {

  /*
  ** DEBUG support --------------------------------------------------------------------
  */
  static inline const wchar_t * get_namespace()   { return L"test_tcp_libev"; }
  static inline const wchar_t * get_class_name()  { return L"test_tcp_libev::noclass"; }
  static inline const wchar_t * get_class_short() { return L"noclass"; }


  /*
  ** Check backends --------------------------------------------------------------------
  */

#define PRINT_BACKEND(V,B) printf("  backend: %26s [%s]\n",#B,(((V&B)!=0)?"OK":"--"))

  void print_backends(unsigned int b)
  {
    PRINT_BACKEND(b,EVBACKEND_SELECT);
    PRINT_BACKEND(b,EVBACKEND_POLL);
    PRINT_BACKEND(b,EVBACKEND_EPOLL);
    PRINT_BACKEND(b,EVBACKEND_KQUEUE);
    PRINT_BACKEND(b,EVBACKEND_DEVPOLL);
    PRINT_BACKEND(b,EVBACKEND_PORT);
  }

  void which_backends()
  {
    printf("\nev_supported_backends()\n");    print_backends(ev_supported_backends());
    printf("\nev_recommended_backends()\n");  print_backends(ev_recommended_backends());
    printf("\nev_embeddable_backends()\n");   print_backends(ev_embeddable_backends());

    /* check what backend are we actually using */
    struct ev_loop * evs = ev_loop_new(EVFLAG_AUTO);
    printf("\nEVFLAG_AUTO\n"); print_backends(ev_backend(evs));
    ev_loop_destroy(evs);
    /* on linux this gave me EVBACKEND_EPOLL, which is what I desired */
  }

  /*
  ** basic TIMEOUT testing --------------------------------------------------------------------
  */

  static void tmr_cb (EV_P_ ev_timer *w, int revents)
  {
    ENTER_FUNCTION();
    CSL_DEBUGF(L"timed out");
    LEAVE_FUNCTION();
  }

  void timeout_test()
  {
    ENTER_FUNCTION();
    ev_timer tmr;
    struct ev_loop * evs = ev_loop_new(EVFLAG_AUTO);
    CSL_DEBUGF(L"init timer");
    ev_timer_init( &tmr, tmr_cb, 3.0, 0.0 );
    ev_timer_start( evs, &tmr );
    CSL_DEBUGF(L"launch loop");
    ev_loop( evs, 0 );
    ev_loop_destroy( evs );
    LEAVE_FUNCTION();
  }

  /*
  ** basic LISTENER testing --------------------------------------------------------------------
  */

  static void tmr_cb2(EV_P_ ev_timer *w, int revents)
  {
    ENTER_FUNCTION();
    CSL_DEBUGF(L"timed out");
    struct ev_loop * evs = reinterpret_cast<struct ev_loop *>(w->data);
    CSL_DEBUGF(L"unlooping %p",evs);
    ev_unloop( evs, EVUNLOOP_ALL );
    LEAVE_FUNCTION();
  }

  static void acc_cb(struct ev_loop *loop, ev_io *w, int revents)
  {
    ENTER_FUNCTION();
    CSL_DEBUGF(L"accept conn: fd:%d w->events:%d revents:%d",w->fd,w->events,revents);
    SAI addr;
    socklen_t sz = sizeof(addr);
    int conn_fd = ::accept( w->fd,reinterpret_cast<struct sockaddr *>(&addr),&sz );
    CSL_DEBUGF(L"accepted conn: %d",conn_fd);
    CSL_DEBUGF(L"closing socket");
    ShutdownCloseSocket( conn_fd );
    LEAVE_FUNCTION();
  }

  void test_listen()
  {
    ENTER_FUNCTION();
    SAI server;
    in_addr_t saddr = inet_addr("127.0.0.1");

    struct ev_loop * evs = ev_loop_new(EVFLAG_AUTO);
    int sock = ::socket( AF_INET, SOCK_STREAM, 0 );
    ev_timer tmr;
    tmr.data = evs;
    ev_io accept_watcher;

    int on = 1;
    if( ::setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) ) < 0)
    {
      perror( "setsockopt" ); goto bail;
    }

    ::memset( &server,0,sizeof(server) );
    ::memcpy( &(server.sin_addr),&saddr,sizeof(saddr) );

    server.sin_family  = AF_INET;
    server.sin_port    = htons(49912);

    CSL_DEBUGF(L"binding socket to port: %d",ntohs(server.sin_port));
    if( ::bind( sock,
                (struct sockaddr *)&server,
                sizeof(server) ) == -1 )    { perror( "bind" );   goto bail; }
    if( ::listen( sock, 100 ) == -1 )       { perror( "listen" ); goto bail; }

    ev_init( &accept_watcher, acc_cb );

    CSL_DEBUGF(L"init socket watcher");
    ev_io_set( &accept_watcher, sock, EV_READ  );
    ev_io_start( evs, &accept_watcher );

    CSL_DEBUGF(L"init timer");
    ev_timer_init( &tmr, tmr_cb2, 5.0, 0.0 );
    ev_timer_start( evs, &tmr );

    CSL_DEBUGF(L"launch loop");
    ev_loop( evs, 0 );

    CSL_DEBUGF(L"cleanup");

  bail:
    ev_loop_destroy( evs );
    ShutdownCloseSocket( sock );
    LEAVE_FUNCTION();
  }

  /*
  ** basic READ testing --------------------------------------------------------------------
  */

  static void read_cb(struct ev_loop *loop, ev_io *w, int revents);

  struct user_conn
  {
    ev_io                 watcher_;
    struct ev_loop *      loop_;
    int                   socket_;
    SAI                   peer_addr_;
    unsigned short        buf_size_;
    mutex                 m_;
    unsigned char         buf_[16*1024];
    const unsigned short  max_size_;

    user_conn( ) : loop_(0), socket_(0), buf_size_(0), max_size_(sizeof(buf_))
    {
      ENTER_FUNCTION();
      ev_init( &watcher_, read_cb );
      CSL_DEBUG_ASSERT( reinterpret_cast<void *>(this) == reinterpret_cast<void *>(&watcher_) );
      LEAVE_FUNCTION();
    }

    CSL_OBJ( test_tcp_libev, user_conn );
  };

  static void read_cb(struct ev_loop *loop, ev_io *w, int revents)
  {
    ENTER_FUNCTION();
    // TODO : double-check fd
    user_conn * c = reinterpret_cast<user_conn *>(w);
    int r = ::recv( c->socket_, c->buf_, 1 /* c->max_size_ */ , 0 );
    CSL_DEBUGF(L"recv(%d,%p,1,0) => %d [%c]",c->socket_,c->buf_,r,c->buf_[0]);
    LEAVE_FUNCTION();
  }

  typedef inpvec<user_conn> conn_pool_t;

  static void acc_cb2( struct ev_loop *loop, ev_io *w, int revents )
  {
    ENTER_FUNCTION();
    CSL_DEBUGF(L"accept conn: fd:%d w->events:%d revents:%d",w->fd,w->events,revents);

    conn_pool_t *   p = reinterpret_cast<conn_pool_t *>(w->data);
    user_conn *     c = (p->last_free().construct());  // TODO : this could be smarter here
    socklen_t      sz = sizeof(c->peer_addr_);

    c->loop_   = loop;
    c->socket_ = ::accept( w->fd,reinterpret_cast<struct sockaddr *>(&(c->peer_addr_)),&sz );

    CSL_DEBUGF( L"accepted conn: %d from %s", c->socket_, inet_ntoa(c->peer_addr_.sin_addr) );

    ev_io_set( &(c->watcher_), c->socket_, EV_READ  );

    CSL_DEBUGF(L"pushing newly allocated watcher to event loop");
    ev_io_start( loop, &(c->watcher_) );

    LEAVE_FUNCTION();
  }

  void test_reader()
  {
    ENTER_FUNCTION();

    /* connection related */
    SAI server;
    in_addr_t saddr = inet_addr("127.0.0.1");
    int sock = ::socket( AF_INET, SOCK_STREAM, 0 );
    ev_io accept_watcher;

    /* the userdata pool */
    conn_pool_t pool;

    /* event loop */
    struct ev_loop * evs = ev_loop_new(EVFLAG_AUTO);

    /* timer related */
    ev_timer tmr;
    tmr.data = evs;

    /* socket options */
    int on = 1;
    if( ::setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) ) < 0)
    {
      perror( "setsockopt" ); goto bail;
    }

    ::memset( &server,0,sizeof(server) );
    ::memcpy( &(server.sin_addr),&saddr,sizeof(saddr) );

    server.sin_family  = AF_INET;
    server.sin_port    = htons(49913);

    /* bind and start listen on the given port */
    CSL_DEBUGF(L"binding socket to port: %d",ntohs(server.sin_port));
    if( ::bind( sock,
                (struct sockaddr *)&server,
                sizeof(server) ) == -1 )    { perror( "bind" );   goto bail; }
    if( ::listen( sock, 100 ) == -1 )       { perror( "listen" ); goto bail; }

    /* socket watcher */
    CSL_DEBUGF(L"init socket watcher");
    ev_init( &accept_watcher, acc_cb2 );
    ev_io_set( &accept_watcher, sock, EV_READ  );
    ev_io_start( evs, &accept_watcher );
    accept_watcher.data = &pool;

    /* setup timer */
    CSL_DEBUGF(L"init timer");
    ev_timer_init( &tmr, tmr_cb2, 8.0, 0.0 );
    ev_timer_start( evs, &tmr );

    /* start loop */
    CSL_DEBUGF(L"launch loop");
    ev_loop( evs, 0 );

    CSL_DEBUGF(L"cleanup");

  bail:
    ev_loop_destroy( evs );
    ShutdownCloseSocket( sock );
    LEAVE_FUNCTION();
  }

} // end of test_tcp_libev

using namespace test_tcp_libev;

int main()
{
  wsa w;
  which_backends();
  timeout_test();
  test_listen();
  test_reader();
  return 0;
}
/* EOF */
