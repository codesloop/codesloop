/*
Copyright (c) 2008,2009, CodeSLoop Team

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

/**
   @file t__udp_data_server.cc
   @brief Tests to verify udp::data_srv routines
 */

#include "codesloop/comm/udp_hello.hh"
#include "codesloop/comm/udp_auth.hh"
#include "codesloop/comm/udp_data.hh"
#include "codesloop/common/test_timer.h"
#include "codesloop/common/common.h"
#include "codesloop/common/str.hh"
#include "codesloop/common/str.hh"
#include <assert.h>
#include <vector>
#include <map>

using namespace csl::common;
using namespace csl::comm;
using namespace csl::sec;
using namespace csl::nthread;

/** @brief contains tests related to udp data servers */
namespace test_udp_data_server {

  struct SessionStore
  {
    typedef std::vector<unsigned char>    ucharvec_t;
    typedef std::map<ucharvec_t,ustr>     map_t;

    mutex mtx_;
    map_t m_;
  };

  class RegisterCB : public udp::register_auth_callback
  {
    public:
      SessionStore * sessions_;
      RegisterCB(SessionStore & s) : sessions_(&s) {}

      bool operator()( const SAI & addr,
                       const ustr & login,
                       const ustr & pass,
                       const ustr & session_key,
                       const udp::saltbuf_t & peer_salt,
                       udp::saltbuf_t & my_salt )
      {
        const unsigned char * dp = peer_salt.data();
        SessionStore::ucharvec_t v(dp,dp+peer_salt.size());

        {
          scoped_mutex m(sessions_->mtx_);
          (sessions_->m_)[v] = session_key;
        }

        return true;
      }
  };

  class LookupCB : public udp::lookup_session_callback
  {
    public:
      SessionStore * sessions_;
      LookupCB(SessionStore & s) : sessions_(&s) {}

      bool operator()( const udp::saltbuf_t & old_salt,
                       const SAI & addr,
                       ustr & sesskey )
      {
        const unsigned char * dp = old_salt.data();
        SessionStore::ucharvec_t v(dp,dp+old_salt.size());

        {
          scoped_mutex m(sessions_->mtx_);
          SessionStore::map_t::iterator it = sessions_->m_.find(v);
          if( it == sessions_->m_.end() )
          {
            return false;
          }
          sesskey = (*it).second;
        }

        return true;
      }
  };

  class HandleCB : public udp::handle_data_callback
  {
    public:
      SessionStore * sessions_;
      HandleCB(SessionStore & s) : sessions_(&s) {}

      bool operator()( const udp::saltbuf_t & old_salt,  // in: from request
                       const udp::saltbuf_t & new_salt,  // in: generated
                       const SAI & addr,                 // in: from request
                       const ustr & sesskey,             // in: looked up in cb
                       int sock,                         // in: from handler
                       const udp::b1024_t & data )       // in: decrypted from request
      {
        assert( memcmp( data.data(),"hello",6 ) == 0 );
        udp::b1024_t rep;
        rep.set( reinterpret_cast<const unsigned char *>("HELLO"),6);
        return send_reply(old_salt, new_salt, addr, sesskey, sock, rep);
      }
  };

  class UpdateCB : public udp::update_session_callback
  {
    public:
      SessionStore * sessions_;
      UpdateCB(SessionStore & s) : sessions_(&s) {}

      bool operator()( const udp::saltbuf_t & old_salt,          // in
                       const udp::saltbuf_t & new_salt,          // in
                       const SAI & addr,                         // in
                       const ustr & sesskey )                    // in
      {
        const unsigned char * dp = old_salt.data();
        SessionStore::ucharvec_t v(dp,dp+old_salt.size());

        const unsigned char * np = new_salt.data();
        SessionStore::ucharvec_t n(np,np+new_salt.size());

        {
          scoped_mutex m(sessions_->mtx_);
          SessionStore::map_t::iterator it = sessions_->m_.find(v);
          if( it != sessions_->m_.end() )
          {
            sessions_->m_.erase( it );
          }
          (sessions_->m_)[n] = sesskey;
        }

        return true;
      }
  };

  void basic()
  {
    udp::hello_srv hellosrv;
    udp::auth_srv authsrv;
    udp::data_srv datasrv;
    SAI h,a,d;

    hellosrv.use_exc(false);
    authsrv.use_exc(false);
    authsrv.debug(false);

    memset( &h,0,sizeof(h) );
    h.sin_family       = AF_INET;
    h.sin_addr.s_addr  = htonl(INADDR_LOOPBACK);
    h.sin_port         = htons(47781);

    d = a = h;
    a.sin_port = htons(47782);
    d.sin_port = htons(47783);

    hellosrv.addr(h);
    authsrv.addr(a);
    datasrv.addr(d);

    ecdh_key pubkey;
    bignum   privkey;

    pubkey.algname("prime192v3");

    /* generate keypair */
    assert( pubkey.gen_keypair(privkey) == true );

    authsrv.private_key(privkey);
    authsrv.public_key(pubkey);

    hellosrv.private_key(privkey);
    hellosrv.public_key(pubkey);

    SessionStore  store;
    RegisterCB    rcb(store);
    LookupCB      lcb(store);
    HandleCB      hcb(store);
    UpdateCB      ucb(store);

    authsrv.register_auth_cb( rcb );
    datasrv.lookup_session_cb( lcb );
    datasrv.handle_data_cb( hcb );
    datasrv.update_session_cb( ucb );

    assert( hellosrv.start() );
    assert( authsrv.start() );
    assert( datasrv.start() );

    SleepSeconds( 60 );
  }

} // end of test_udp_data_server

using namespace test_udp_data_server;

int main()
{
  basic();
  return 0;
}

/* EOF */
