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
 
#ifndef _csl_rpc_srv_trans_tcp_hh_included_
#define _csl_rpc_srv_trans_tcp_hh_included_

#include "codesloop/common/common.h"
#ifdef __cplusplus
#include "codesloop/common/obj.hh"
#include "codesloop/common/pbuf.hh"
#include "codesloop/comm/handler.hh"
#include "codesloop/comm/tcp_lstnr.hh"

using namespace csl::comm;
using namespace csl::comm::tcp;

namespace csl 
{ 
  namespace rpc 
  {
    /** @brief stores parsed interface description */
    class srv_trans_tcp : public csl::comm::handler
    {
      CSL_OBJ(csl::rpc,srv_trans_tcp);

    public:
      void listen(const char * hostname, unsigned short port);

    protected:
      virtual bool on_connected( connid_t id,
                                 const SAI & sai,
                                 bfd & buf_fd );

      virtual bool on_data_arrival( connid_t id,
                                    const SAI & sai,
                                    bfd & buf_fd );

      virtual void on_disconnected( connid_t id,
                                    const SAI & sai );
  
      virtual void despatch( csl::common::pbuf & buffer) = 0;      
    };


  }
}

#endif /* __cplusplus */
#endif /* _csl_rpc_srv_trans_tcp_hh_included_ */
