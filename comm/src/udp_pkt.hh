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

#ifndef _csl_comm_udp_pkt_hh_included_
#define _csl_comm_udp_pkt_hh_included_

#include "udp_srv_info.hh"
#include "ecdh_key.hh"
#include "bignum.hh"
#include "pbuf.hh"
#include "common.h"
#ifdef __cplusplus

namespace csl
{
  using common::tbuf;
  using sec::bignum;
  using sec::ecdh_key;

  namespace comm
  {
    class udp_pkt
    {
      public:
        enum {
          hello_p = 1,
          olleh_p,
          result_p,
          unicast_auth_p,
          unicast_htua_p,
          multicast_auth_p,
          multicast_htua_p,
          data_p,
          ack_rand_p
        };

        enum {
          max_length_v = 65536
        };

        unsigned char * data();
        unsigned int maxlen();

        /* hello packet */
        bool init_hello(unsigned int len);
        unsigned char * prepare_hello(unsigned int & len);

        /* olleh packet */
        bool init_olleh(unsigned int len);
        unsigned char * prepare_olleh(unsigned int & len);

#if 0
        /* auth packet */
        bool init_uc_auth(unsigned int len);
        unsigned char * prepare_uc_auth(unsigned int & len);

        /* htua packet */
        bool init_uc_htua(unsigned int len);
        unsigned char * prepare_uc_htua(unsigned int & len);
#endif

        /* variables */
        ecdh_key & peer_pubkey();

        void own_pubkey(const ecdh_key & pk);
        ecdh_key & own_pubkey();

        void srv_info(const udp_srv_info & info);
        udp_srv_info & srv_info();

        void own_privkey(const bignum & pk);
        bignum & own_privkey();

        virtual inline ~udp_pkt() {}
        udp_pkt();

      private:
        udp_srv_info   info_;
        bignum         own_privkey_;
        ecdh_key       own_pubkey_;
        ecdh_key       peer_pubkey_;
        unsigned char  data_[max_length_v];
        bool           use_exc_;
        bool           debug_;

      public:
        inline bool use_exc() const      { return use_exc_;  }
        inline void use_exc(bool yesno)  { use_exc_ = yesno; }

        inline bool debug() const      { return debug_; }
        inline void debug(bool yesno)  { debug_ = yesno; }
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_comm_udp_pkt_hh_included_ */
