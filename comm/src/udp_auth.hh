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

#ifndef _csl_comm_udp_auth_hh_included_
#define _csl_comm_udp_auth_hh_included_

/**
   @file udp_auth.hh
   @brief the authentication part of the UDP communication layer
 */

#include "udp_hello.hh"
#include "udp_recvr.hh"
#include "bignum.hh"
#include "ecdh_key.hh"
#include "tbuf.hh"
#include "common.h"
#include "str.hh"
#ifdef __cplusplus

namespace csl
{
  using nthread::event;
  using nthread::mutex;
  using nthread::thread;
  using nthread::thrpool;

  using sec::ecdh_key;
  using sec::bignum;

  using common::tbuf;
  using common::str;

  namespace comm
  {
    namespace udp
    {
      typedef tbuf<8> saltbuf_t; ///<type for salt data

      /** @brief server side callback for validating credentials sent by the client */
      class valid_creds_callback
      {
        public:
          virtual ~valid_creds_callback() {}
          virtual bool operator()( const ecdh_key & peer_public_key,    // in
                                   const SAI & addr,                    // in
                                   const str & login,                // in
                                   const str & pass) = 0;            // in
      };

      /** @brief server side callback for registering authenticated clients */
      class register_auth_callback
      {
        public:
          virtual ~register_auth_callback() {}
          virtual bool operator()( const SAI & addr,
                                   const str & login,
                                   const str & pass,
                                   const str & session_key,
                                   const saltbuf_t & peer_salt,
                                   saltbuf_t & my_salt ) = 0;
      };

      /** @brief handler that is called on new packet arrival */
      class auth_handler : public recvr::msg_handler
      {
        public:
          enum { salt_size_v = saltbuf_t::preallocated_size };

          virtual void operator()(void);
          virtual ~auth_handler() {}

          valid_key_callback       * valid_key_cb_;
          valid_creds_callback     * valid_creds_cb_;
          register_auth_callback   * register_auth_cb_;

          auth_handler() : valid_key_cb_(0), valid_creds_cb_(0), register_auth_cb_(0) {}

          /* auth packet */
          bool init_auth( ecdh_key & peer_public_key,
                          str & login,
                          str & pass,
                          str & session_key,
                          saltbuf_t & salt,
                          const msg & m );

          /* htua packet */
          bool prepare_htua( const saltbuf_t & pkt_salt,
                             const saltbuf_t & my_salt,
                             const str & session_key,
                             msg & m );
      };

      /** @brief the server class to be started */
      class auth_srv
      {
        public:
          bool start();
          bool stop();

          auth_srv();
          virtual ~auth_srv();

          /* addresses */
          SAI addr()               { return receiver_.addr(); }
          void addr(const SAI & a) { receiver_.addr(a); }

          /* private key */
          const bignum & private_key()       { return handler_.private_key(); }
          void private_key(const bignum & v) { handler_.private_key(v);       }

          /* public key */
          const ecdh_key & public_key()       { return handler_.public_key(); }
          void public_key(const ecdh_key & v) { handler_.public_key(v);       }

          /* valid key callback */
          void valid_key_cb(valid_key_callback & cb) { handler_.valid_key_cb_ = &cb; }

          /* validate credentials callback */
          void valid_creds_cb(valid_creds_callback & cb) { handler_.valid_creds_cb_ = &cb; }

          /* register authenticated clients */
          void register_auth_cb(register_auth_callback & cb) { handler_.register_auth_cb_ = &cb; }

          /* use exceptions ? */
          inline void use_exc(bool yesno) { use_exc_ = yesno; }
          inline bool use_exc() const     { return use_exc_;  }

          /* debug ? */
          inline void debug(bool yesno) { debug_ = yesno; }
          inline bool debug() const     { return debug_;  }

          /* set threadpool params _before_ start */
          inline void set_threadpool_params( unsigned int min_th,
                                             unsigned int max_th,
                                             unsigned int timeout_ms,
                                             unsigned int retries )
          {
            min_threads_ = min_th;
            max_threads_ = max_th;
            timeout_ms_  = timeout_ms;
            retries_     = retries;
          }

          inline unsigned int min_threads() const { return min_threads_; }
          inline unsigned int max_threads() const { return max_threads_; }
          inline unsigned int timeout_ms() const  { return timeout_ms_;  }
          inline unsigned int retries() const     { return retries_;     }

        private:
          /* */
          thread          thread_;
          recvr           receiver_;
          auth_handler    handler_;

          /* internal */
          bool use_exc_;
          bool debug_;

          /* thread pool */
          unsigned int min_threads_;
          unsigned int max_threads_;
          unsigned int timeout_ms_;
          unsigned int retries_;
      };

      /** @brief the client class */
      class auth_cli
      {
        public:
          bool auth( unsigned int timeout_ms=0 );

          auth_cli();
          virtual ~auth_cli();

        private:
          /* internal */
          bool         use_exc_;
          bool         debug_;

          SAI          addr_;
          int          sock_;
          bignum       private_key_;
          ecdh_key     public_key_;
          ecdh_key     server_public_key_;
          saltbuf_t    server_salt_;
          saltbuf_t    my_salt_;

          str          login_;
          str          pass_;
          str          session_key_;

          bool init();

        public:

          /* pkt manipulation */
          bool prepare_auth( msg & m );
          bool init_htua( const msg & m );

          /* server salt */
          inline const saltbuf_t & server_salt() const { return server_salt_; }

          /* own salt */
          inline const saltbuf_t & my_salt() const { return my_salt_; }

          /* session_key */
          inline const str & session_key() const { return session_key_; }

          /* auth_addr */
          inline const SAI & addr() const { return addr_; }
          inline void addr(const SAI & a) { addr_ = a;    }

          /* private key */
          inline const bignum & private_key() const { return private_key_; }
          inline void private_key(const bignum & v) { private_key_ = v;    }

          /* public key */
          inline const ecdh_key & public_key() const { return public_key_; }
          inline void public_key(const ecdh_key & v) { public_key_ = v;    }

          /* server_public_key */
          inline const ecdh_key & server_public_key() const  { return server_public_key_; }
          inline void server_public_key(const ecdh_key & pk) { server_public_key_ = pk;   }

          /* login */
          inline const str & login() const { return login_; }
          inline void login(const str & l) { login_ = l; }

          /* pass */
          inline const str & pass() const  { return pass_;  }
          inline void pass(const str & p)  { pass_ = p; }

          /* debug ? */
          inline void debug(bool yesno) { debug_ = yesno; }
          inline bool debug() const     { return debug_; }

          /* use exceptions ? */
          inline void use_exc(bool yesno) { use_exc_ = yesno; }
          inline bool use_exc() const { return use_exc_; }
      };
    } /* end of udp namespace */
  } /* end of comm namespace */
} /* end of csl namespace */

#endif /* __cplusplus */
#endif /* _csl_comm_udp_auth_hh_included_ */
