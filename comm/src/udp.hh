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

#ifndef _csl_comm_udp_hh_included_
#define _csl_comm_udp_hh_included_

#include "event.hh"
#include "thread.hh"
#include "thrpool.hh"
#include "mutex.hh"
#include "circbuf.hh"
#include "common.h"
#ifdef __cplusplus
#include <string>

namespace csl
{
  using nthread::event;
  using nthread::mutex;
  using nthread::thread;
  using nthread::thrpool;

  namespace comm
  {
    class udp
    {
      public:
        /* typedefs */
        typedef struct sockaddr_in SAI;

        struct msg
        {
          enum { max_len_v = 65536 };
          unsigned char  data_[max_len_v];
          unsigned int   size_;
          SAI            sender_;

          inline unsigned int max_len() { return max_len_v; }
        };

        class recvr : public thread::callback
        {
          public:
            class msgs : public common::circbuf<msg,30>
            {
              public:
                virtual void on_new_item() { ev_.notify(); }
                virtual ~msgs() { }

                mutex   mtx_;
                event   ev_;
            };

            class msg_handler : public thread::callback
            {
              public:
                /* this must lock/unlock msgs_.mtx_ */
                virtual void operator()(void) = 0;

                inline void set_msgs(msgs & m) { msgs_ = &m; }

              protected:
                msgs * msgs_;
            };

          private:
            thrpool       thread_pool_;
            msgs          msgs_;
            SAI           addr_;
            int           socket_;
            bool          stop_me_;
            bool          use_exc_;
            mutex         mtx_;

          public:
            bool start( const SAI & addr,
                        unsigned int min_threads,
                        unsigned int max_threads,
                        unsigned int timeout_ms,
                        unsigned int attempts,
                        msg_handler & cb );

            bool use_exc();
            void use_exc(bool yesno);

            bool stop_me();
            void stop_me(bool yesno);

            virtual void operator()(void);
            virtual ~recvr();
            recvr();
        };
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_comm_udp_hh_included_ */
