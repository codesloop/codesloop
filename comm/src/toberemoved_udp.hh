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

#ifndef _csl_comm_udp_hh_included_
#define _csl_comm_udp_hh_included_

#include "event.hh"
#include "thread.hh"
#include "thrpool.hh"
#include "mutex.hh"
#include "circbuf.hh"
#include "bignum.hh"
#include "ecdh_key.hh"
#include "tbuf.hh"
#include "common.h"
#ifdef __cplusplus
#include <string>

namespace csl
{
  using nthread::event;
  using nthread::mutex;
  using nthread::thread;
  using nthread::thrpool;

  using sec::ecdh_key;
  using sec::bignum;

  using std::string;

  namespace comm
  {
    class udp
    {
      public:
        /* typedefs */
        typedef struct sockaddr_in SAI;

        struct msg
        {
          enum { max_len_v = 65536 };
          unsigned char  data_[max_len_v];
          unsigned int   size_;
          SAI            sender_;

          inline unsigned int max_len() { return max_len_v; }
        };

        struct srv_info
        {
          ecdh_key  public_key_;
          bool      need_login_;
          bool      need_pass_;

          inline srv_info() : need_login_(true), need_pass_(true) {}
        };

        class session_data
        {
          public:
        };

        class pkt
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
              salt_p
            };

            enum {
              max_length_v = 65496,
              max_salt_v = 8
            };

            typedef common::tbuf<1024> b1024_t;

            inline unsigned char * data() { return data_; }
            inline unsigned int maxlen()  { return max_length_v; }
            inline unsigned int maxsalt() { return max_salt_v; }

            /* hello packet */
            bool init_hello(unsigned int len);
            unsigned char * prepare_hello(unsigned int & len);

            /* olleh packet */
            bool init_olleh(unsigned int len);
            unsigned char * prepare_olleh(unsigned int & len);

            /* auth packet */
            bool init_uc_auth(unsigned int len);
            unsigned char * prepare_uc_auth(unsigned int & len);

            /* htua packet */
            bool init_uc_htua(unsigned int len);
            unsigned char * prepare_uc_htua(unsigned int & len);

            /* data packet */
            bool init_data( unsigned int len,
                            b1024_t & data ); // process received

            unsigned char * prepare_data( const b1024_t & data, // prepare before send
                                          unsigned int & len );

            /* salt packet */
            bool init_salt(unsigned int len); // TODO
            unsigned char * prepare_salt(unsigned int & len); // TODO

            /* variables */
            void peer_pubkey(const ecdh_key & pk);
            ecdh_key & peer_pubkey();

            void own_pubkey(const ecdh_key & pk);
            ecdh_key & own_pubkey();
            const ecdh_key & own_pubkey_const() const;

            void server_info(const srv_info & info);
            srv_info & server_info();
            const srv_info & server_info_const() const;

            void own_privkey(const bignum & pk);
            bignum & own_privkey();
            const bignum & own_privkey_const() const;

            const std::string & login() const;
            void login(const std::string & l);

            const std::string & pass() const;
            void pass(const std::string & p);

            unsigned long long * newsalt();
            unsigned long long * salt();

            const std::string & session_key() const;
            void session_key(const std::string & k);

            virtual inline ~pkt() {}
            pkt();

          private:
            srv_info               info_;
            bignum                 own_privkey_;
            ecdh_key               own_pubkey_;
            ecdh_key               peer_pubkey_;
            std::string            login_;
            std::string            pass_;
            unsigned long long     newsalt_;
            unsigned long long     salt_;
            std::string            session_key_;
            unsigned char          data_[max_length_v];
            bool                   use_exc_;
            bool                   debug_;

          public:
            inline bool use_exc() const      { return use_exc_;  }
            inline void use_exc(bool yesno)  { use_exc_ = yesno; }

            inline bool debug() const      { return debug_; }
            inline void debug(bool yesno)  { debug_ = yesno; }
        };

        class recvr : public thread::callback
        {
          public:
            class msgs : public common::circbuf<msg,30>
            {
              public:
                virtual void on_new_item() { ev_.notify(); }
                virtual ~msgs() { }

                mutex   mtx_;
                event   ev_;
            };

            class msg_handler : public thread::callback
            {
              public:
                /* this must lock/unlock msgs_.mtx_ */
                virtual void operator()(void) = 0;

                virtual void set_msgs(msgs & m);

                inline msg_handler() : msgs_(0), debug_(false), use_exc_(false), socket_(-1) {}

                /* debug ? */
                inline bool debug() const      { return debug_; }
                inline void debug(bool yesno)  { debug_ = yesno; }

                /* use exceptions ? */
                inline bool use_exc() const      { return use_exc_; }
                inline void use_exc(bool yesno)  { use_exc_ = yesno; }

                /* packet */
                void init_pkt(const bignum & privk, const srv_info & info, int sock);

              protected:
                mutex  mtx_;
                msgs * msgs_;
                bool   debug_;
                bool   use_exc_;
                pkt    pkt_;
                int    socket_;
            };

          private:
            thrpool       thread_pool_;
            msgs          msgs_;
            SAI           addr_;
            int           socket_;
            bool          stop_me_;
            bool          use_exc_;
            bool          debug_;
            mutex         mtx_;

          public:
            inline int socket() { return socket_; }

            bool start( unsigned int min_threads,
                        unsigned int max_threads,
                        unsigned int timeout_ms,
                        unsigned int attempts,
                        msg_handler & cb );

            bool stop();

            SAI addr();
            void addr(const SAI & a);

            bool use_exc();
            void use_exc(bool yesno);

            bool stop_me();
            void stop_me(bool yesno);

            virtual void operator()(void);
            virtual ~recvr();
            recvr();

            /* debug ? */
            inline void debug(bool yesno) { debug_ = yesno; }
            inline bool debug() const     { return debug_;  }
        };

        class chann
        {
          public:
        };

        class valid_key
        {
          public:
            virtual ~valid_key() {}
            virtual bool operator()( const ecdh_key & peer_public_key ) = 0; // in
        };

        class hello
        {
          public:
            virtual ~hello() {}
            virtual bool operator()( const ecdh_key & peer_public_key,  // in
                                     const SAI & addr,                  // in
                                     srv_info & info,                   // out
                                     bignum & my_private_key ) = 0;     // out
        };

        class valid_creds
        {
          public:
            virtual ~valid_creds() {}
            virtual bool operator()( const ecdh_key & peer_public_key,    // in
                                     const SAI & addr,                    // in
                                     const string & login,                // in
                                     const string & pass) = 0;            // in
        };

        class create_session
        {
          public:
            virtual ~create_session() {}
            virtual session_data * operator()( const ecdh_key & peer_public_key,  // in
                                               const ecdh_key & own_public_key,   // in
                                               const bignum & own_private_key,    // in
                                               const SAI & addr,                  // in
                                               const string & login,              // in
                                               const string & pass,               // in
                                               const string & session_key ) = 0;  // in
        };

        class data_arrival
        {
          public:
            typedef common::tbuf<1024> b1024_t;

            virtual ~data_arrival() {}
            virtual bool operator()( session_data * sess,         // in
                                     chann & channel_,            // in
                                     const b1024_t & data ) = 0;  // in
        };

        class cleanup_session
        {
          public:
            virtual ~cleanup_session() {}
            virtual void operator()( session_data * sess ) = 0;   // in
        };

        class hello_handler : public recvr::msg_handler
        {
          public:
            virtual void operator()(void);
            virtual ~hello_handler() {}

            valid_key * valid_key_cb_;
            hello     * hello_cb_;

            hello_handler() : valid_key_cb_(0), hello_cb_(0) {}
        };

        class auth_handler : public recvr::msg_handler
        {
          public:
            virtual void operator()(void);
            virtual ~auth_handler() {}

            valid_key * valid_key_cb_;
            auth_handler() : valid_key_cb_(0) {}
        };

        class data_handler : public recvr::msg_handler
        {
          public:
            virtual void operator()(void);
            virtual ~data_handler() {}
        };

        class srv
        {
          public:
            bool start();
            bool stop();

            srv();
            virtual ~srv();

            /* accessors and manipulators */
            void valid_key_cb(valid_key & c);
            void hello_cb(hello & c);
            void valid_creds_cb(valid_creds & c);

            void create_session_cb(create_session & c);
            void cleanup_session_cb(cleanup_session & c);
            void data_arrival_cb(data_arrival & c);

            /* private key */
            inline const bignum & private_key() const { return private_key_; }
            inline void private_key(const bignum & v) { private_key_ = v; }

            /* info */
            inline const srv_info & server_info() const { return public_info_; }
            inline void server_info(const srv_info & v) { public_info_ = v; }

            /* addresses */
            SAI hello_addr() { return hello_receiver_.addr(); }
            SAI auth_addr()  { return auth_receiver_.addr();  }
            SAI data_addr()  { return data_receiver_.addr();  }

            void hello_addr(const SAI & a) { hello_receiver_.addr(a); }
            void auth_addr(const SAI & a)  { auth_receiver_.addr(a);  }
            void data_addr(const SAI & a)  { data_receiver_.addr(a);  }

            /* use exceptions ? */
            inline void use_exc(bool yesno) { use_exc_ = yesno; }
            inline bool use_exc() const     { return use_exc_;  }

            /* debug ? */
            inline void debug(bool yesno) { debug_ = yesno; }
            inline bool debug() const     { return debug_;  }

          private:
            /* */
            bignum    private_key_;
            srv_info  public_info_;

            /* receiver threads */
            thread    hello_thread_;
            thread    auth_thread_;
            thread    data_thread_;

            /* receivers */
            recvr     hello_receiver_;
            recvr     auth_receiver_;
            recvr     data_receiver_;

            /* handlers */
            hello_handler   hello_handler_;
            auth_handler    auth_handler_;
            data_handler    data_handler_;

            /* session callbacks */
            create_session   * create_session_cb_;
            cleanup_session  * cleanup_session_cb_;
            data_arrival     * data_arrival_cb_;

            /* internal */
            bool use_exc_;
            bool debug_;
        };

        class cli
        {
          public:
          private:
            bignum    private_key_;
            ecdh_key  public_key_;
            srv_info  srv_info_;
        };
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_comm_udp_hh_included_ */
