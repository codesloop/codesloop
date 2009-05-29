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

#include "exc.hh"
#include "thread.hh"
#include "mutex.hh"
#include "cb.hh"
#include "bignum.hh"
#include "udp_srv_info.hh"
//#include "udp_hello_entry.hh"
#include "udp_auth_entry.hh"
#include "udp_data_entry.hh"
#include "udp_pkt.hh"
#include "common.h"
#ifdef __cplusplus
#include <string>

namespace csl
{
  using sec::bignum;
  using std::string;
  using nthread::thread;
  using nthread::mutex;

  namespace comm
  {
    class udp_srv
    {
      public:
        /* typedefs */
        typedef struct sockaddr_in SAI;

        /* interface */
        bool start();

        udp_srv();
        virtual ~udp_srv();

      private:
        /* internal */
        bool use_exc_;

        /* user should fill these */
        bignum             private_key_;
        udp_srv_info       server_info_;

        /* worker threads */
        //thread hello_thread_;
        thread auth_thread_;
        thread data_thread_;

        /* worker entries */
        //udp_hello_entry hello_entry_;
        udp_auth_entry  auth_entry_;
        udp_data_entry  data_entry_;

        /* session callbacks */
        cb::create_session   * create_session_cb_;
        cb::cleanup_session  * cleanup_session_cb_;
        cb::data_arrival     * data_arrival_cb_;

      public:
        /* accessors and manipulators */
        void valid_key_cb(cb::valid_key & c);
        //void hello_cb(cb::hello & c);
        void valid_creds_cb(cb::valid_creds & c);

        void create_session_cb(cb::create_session & c);
        void cleanup_session_cb(cb::cleanup_session & c);
        void data_arrival_cb(cb::data_arrival & c);

        /* addresses */
        //const SAI & hello_addr() const;
        const SAI & auth_addr() const;
        const SAI & data_addr() const;

        //void hello_addr(const SAI & a);
        void auth_addr(const SAI & a);
        void data_addr(const SAI & a);

        /* private key */
        inline const bignum & private_key() const { return private_key_; }
        inline void private_key(const bignum & v) { private_key_ = v; }

        /* info */
        inline const udp_srv_info & server_info() const { return server_info_; }
        inline void server_info(const udp_srv_info & v) { server_info_ = v; }

        /* use exceptions ? */
        inline void use_exc(bool yesno) { use_exc_ = yesno; }
        inline bool use_exc() const     { return use_exc_; }

      private:
        friend class udp_auth_entry;
        friend class udp_data_entry;

        struct session // TODO
        {
          session_data *    sessdata_;
          udp_chann         chann_;
          udp_pkt::b1024_t  data_;
        };

        /* internal functions */
        bool on_accept_auth( udp_pkt & pkt, SAI & addr );
        bool on_data_arrival( udp_pkt & pkt, udp_pkt::b1024_t & dta, SAI & addr );

        /* no copy */
        udp_srv(const udp_srv & other);
        inline udp_srv & operator=(const udp_srv & other) { return *this; }
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_comm_udp_srv_hh_included_ */
