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

#ifndef _csl_comm_udp_auth_entry_hh_included_
#define _csl_comm_udp_auth_entry_hh_included_

#include "udp_srv_entry.hh"
#include "cb.hh"
#include "common.h"
#ifdef __cplusplus

namespace csl
{
  namespace comm
  {
    class udp_srv;

    class udp_auth_entry : public udp_srv_entry
    {
      public:
        virtual void operator()(void);
        virtual ~udp_auth_entry();
        udp_auth_entry(udp_srv & srv);

        void valid_key_cb( cb::valid_key & vkcb );
        void valid_creds_cb( cb::valid_creds & vccb );

      private:
        cb::valid_key    * valid_key_cb_;
        cb::valid_creds  * valid_creds_cb_;
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_comm_udp_auth_entry_hh_included_ */
