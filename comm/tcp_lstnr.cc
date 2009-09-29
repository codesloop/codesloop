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
  @brief @todo
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

        void lstnr_accept_cb( struct ev_loop *loop, ev_io *w, int revents );
        void lstnr_wakeup_cb( struct ev_loop *loop, struct ev_async *w, int revents );

      };

      struct lstnr::impl
      {
        class listener_entry : public thread::callback
        {
          private:
            lstnr::impl * impl_;

          public:
            listener_entry(lstnr::impl * i) : impl_(i) { }

            virtual void operator()(void)
            {
              CSL_DEBUGF(L"listener_entry::operator()");
            }
            CSL_OBJ(csl::comm, lstnr::impl::listener_entry);
        };

        SAI                  addr_;
        inpvec<ev_data>      ev_pool_;
        inpvec<tcp_conn>     conn_pool_;
        inpvec<ev_data *>    unqueued_;
        auto_close_socket    listener_;
        struct ev_loop *     loop_;
        ev_io                accept_watcher_;
        ev_async             wakeup_watcher_;
        listener_entry       entry_;
        thread               entry_thread_;
        bool                 stop_me_;
        mutex                mtx_;

        impl() : entry_(this), stop_me_(false), use_exc_(false)
        {
          loop_ = ev_loop_new( EVFLAG_AUTO );
        }

        ~impl()
        {
          if( loop_ ) ev_loop_destroy( loop_ );
          loop_ = 0;
        }

        // TODO things to be implemented:
        //   - async watcher cleanup ???

        bool init(handler & h, SAI address)
        {
          // TODO
          //  - listener socket and co. init
          int sock = ::socket( AF_INET, SOCK_STREAM, 0 );
          if( sock < 0 )
          {
            THRC(exc::rs_socket_failed,false);
          }

          int on = 1;

          if( ::setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) ) < 0)
          {
            THRC(exc::rs_setsockopt,false);
          }
          //  - accept watcher init
          //  - register async notifier
          //  - register accept watcher

          // async notifier init
          ev_async_init( &wakeup_watcher_, lstnr_wakeup_cb );
          return false;
        }

        bool start()
        {
          // TODO
          //  - start listener thread
          return false;
        }

        bool stop()
        {
          // TODO
          // - set stop flag
          // - stop loop
          // - stop listener thread
          return false;
        }

        /* network ops */
        read_res read(connid_t id, size_t sz, uint32_t timeout_ms) { read_res rr; return rr; } // TODO
        read_res & read(connid_t id, size_t sz, uint32_t timeout_ms, read_res & rr) { return rr; } // TODO
        bool write(connid_t id, uint8_t * data, size_t sz) { return false; } // TODO

        /* info ops */
        const SAI & peer_addr(connid_t id) const { return addr_; } // TODO

        CSL_OBJ(csl::comm, lstnr::impl);
        USE_EXC();
      };

      namespace
      {
        void lstnr_accept_cb( struct ev_loop *loop, ev_io *w, int revents )
        {
        }

        void lstnr_wakeup_cb( struct ev_loop *loop, struct ev_async *w, int revents )
        {
        }
      }

      /* forwarding functions */
      const SAI & lstnr::peer_addr(connid_t id) const
      {
        return impl_->peer_addr(id);
      }

      const SAI & lstnr::own_addr() const
      {
        return impl_->addr_;
      }

      bool lstnr::init(handler & h, SAI address)
      {
        return impl_->init(h,address);
      }

      bool lstnr::start()
      {
        return impl_->start();
      }

      bool lstnr::stop()
      {
        return impl_->stop();
      }

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
