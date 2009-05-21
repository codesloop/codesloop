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

#ifndef _csl_comm_udp_cli_hh_included_
#define _csl_comm_udp_cli_hh_included_

#include "exc.hh"
#include "pbuf.hh"
#include "ecdh_key.hh"
#include "bignum.hh"
#include "udp_srv_info.hh"
#include "udp_pkt.hh"
#include "common.h"
#ifdef __cplusplus
#include <string>

namespace csl
{
  using sec::ecdh_key;
  using sec::bignum;
  using std::string;
  using common::pbuf;

  namespace comm
  {
    class udp_cli
    {
      public:
        /* typedefs */
        typedef struct sockaddr_in SAI;

        bool hello( unsigned int timeout_ms=0 );
        bool start( unsigned int timeout_ms=0 );

        bool send( unsigned char * data, unsigned int sz );
        bool recv( unsigned char * data, unsigned int & sz, unsigned int timeout_ms=0 );

        udp_cli();
        virtual ~udp_cli();

      private:
        /* internal */
        bool use_exc_;

        /* user should fill these */
        SAI                    hello_addr_;
        SAI                    auth_addr_;
        SAI                    data_addr_;

        /* user or server fills this */
        udp_pkt                pkt_;

        /* connection related */
        int                    hello_sock_;
        int                    auth_sock_;
        int                    data_sock_;
        unsigned long long     client_rand_;
        unsigned long long     server_rand_;
        string                 session_key_;

        bool init();

      public:

        /* inline accessors and manipulators */

        /* hello_addr */
        inline const SAI & hello_addr() const { return hello_addr_; }
        inline void hello_addr(const SAI & a) { hello_addr_ = a;    }

        /* auth_addr */
        inline const SAI & auth_addr() const { return auth_addr_; }
        inline void auth_addr(const SAI & a) { auth_addr_ = a;    }

        /* data_addr */
        inline const SAI & data_addr() const { return data_addr_; }
        inline void data_addr(const SAI & a) { data_addr_ = a;    }

        /* login */
        inline const std::string & login() const { return pkt_.login(); }
        inline void login(const std::string & v) { pkt_.login(v);       }

        /* pass */
        inline const std::string & pass() const { return pkt_.pass(); }
        inline void pass(const std::string & v) { pkt_.pass(v);       }

        /* private key */
        inline const bignum & private_key() const { return pkt_.own_privkey_const(); }
        inline void private_key(const bignum & v) { pkt_.own_privkey(v);       }

        /* public key */
        inline const ecdh_key & public_key() const { return pkt_.own_pubkey_const(); }
        inline void public_key(const ecdh_key & v) { pkt_.own_pubkey(v);       }

        /* info */
        inline const udp_srv_info & server_info() const { return pkt_.server_info_const(); }
        inline void server_info(const udp_srv_info & v) { pkt_.server_info(v); }


        /** @brief Specifies whether param should throw common::exc exceptions
        @param yesno is the desired value to be set

        the default value for use_exc() is true, so it throws exceptions by default */
        inline void use_exc(bool yesno) { use_exc_ = yesno; }

        /** @brief Returns the current value of use_exc
        @return true if exc exceptions are used */
        inline bool use_exc() const { return use_exc_; }
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_comm_udp_cli_hh_included_ */
