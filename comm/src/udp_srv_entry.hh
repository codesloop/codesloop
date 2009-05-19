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

#ifndef _csl_comm_udp_srv_entry_hh_included_
#define _csl_comm_udp_srv_entry_hh_included_

#include "exc.hh"
#include "thread.hh"
#include "mutex.hh"
#include "cb.hh"
#include "udp_srv_info.hh"
#include "common.h"
#ifdef __cplusplus
#include <string>

namespace csl
{
  using std::string;
  using nthread::thread;
  using nthread::mutex;

  namespace comm
  {
    class udp_srv;

    class udp_srv_entry : public thread::callback
    {
      public:
        /* typedefs */
        typedef struct sockaddr_in SAI;

        virtual void operator()(void) = 0;
        virtual ~udp_srv_entry();
        virtual bool start();

        udp_srv_entry(udp_srv & srv);

        virtual bool stop_me();
        virtual void stop_me(bool yesno);

        /* addr */
        virtual const SAI & addr() const;
        virtual void addr(const SAI & a);

        /* socket */
        virtual int socket() const;

        /* parent: udp_srv */
        virtual udp_srv & srv();

        /* exc */
        bool use_exc() const;

      private:
        udp_srv *       srv_;
        SAI             addr_;
        int             socket_;
        bool            stop_me_;
        bool            use_exc_;
        mutex           mtx_;
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_comm_udp_srv_entry_hh_included_ */
