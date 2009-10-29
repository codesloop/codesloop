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
#include "codesloop/common/queue.hh"
#include "codesloop/common/logger.hh"
#include "codesloop/comm/exc.hh"
#include "codesloop/comm/tcp_lstnr.hh"
#include "codesloop/comm/bfd.hh"
#include "codesloop/comm/sai.hh"
#include "codesloop/nthread/mutex.hh"
#include "codesloop/nthread/thread.hh"
#include "codesloop/nthread/thrpool.hh"
#include "codesloop/nthread/event.hh"

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

          tcp_conn( int fd,
                    const SAI & peer_addr)
            : bfd_(fd), peer_addr_(peer_addr) { }

          ~tcp_conn() { bfd_.shutdown(); }
        };

        struct ev_data
        {
          ev_io              watcher_;
          struct ev_loop *   loop_;
          tcp_conn *         conn_;
          connid_t           id_;
          uint64_t           pos_;
          mutex              mtx_;
          bool               unqueued_;

          ev_data(const ev_data & other) : use_exc_(true)
          {
            ENTER_FUNCTION();
            THRNORET(exc::rs_not_implemented);
            LEAVE_FUNCTION();
          }

          ev_data & operator=(const ev_data & other)
          {
            ENTER_FUNCTION();
            THR(exc::rs_not_implemented,*this);
            RETURN_FUNCTION(*this);
          }

          ev_data( struct ev_loop * loop,
                   tcp_conn * cn,
                   connid_t id,
                   uint64_t pos )
            : loop_(loop),
              conn_(cn),
              id_(id),
              pos_(pos),
              unqueued_(false),
              use_exc_(true) { }

          CSL_OBJ(csl::comm::anonymous,ev_data);
          USE_EXC();
        };

        void lstnr_accept_cb( struct ev_loop *loop, struct ev_io *w, int revents );
        void lstnr_wakeup_cb( struct ev_loop *loop, struct ev_async *w, int revents );
        void lstnr_timer_cb( struct ev_loop *loop, struct ev_timer *w, int revents );
        void lstnr_new_data_cb( struct ev_loop *loop, ev_io *w, int revents );

        class conn_queue : public csl::common::queue<ev_data *>
        {
          private:
            mutex mtx_;
            event evt_;

          public:
            void on_new_item()       { evt_.notify(); }
            void on_lock_queue()     { mtx_.lock();   }
            void on_unlock_queue()   { mtx_.unlock(); }
            event & new_item_event() { return evt_;   }
        };

        class data_handler : public thread::callback
        {
          private:
            lstnr::impl * lstnr_;
            conn_queue  * queue_;
          public:
            data_handler(lstnr::impl * l, conn_queue  * q)
              : lstnr_(l),
                queue_(q)  { }

            virtual void operator()(void);
            virtual ~data_handler() { }
        };
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

        typedef inpvec<ev_data>     ev_data_vec_t;
        typedef inpvec<tcp_conn>    tcp_conn_vec_t;
        typedef inpvec<ev_data *>   ev_data_ptr_vec_t;

        SAI                  addr_;                        // OK
        auto_close_socket    listener_;                    // OK
        struct ev_loop *     loop_;                        // OK
        ev_io                accept_watcher_;              // OK
        ev_async             wakeup_watcher_;              // OK
        ev_timer             periodic_watcher_;            // OK
        listener_entry       entry_;                       // OK
        thread               listener_thread_;             // OK
        bool                 stop_me_;            // -----------
        mutex                mtx_;
        handler *            handler_;
        ev_data_vec_t        ev_pool_;
        tcp_conn_vec_t       conn_pool_;

        //
        conn_queue           new_data_queue_;
        data_handler         new_data_handler_;

        impl() : entry_(this),
                 stop_me_(false),
                 handler_(0),
                 new_data_handler_(this, &new_data_queue_),
                 use_exc_(false)
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
          CSL_DEBUGF(L"listener socket:%d created for (%s:%d)",
                      sock,
                      inet_ntoa(address.sin_addr),
                      ntohs(address.sin_port) );

          // this ensures that the listener socket will be closed
          listener_.init(sock);

          int on = 1;
          if( ::setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) ) < 0 )
            THRC(exc::rs_setsockopt,false);

          CSL_DEBUGF(L"setsockopt has set SO_REUSEADDR on %d",sock);

          if( ::bind( sock,
                      reinterpret_cast<const struct sockaddr *>(&address),
                      sizeof(address) ) < 0 )
            THRC(exc::rs_bind_failed,false);

          CSL_DEBUGF(L"socket %d bound to (%s:%d)",
                      sock,
                      inet_ntoa(address.sin_addr),
                      ntohs(address.sin_port));

          if( ::listen( sock, backlog ) < 0 )
            THRC(exc::rs_listen_failed,false);

          CSL_DEBUGF(L"listen(sock:%d,backlog:%d) succeeded",sock,backlog);

          // set handler
          handler_ = &h;

          // save the listener address
          addr_ = address;

          //  - register async notifier
          ev_async_start( loop_, &wakeup_watcher_ );

          //  - register accept watcher
          ev_io_set( &accept_watcher_, sock, EV_READ  );
          ev_io_start( loop_, &accept_watcher_ );

          RETURN_FUNCTION(true);
        }

        void wakeup_cb( struct ev_async *w, int revents ) // TODO
        {
          ENTER_FUNCTION();
          LEAVE_FUNCTION();
        }

        void timer_cb( struct ev_timer *w, int revents ) // TODO
        {
          ENTER_FUNCTION();
          LEAVE_FUNCTION();
        }

        void accept_cb( struct ev_io *w, int revents )
        {
          ENTER_FUNCTION();

          CSL_DEBUGF( L"accept conn: fd:%d w->events:%d revents:%d",
                       w->fd,
                       w->events,
                       revents );

          SAI addr;
          socklen_t sz = sizeof(addr);
          int conn_fd = ::accept( w->fd,
                                  reinterpret_cast<struct sockaddr *>(&addr),
                                  &sz );

          if( conn_fd > 0 )
          {
            CSL_DEBUGF( L"accepted conn: %d from %s:%d",
                         conn_fd,inet_ntoa(addr.sin_addr),
                         ntohs(addr.sin_port) );

            // initialize iterators
            tcp_conn_vec_t::iterator tc_it( conn_pool_.begin() );
            ev_data_vec_t::iterator  ev_it( ev_pool_.begin() );

            // find the first free position for both arrays
            tcp_conn_vec_t::iterator & tc_it_ref( conn_pool_.first_free(tc_it) );
            ev_data_vec_t::iterator  & ev_it_ref( ev_pool_.first_free(ev_it) );

            // create the items
            tcp_conn * cn = tc_it_ref.set( conn_fd, addr );
            ev_data  * ed = ev_it_ref.set( loop_,
                                           cn,
                                           tc_it_ref.get_pos(),
                                           ev_it_ref.get_pos() );

            CSL_DEBUG_ASSERT( cn != NULL );
            CSL_DEBUG_ASSERT( ed != NULL );

            // signal connection startup
            bool cres = handler_->on_connected( ed->id_, addr, cn->bfd_ );

            if( cres == false )
            {
              CSL_DEBUG(L"handler returned FALSE, this tells to close the connection");
              tc_it_ref.free();
              ev_it_ref.free();
            }
            else
            {
              CSL_DEBUG(L"handler returned TRUE for connection startup");
              // initialize connection watcher
              ev_init( &(ed->watcher_), lstnr_new_data_cb );
              ed->watcher_.data = this;
              ev_io_set( &(ed->watcher_), conn_fd, EV_READ  );
              ev_io_start( loop_, &(ed->watcher_) );
            }
          }
          else
          {
            CSL_DEBUGF( L"accept failed" );
          }

          LEAVE_FUNCTION();
        }

        void process_data_cb( ev_data * dta )
        {
          ENTER_FUNCTION();

          bool hres = handler_->on_data_arrival( dta->id_,
                                                 dta->conn_->peer_addr_,
                                                 dta->conn_->bfd_ );

          // TODO XXX TODO
          // ?? unqueued ??
          // ?? failed ??
          // ?? to be removed ??
          // ?? need requeue ??
          //
          if( hres == false )
          {
            CSL_DEBUGF( L"handler returned FALSE, this tells to "
                         "remove at pos:%lld conn_id:%lld",
                         dta->pos_,
                         dta->id_ );
          }
          else
          {
            CSL_DEBUGF( L"XXX" );
          }

          LEAVE_FUNCTION();
        }

        void new_data_cb( struct ev_io *w, int revents )
        {
          ENTER_FUNCTION();

          ev_data * dta = reinterpret_cast<ev_data *>(w);

          CSL_DEBUG_ASSERT( dta->conn_ != NULL );

          int fd = dta->conn_->bfd_.file_descriptor();

          CSL_DEBUGF( L"data arrived on fd:%d conn_id:%lld",
                       fd,
                       dta->id_ );

          uint32_t timeout_ms = 0;
          uint64_t res = dta->conn_->bfd_.recv_some( timeout_ms );

          // check for errors
          if( dta->conn_->bfd_.state() != bfd::ok_ )
          {
            CSL_DEBUGF( L"error during read on fd:%d conn_id:%lld",fd, dta->id_ );
            CSL_DEBUGF( L"remove watcher conn_id:%lld from the loop", dta->id_ );
            ev_io_stop( loop_, w );
            dta->unqueued_ = true;

            // signal connection close
            handler_->on_disconnected( dta->id_, dta->conn_->peer_addr_ );

            if( dta->conn_->bfd_.size() == 0 )
            {
              CSL_DEBUGF( L"no data in bfd. safe to remove conn_id:%lld", dta->id_ );
              remove_connection( dta->id_ );
              LEAVE_FUNCTION();
            }
          }

          // TODO XXX TODO XXX
          // there is data to be processed
          if( res > 0 ) // XXX may be bfd_.size() ???
          {
            if( dta->conn_->bfd_.n_free() == 0 )
            {
              dta->unqueued_ = true;
              ev_io_stop( loop_, w );
            }
            new_data_queue_.push( dta );
          }

          LEAVE_FUNCTION();
        }

        void remove_connection(connid_t id)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"remove_connection(id:%lld)",id);
          LEAVE_FUNCTION();
        }

        void listener_entry_cb( )
        {
          thrpool              tpool;
          // TODO : make these configurable
          unsigned int         min_threads = 1;
          unsigned int         max_threads = 4;
          unsigned int         timeout_ms  = 1000;
          unsigned int         attempts    = 3;

          if( tpool.init( min_threads,
                          max_threads,
                          timeout_ms,
                          attempts,
                          new_data_queue_.new_item_event(),
                          new_data_handler_ ) )
          {
            CSL_DEBUGF(L"thread pool started (min:%d max:%d timeout:%d attempts:%d)",
                        min_threads,
                        max_threads,
                        timeout_ms,
                        attempts);

            CSL_DEBUGF(L"launch loop: %p",loop_);
            ev_loop( loop_, 0 );
            CSL_DEBUGF(L"loop has been stopped: %p",loop_);
            ev_loop_destroy( loop_ );
            loop_ = 0;
          }
          else
          {
            CSL_DEBUGF(L"not launching loop as the thread pool failed to be started");
          }
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

        void lstnr_new_data_cb( struct ev_loop *loop, ev_io *w, int revents )
        {
          lstnr::impl * this_ptr = reinterpret_cast<lstnr::impl *>(w->data);
          this_ptr->new_data_cb(w, revents);
        }

        void data_handler::operator()(void)
        {
          conn_queue::handler h;

          if( queue_->pop(h) )
          {
            ev_data * dta = *(h.get());
            lstnr_->process_data_cb( dta );
          }
        }
      }

      /* forwarding functions */
      const SAI & lstnr::own_addr() const             { return impl_->addr_;         }

      bool lstnr::init(handler & h, SAI address, int backlog)
      {
        return impl_->init(h,address,backlog);
      }

      bool lstnr::start() { return impl_->start(); }
      bool lstnr::stop()  { return impl_->stop();  }

      pevent & lstnr::start_event() { return impl_->start_event(); }
      pevent & lstnr::exit_event()  { return impl_->exit_event();  }

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
