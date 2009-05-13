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

#ifndef _csl_comm_udp_srv_info_hh_included_
#define _csl_comm_udp_srv_info_hh_included_

#include "ecdh_key.hh"
#include "common.h"
#ifdef __cplusplus

namespace csl
{
  using sec::ecdh_key;

  namespace comm
  {
    class udp_srv_info
    {
      public:
        /* public key */
        inline const ecdh_key & public_key() const   { return public_key_; }
        inline void public_key(const ecdh_key & pk)  { public_key_ = pk; }

        /* need login ? */
        inline bool need_login() const      { return need_login_; }
        inline void need_login(bool yesno)  { need_login_ = yesno; }

        /* need pass ? */
        inline bool need_pass() const      { return need_pass_; }
        inline void need_pass(bool yesno)  { need_pass_ = yesno; }

        inline udp_srv_info() : need_login_(true), need_pass_(true) {}
        virtual ~udp_srv_info() {}

      private:
        ecdh_key public_key_;
        bool need_login_;
        bool need_pass_;
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_comm_udp_srv_info_hh_included_ */
