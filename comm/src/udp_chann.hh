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

#ifndef _csl_comm_udp_chann_hh_included_
#define _csl_comm_udp_chann_hh_included_

#include "udp_pkt.hh"
#include "common.h"
#ifdef __cplusplus

namespace csl
{
  namespace comm
  {
    class udp_chann
    {
      public:
        typedef struct sockaddr_in SAI;

        bool send( );
        bool send( const udp_pkt::b1024_t & buffer );

        bool recv( unsigned int timeout_ms );
        bool recv( udp_pkt::b1024_t & dta, unsigned int timeout_ms );

        bool init( const udp_pkt & pk,
                   int sock,
                   SAI & peer_addr,
                   unsigned long long my_salt,
                   unsigned long long peer_salt );

        udp_chann();

        /* return internal buffer */
        inline udp_pkt::b1024_t & buf()  { return buf_; }

        /* server may disable receiving */
        void disable_receive()           { can_receive_ = false; }
        inline bool can_receive() const  { return can_receive_;  }

        /* use exceptions ? */
        inline bool use_exc() const       { return use_exc_; }
        inline void use_exc(bool yesno)   { use_exc_ = yesno; }

      private:
        bool                 use_exc_;
        int                  sock_;
        SAI                  peer_addr_;
        bool                 can_receive_;
        bool                 is_connected_;
        udp_pkt              pkt_;
        udp_pkt::b1024_t     buf_;
        unsigned long long   my_salt_;
        unsigned long long   peer_salt_;

        /* no copy */
        udp_chann & operator=(const udp_chann & other) { return *this; }
        udp_chann(const udp_chann & other) {}
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_comm_udp_chann_hh_included_ */
