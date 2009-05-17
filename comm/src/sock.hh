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

#ifndef _csl_comm_synchsock_hh_included_
#define _csl_comm_synchsock_hh_included_

#include "mutex.hh"
#include "event.hh"
#include "common.h"
#ifdef __cplusplus

namespace csl
{
  namespace comm
  {
    class synchsock
    {
      public:
        typedef struct sockaddr_in SAI;

        bool init(int sck);
        bool wait_read(unsigned int ms);
        bool wait_write(unsigned int ms);

        inline mutex & mtx() { return mtx_; }
        inline synchsock() : socket_(-1), siglstnr_(-1), sigsock_(-1) {}

      private:
        mutex          mtx_;
        event          read_evt_;
        event          write_evt_;

        int            socket_;     // working socket
        int            siglstnr_;   // notif listener
        int            sigsock_;    // notif sender
        SAI            addr_;       // address of notif socket

        /* no copy */
        synchsock & operator(const synchsock & other) { return *this; }
        synchsock(const synchsock & other) {}
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_comm_synchsock_hh_included_ */
