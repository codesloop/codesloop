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

#include "ecdh_key.hh"
#include "bignum.hh"
#include "crypt_pkt.hh"
#include "pbuf.hh"
#include "common.h"
#ifdef __cplusplus

namespace csl
{
  using common::tbuf;
  using sec::crypt_pkt;
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
          unicast_auth_resp_p,
          multicast_auth_p,
          multicast_auth_resp_p,
          data_p,
          ack_rand_p
        };

        typedef tbuf<64>               prolog_t;
        typedef crypt_pkt::headbuf_t   headbuf_t;
        typedef crypt_pkt::databuf_t   databuf_t;
        typedef crypt_pkt::footbuf_t   footbuf_t;

        // virtual bool encrypt();
        // virtual bool decrypt();

      protected:
        prolog_t   prolog_;
        headbuf_t  header_;
        databuf_t  data_;
        footbuf_t  footer_;

      private:
#if 0
        const bignum   * privk_;
        const ecdh_key * peerpub_;
        const ecdh_key * ownpub_;
#endif
      public:
#if 0
        /* own private key */
        virtual inline const bignum & privk() const { return *privk_; }
        virtual inline void privk(const bignum & pk) { privk_ = pk; }

        /* peer public key */
        virtual inline const ecdh_key & peerpub() const { return peerpub_; }
        virtual inline void peerpub(const ecdh_key & pk) { peerpub_ = pk; }

        /* own public key */
        virtual inline const ecdh_key & ownpub() const { return ownpub_; }
        virtual inline void ownpub(const ecdh_key & pk) { ownpub_ = pk; }
#endif

        virtual inline ~udp_pkt() {}
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_comm_udp_pkt_hh_included_ */
