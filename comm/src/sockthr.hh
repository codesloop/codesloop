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

#ifndef _csl_comm_sockthr_hh_included_
#define _csl_comm_sockthr_hh_included_

#include "tbuf.hh"
#include "thread.hh"
#include "mutex.hh"
#include "event.hh"
#include "common.h"
#ifdef __cplusplus
#include <list>

namespace csl
{
  using csl::nthread::thread;
  using csl::nthread::event;
  using csl::nthread::mutex;
  using csl::common::tbuf;

  namespace comm
  {
    class sockthr
    {
      public:
        typedef struct sockaddr_in SAI;

        struct item
        {
          /* types */
          typedef tbuf<256> buf_t;

          /* data part */
          buf_t   buffer_;
          SAI     addr_;
        };

        inline bool init(int sock, bool is_connected) { return entry_.init(sock,is_connected); }

        inline bool send(const item & it)   { return entry_.send(it);   }
        inline bool recv(item & it)         { return entry_.recv(it);   }
        inline event & on_recvd()           { return entry_.on_recvd(); }

      private:
        /* socket server thread */
        class entry : public thread::callback
        {
          public:
            virtual void operator()(void);
            virtual ~entry() {}

            bool init(int sock, bool is_connected);
            bool send(const item & it);
            bool recv(item & it);

            typedef std::list<item *> itemlist_t;

          private:
            mutex          mtx_;
            event          on_recvd_;

            bool           stop_me_;    // signals the thread to be stoped
            bool           connected_;  // sendto/recvfrom or send/recv to be used
            int            socket_;     // working socket
            int            siglstnr_;   // notif listener
            int            sigsock_;    // notif sender
            SAI            addr_;       // address of notif socket

            itemlist_t     items_in_;   // arrived items
            itemlist_t     items_out_;  // items to be sent

            void conn_loop();
            void noconn_loop();

          public:
            inline entry() : stop_me_(false), connected_(false), socket_(-1) {}

            inline event & on_recvd() { return on_recvd_; }
        };

        thread thread_;
        entry  entry_;
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_comm_sockthr_hh_included_ */
