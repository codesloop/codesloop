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

#ifndef _csl_comm_udp_hello_hh_included_
#define _csl_comm_udp_hello_hh_included_

#include "udp_recvr.hh"
#include "bignum.hh"
#include "ecdh_key.hh"
#include "common.h"
#ifdef __cplusplus

namespace csl
{
  using nthread::event;
  using nthread::mutex;
  using nthread::thread;
  using nthread::thrpool;

  using sec::ecdh_key;
  using sec::bignum;

  using common::str; 

  namespace comm
  {
    namespace udp
    {
#ifndef CSL_COMM_SAI_DEFINED
#define CSL_COMM_SAI_DEFINED
        typedef struct sockaddr_in SAI;
#endif

        class valid_key_callback
        {
          public:
            virtual ~valid_key_callback() {}
            virtual bool operator()( const ecdh_key & peer_public_key ) = 0; // in
        };

        class hello_callback
        {
          public:
            virtual ~hello_callback() {}
            virtual bool operator()( const ecdh_key & peer_public_key,  // in
                                     const SAI & addr,                  // in
                                     ecdh_key & my_public_key,          // in-out
                                     bool & need_login,                 // in-out
                                     bool & need_pass,                  // in-out
                                     bignum & my_private_key ) = 0;     // in-out
        };

        class hello_handler : public recvr::msg_handler
        {
          public:
            virtual void operator()(void);
            virtual ~hello_handler() {}

            valid_key_callback * valid_key_cb_;
            hello_callback     * hello_cb_;

            bool      need_login_;
            bool      need_pass_;

            hello_handler() : valid_key_cb_(0), hello_cb_(0), need_login_(true), need_pass_(true) {}

            /* hello packet */
            bool init_hello( ecdh_key & peer_public_key,
                             const msg & m );

            /* olleh packet */
            bool prepare_olleh( const ecdh_key & my_public_key,
                                bool need_login,
                                bool need_pass,
                                const bignum & my_private_key,
                                const ecdh_key & peer_public_key,
                                msg & m );
        };

        class hello_srv
        {
          public:
            bool start();
            bool stop();

            hello_srv();
            virtual ~hello_srv();

            /* addresses */
            SAI addr()               { return receiver_.addr(); }
            void addr(const SAI & a) { receiver_.addr(a); }

            /* private key */
            const bignum & private_key() const { return handler_.private_key(); }
            void private_key(const bignum & v) { handler_.private_key(v);       }

            /* public key */
            const ecdh_key & public_key() const { return handler_.public_key(); }
            void public_key(const ecdh_key & v) { handler_.public_key(v);       }

            /* valid key callback */
            void valid_key_cb(valid_key_callback & cb) { handler_.valid_key_cb_ = &cb; }

            /* hello callback */
            void hello_cb(hello_callback & cb) { handler_.hello_cb_ = &cb; }

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
            hello_handler   handler_;

            /* internal */
            bool use_exc_;
            bool debug_;

            /* thread pool */
            unsigned int min_threads_;
            unsigned int max_threads_;
            unsigned int timeout_ms_;
            unsigned int retries_;
        };

        class hello_cli
        {
          public:
            bool hello( unsigned int timeout_ms=0 );

            hello_cli();
            virtual ~hello_cli();

          private:
            /* internal */
            bool      use_exc_;
            bool      debug_;

            SAI        addr_;
            int        sock_;
            bignum     private_key_;
            ecdh_key   public_key_;
            ecdh_key   server_public_key_;
            bool       need_login_;
            bool       need_pass_;

            bool init();

          public:

            /* pkt manipulation */
            bool prepare_hello( msg & m );
            bool init_olleh( const msg & m );

            /* hello_addr */
            inline const SAI & addr() const { return addr_; }
            inline void addr(const SAI & a) { addr_ = a;    }

            /* private key */
            inline const bignum & private_key() const { return private_key_; }
            inline void private_key(const bignum & v) { private_key_ = v;    }

            /* public key */
            inline const ecdh_key & public_key() const { return public_key_; }
            inline void public_key(const ecdh_key & v) { public_key_ = v;    }

            /* server_public_key */
            inline const ecdh_key & server_public_key() const { return server_public_key_; }

            /* need login ? */
            inline bool need_login() const { return need_login_; }

            /* need pass ? */
            inline bool need_pass() const  { return need_pass_;  }

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
#endif /* _csl_comm_udp_hello_hh_included_ */
