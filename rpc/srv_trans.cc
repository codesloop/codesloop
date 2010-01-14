/*
Copyright (c) 2008,2009,2010, CodeSLoop Team

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

#include <sys/time.h>

#include "codesloop/rpc/srv_trans.hh"
#include "codesloop/common/common.h"
#include "codesloop/common/logger.hh"
#include "codesloop/common/pbuf.hh"
#include "codesloop/common/arch.hh"
#include "codesloop/rpc/exc.hh"

/**
  @file rpc/src/srv_trans.cc
  @brief common functions of rpc transport layer (server side)
 */
using csl::common::logger;

namespace csl
{
  namespace rpc
  {
    
    void srv_trans::ping (
        /* input */     const csl::rpc::client_info &   ci,
        /* input */     const uint64_t & client_time,
        /* output */    uint64_t & server_time
        /* throws csl::common::exc */
        ) 
    {
      ENTER_FUNCTION();

      struct timeval tv;
      struct timezone tz;

      gettimeofday(&tv,&tz);
      server_time = tv.tv_sec * 1000000 + tv.tv_usec;

      LEAVE_FUNCTION();
    }

  };
};

/* EOF */
