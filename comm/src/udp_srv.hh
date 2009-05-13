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

#ifndef _csl_comm_udp_srv_hh_included_
#define _csl_comm_udp_srv_hh_included_

#include "thread.hh"
#include "cb.hh"
#include "ecdh_key.hh"
#include "bignum.hh"
#include "udp_srv_info.hh"
#include "common.h"
#ifdef __cplusplus
#include <string>

namespace csl
{
  using sec::ecdh_key;
  using sec::bignum;
  using std::string;
  using nthread::thread;

  namespace comm
  {
    class udp_srv
    {
      public:

        bool start();

        inline udp_srv()
        : port_(0), valid_key_cb_(0), hello_cb_(0), valid_creds_cb_(0), server_entry_(*this) {}

      private:

        string             host_;
        unsigned short     port_;
        bignum             private_key_;
        udp_srv_info       server_info_;
        /* callbacks */
        cb::valid_key    * valid_key_cb_;
        cb::hello        * hello_cb_;
        cb::valid_creds  * valid_creds_cb_;

        /* server thread */
        class server_entry : public thread::callback
        {
          public:
            virtual void operator()(void);
            virtual ~server_entry();

            inline server_entry(udp_srv & srv) : srv_(&srv), socket_(-1) {}

            udp_srv * srv_;
            int       socket_;

          private:
            /* no default construct */
            server_entry() {}
        };

        thread             server_thread_;
        server_entry       server_entry_;

        /* no copy */
        inline udp_srv(const udp_srv & other) : server_entry_(*this) {}
        inline udp_srv & operator=(const udp_srv & other) { return *this; }

      public:

        /* inline accessors and manipulators */
        void valid_key_cb(cb::valid_key & c)      { valid_key_cb_ = &c;    }
        void hello_cb(cb::hello & c)              { hello_cb_ = &c;        }
        void valid_creds_cb(cb::valid_creds & c)  { valid_creds_cb_ = &c;  }

        /* host */
        inline const std::string & host() const { return host_; }
        inline void host(const std::string & v) { host_ = v; }

        /* port */
        inline unsigned short port() const { return port_; }
        inline void port(unsigned short v) { port_ = v; }

        /* private key */
        inline const bignum & private_key() const { return private_key_; }
        inline void private_key(const bignum & v) { private_key_ = v; }

        /* info */
        inline const udp_srv_info & server_info() const { return server_info_; }
        inline void server_info(const udp_srv_info & v) { server_info_ = v; }
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_comm_udp_srv_hh_included_ */
