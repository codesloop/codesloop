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

#ifndef _csl_comm_udp_data_hh_included_
#define _csl_comm_udp_data_hh_included_

#include "udp_auth.hh"
#include "tbuf.hh"
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

  using common::tbuf;

  using std::string;

  namespace comm
  {
    namespace udp
    {
#ifndef CSL_COMM_B1024_T_DEFINED
#define CSL_COMM_B1024_T_DEFINED
      typedef tbuf<1024> b1024_t;
#endif /* CSL_COMM_B1024_T_DEFINED */

      class lookup_session_callback
      {
        public:
          virtual ~lookup_session_callback() {}
          /* this is what we need to decode the incoming packet */
          virtual bool operator()( const saltbuf_t & old_salt,       // in
                                   const SAI & addr,                 // in
                                   std::string & sesskey ) = 0;      // out
      };

      class handle_data_callback
      {
        public:
          virtual ~handle_data_callback() {}

          virtual bool send_reply( const saltbuf_t & old_salt,
                                   const saltbuf_t & new_salt,
                                   const SAI & addr,
                                   const std::string & sesskey,
                                   int sock,
                                   const b1024_t & data );

          virtual bool operator()( const saltbuf_t & old_salt,  // in: from request
                                   const saltbuf_t & new_salt,  // in: generated
                                   const SAI & addr,            // in: from request
                                   const std::string & sesskey, // in: looked up in cb
                                   int sock,                    // in: from handler
                                   const b1024_t & data ) = 0;  // in: decrypted from request
      };

      class update_session_callback
      {
        public:
          virtual ~update_session_callback() {}
          virtual bool operator()( const saltbuf_t & old_salt,          // in
                                   const saltbuf_t & new_salt,          // in
                                   const SAI & addr,                    // in
                                   const std::string & sesskey ) = 0;   // in
      };

      class data_handler : public recvr::msg_handler
      {
        public:
          enum { salt_size_v = saltbuf_t::preallocated_size };

          virtual void operator()(void);
          virtual ~data_handler() {}

          lookup_session_callback  * lookup_session_cb_;
          handle_data_callback     * handle_data_cb_;
          update_session_callback  * update_session_cb_;

          data_handler() : lookup_session_cb_(0), handle_data_cb_(0), update_session_cb_(0) {}

          /* data packet */
          bool get_salt( saltbuf_t & old_salt,    // received in packet header
                         const msg & m );

          bool init_data( saltbuf_t & new_salt,    // received in encrypted part
                          const string & sesskey,  // needed for decrypt packet
                          const msg & m,           // the messages as received
                          b1024_t & recvdta );     // the decrypted data

          /* data packet */
          bool prepare_data( const saltbuf_t & old_salt,
                             const saltbuf_t & new_salt,
                             const string & sesskey,
                             const b1024_t & senddta,
                             msg & m );
      };

      class data_srv
      {
        public:
          bool start();
          bool stop();

          data_srv();
          virtual ~data_srv();

          /* addresses */
          SAI addr()               { return receiver_.addr(); }
          void addr(const SAI & a) { receiver_.addr(a); }

          /* lookup session data including session key to be used for decrypt packet */
          void lookup_session_cb(lookup_session_callback & cb)  { handler_.lookup_session_cb_ = &cb; }

          /* handle incoming, previously unencrypted data */
          void handle_data_cb(handle_data_callback & cb)        { handler_.handle_data_cb_ = &cb; }

          /* update session with the new salts to be used */
          void update_session_cb(update_session_callback & cb)  { handler_.update_session_cb_ = &cb; }

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
          data_handler    handler_;

          /* internal */
          bool use_exc_;
          bool debug_;

          /* thread pool */
          unsigned int min_threads_;
          unsigned int max_threads_;
          unsigned int timeout_ms_;
          unsigned int retries_;
      };

      class data_cli
      {
        public:
          bool send(const b1024_t & data);
          bool recv(b1024_t & data,unsigned int timeout_ms=0);

          data_cli();
          virtual ~data_cli();

        private:
          /* internal */
          bool         use_exc_;
          bool         debug_;

          SAI          addr_;
          int          sock_;

          saltbuf_t    server_salt_;
          saltbuf_t    my_salt_;
          string       session_key_;

          bool init();

        public:
          /* server salt */
          inline const saltbuf_t & server_salt() const { return server_salt_; }
          inline void server_salt(const saltbuf_t & s) { server_salt_ = s;    }

          /* own salt */
          inline const saltbuf_t & my_salt() const { return my_salt_; }
          inline void my_salt(const saltbuf_t & s) { my_salt_ = s;    }

          /* session_key */
          inline const string & session_key() const { return session_key_; }
          inline void session_key(const string & s) { session_key_ = s;    }

          /* auth_addr */
          inline const SAI & addr() const { return addr_; }
          inline void addr(const SAI & a) { addr_ = a;    }

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
#endif /* _csl_comm_udp_data_hh_included_ */
