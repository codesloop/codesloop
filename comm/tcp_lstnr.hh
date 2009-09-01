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

#ifndef _csl_comm_tcp_lstnr_hh_included_
#define _csl_comm_tcp_lstnr_hh_included_

/**
   @file tcp_lstnr.hh
   @brief @todo
 */

#include "sai.hh"
#include "csl_common.hh"
#ifdef __cplusplus
#include <memory>

namespace csl
{
  namespace comm
  {
    namespace tcp
    {
      class notify_new_conn_cb
      {
        public:
          virtual ~notify_new_conn_cb() {}
          virtual bool operator()( const SAI & lstnr_addr, // in
                                   const SAI & peer_addr,  // in
                                   int peer_fd             // in
                                 ) = 0;
      };

      class conn
      {
        public:
      };

      class conn_factory
      {
        public:
      };

      class lstnr
      {
        public:
          lstnr();
          virtual ~lstnr();

          /* address, to be setup during initialization */
          SAI addr();

          bool init();
          bool start();
          bool stop();

          struct impl;
        private:
          /* private implementation */
          std::auto_ptr<impl> impl_;

          /* no-copy */
          lstnr(const lstnr & other);
          lstnr & operator=(const lstnr & other);

          /* for trace and debug */
          CSL_OBJ(csl::comm::tcp,lstnr);
          USE_EXC();
      };
    }
  }
}

#endif /*__cplusplus*/
#endif /*_csl_comm_tcp_lstnr_hh_included_*/
