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
  @file tcp_lstnr.cc
  @brief TODO: complete description
*/

#ifndef DEBUG
#define DEBUG
#define DEBUG_ENABLE_INDENT
//#define DEBUG_VERBOSE
#endif /* DEBUG */

#include "codesloop/common/inpvec.hh"
#include "codesloop/common/libev/evwrap.h"
#include "codesloop/common/auto_close.hh"
#include "codesloop/common/logger.hh"
#include "codesloop/comm/exc.hh"
#include "codesloop/comm/tcp_lstnr.hh"
#include "codesloop/comm/bfd.hh"
#include "codesloop/comm/sai.hh"
#include "codesloop/nthread/mutex.hh"
#include "codesloop/nthread/thread.hh"
#include "codesloop/nthread/thrpool.hh"

namespace csl
{
  using namespace nthread;
  using namespace common;

  namespace comm
  {
    namespace tcp
    {
      namespace
      {
        struct tcp_conn
        {
          bfd                bfd_;
          SAI                peer_addr_;

          ~tcp_conn() { bfd_.shutdown(); }
        };

        struct ev_data
        {
          ev_io              watcher_;
          struct ev_loop *   loop_;
          tcp_conn *         conn_;
          connid_t           id_;
          bool               is_active_;
          mutex              mtx_;
        };

        void lstnr_accept_cb( struct ev_loop *loop, struct ev_io *w, int revents );
        void lstnr_wakeup_cb( struct ev_loop *loop, struct ev_async *w, int revents );
        void lstnr_timer_cb( struct ev_loop *loop, struct ev_timer *w, int revents );
      };

      struct lstnr::impl
      {
        class listener_entry : public thread::callback
        {
          private:
            lstnr::impl * impl_;
          public:
            listener_entry(lstnr::impl * i) : impl_(i) { }
            virtual void operator()(void) { impl_->listener_entry_cb(); }
        };

        SAI                  addr_;
        inpvec<ev_data>      ev_pool_;
        inpvec<tcp_conn>     conn_pool_;
        inpvec<ev_data *>    unqueued_;
        auto_close_socket    listener_;
        struct ev_loop *     loop_;
        ev_io                accept_watcher_;
        ev_async             wakeup_watcher_;
        ev_timer             periodic_watcher_;
        listener_entry       entry_;
        thread               listener_thread_;
        bool                 stop_me_;
        mutex                mtx_;

        impl() : entry_(this), stop_me_(false), use_exc_(false)
        {
          // create loop object
          loop_ = ev_loop_new( EVFLAG_AUTO );

          // accept watcher init
          ev_init( &accept_watcher_, lstnr_accept_cb );
          accept_watcher_.data = this;

          // async notifier init
          ev_async_init( &wakeup_watcher_, lstnr_wakeup_cb );
          wakeup_watcher_.data = this;

          // timer init
          ev_init( &periodic_watcher_, lstnr_timer_cb );
          periodic_watcher_.repeat = 15.0;
          periodic_watcher_.data   = this;

          // set thread entry
          listener_thread_.set_entry( entry_ );
        }

        ~impl()
        {
          if( loop_ ) ev_loop_destroy( loop_ );
          loop_ = 0;
        }

        bool init(handler & h, SAI address, int backlog)
        {
          ENTER_FUNCTION();

          //  listener socket and co. init
          int sock = ::socket( AF_INET, SOCK_STREAM, 0 );
          if( sock < 0 ) { THRC(exc::rs_socket_failed,false); }

          // this ensures that the listener socket will be closed
          listener_.init(sock);

          int on = 1;
          if( ::setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) ) < 0 )    { THRC(exc::rs_setsockopt,false); }
          if( ::bind( sock, (const struct sockaddr *)&address, sizeof(address) ) < 0 ) { THRC(exc::rs_bind_failed,false); }
          if( ::listen( sock, backlog ) < 0 )                                          { THRC(exc::rs_listen_failed,false); }

          // save the listener address
          addr_ = address;

          //  - register async notifier
          ev_async_start( loop_, &wakeup_watcher_ );

          //  - register accept watcher
          ev_io_set( &accept_watcher_, sock, EV_READ  );
          ev_io_start( loop_, &accept_watcher_ );

          RETURN_FUNCTION(true);
        }

        /* network ops */
        read_res read(connid_t id, size_t sz, uint32_t timeout_ms) { read_res rr; return rr; } // TODO
        read_res & read(connid_t id, size_t sz, uint32_t timeout_ms, read_res & rr) { return rr; } // TODO
        bool write(connid_t id, uint8_t * data, size_t sz) { return false; } // TODO

        /* info ops */
        const SAI & peer_addr(connid_t id) const { return addr_; } // TODO

        void accept_cb( struct ev_io *w, int revents )
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"accept conn: fd:%d w->events:%d revents:%d",w->fd,w->events,revents);
          SAI addr;
          socklen_t sz = sizeof(addr);
          int conn_fd = ::accept( w->fd,reinterpret_cast<struct sockaddr *>(&addr),&sz );
          CSL_DEBUGF(L"accepted conn: %d from %s:%d",conn_fd,inet_ntoa(addr.sin_addr),ntohs(addr.sin_port));
          CSL_DEBUGF(L"closing socket");
          ShutdownCloseSocket( conn_fd );
          LEAVE_FUNCTION();
        }

        void wakeup_cb( struct ev_async *w, int revents )
        {
          ENTER_FUNCTION();
          LEAVE_FUNCTION();
        }

        void timer_cb( struct ev_timer *w, int revents )
        {
          ENTER_FUNCTION();
          LEAVE_FUNCTION();
        }

        void listener_entry_cb( )
        {
          CSL_DEBUGF(L"launch loop: %p",loop_);
          ev_loop( loop_, 0 );
          CSL_DEBUGF(L"loop has been stopped: %p",loop_);
          ev_loop_destroy( loop_ );
          CSL_DEBUGF(L"exiting listener thread");
        }

        bool start()
        {
          ENTER_FUNCTION();
          bool ret = listener_thread_.start();

          if( ret )
          {
            CSL_DEBUGF(L"start waiting for the thread to be really started");
            ret = listener_thread_.start_event().wait();
            CSL_DEBUGF(L"thread %s",(ret==true?"STARTED":"NOT STARTED"));
          }

          CSL_DEBUGF(L"start() => %s",(ret==true?"OK":"FAILED"));
          RETURN_FUNCTION(ret);
        }

        bool stop()
        {
          // TODO
          // - set stop flag
          // - stop loop
          // - stop listener thread
          return false;
        }

        pevent & start_event()
        {
          return listener_thread_.start_event();
        }

        pevent & exit_event()
        {
          return listener_thread_.exit_event();
        }

        CSL_OBJ(csl::comm, lstnr::impl);
        USE_EXC();
      };

      namespace
      {
        //
        // event handlers. these forward the call to the implementation object
        //
        void lstnr_accept_cb( struct ev_loop *loop, struct ev_io *w, int revents )
        {
          lstnr::impl * this_ptr = reinterpret_cast<lstnr::impl *>(w->data);
          this_ptr->accept_cb(w, revents);
        }

        void lstnr_wakeup_cb( struct ev_loop *loop, struct ev_async *w, int revents )
        {
          lstnr::impl * this_ptr = reinterpret_cast<lstnr::impl *>(w->data);
          this_ptr->wakeup_cb(w, revents);
        }

        void lstnr_timer_cb( struct ev_loop *loop, struct ev_timer *w, int revents)
        {
          lstnr::impl * this_ptr = reinterpret_cast<lstnr::impl *>(w->data);
          this_ptr->timer_cb(w, revents);
        }
      }

      /* forwarding functions */
      const SAI & lstnr::peer_addr(connid_t id) const { return impl_->peer_addr(id); }
      const SAI & lstnr::own_addr() const             { return impl_->addr_;         }

      bool lstnr::init(handler & h, SAI address, int backlog)
      {
        return impl_->init(h,address,backlog);
      }

      bool lstnr::start() { return impl_->start(); }
      bool lstnr::stop()  { return impl_->stop();  }

      pevent & lstnr::start_event() { return impl_->start_event(); }
      pevent & lstnr::exit_event()  { return impl_->exit_event();  }

      read_res lstnr::read(connid_t id, size_t sz, uint32_t timeout_ms)
      {
        return impl_->read(id,sz,timeout_ms);
      }

      read_res & lstnr::read(connid_t id, size_t sz, uint32_t timeout_ms, read_res & rr)
      {
        return impl_->read(id, sz, timeout_ms, rr);
      }

      bool lstnr::write(connid_t id, uint8_t * data, size_t sz)
      {
        return impl_->write(id, data, sz);
      }

      /* default constructor, destructor */
      lstnr::lstnr() : impl_(new impl()) { }
      lstnr::~lstnr() { }

      /* no copy */
      lstnr::lstnr(const lstnr & other) : impl_(reinterpret_cast<impl *>(0))
      {
        THRNORET(exc::rs_not_implemented);
      }

      lstnr & lstnr::operator=(const lstnr & other)
      {
        THR(exc::rs_not_implemented, *this);
        return *this;
      }

    } /* end of ns:tcp */
  } /* end of ns:comm */
} /* end of ns::csl */

/* EOF */
