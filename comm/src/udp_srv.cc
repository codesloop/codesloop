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

#include "exc.hh"
#include "pbuf.hh"
#include "xdrbuf.hh"
#include "udp_srv.hh"
#include "udp_pkt.hh"
#include "common.h"
#include <sys/types.h>
#include <string.h>


namespace csl
{
  namespace comm
  {
    using common::pbuf;
    using common::xdrbuf;

    bool udp_srv::start()
    {
      /* private_key has been set ? */
      if( private_key_.is_empty() ) { THR(exc::rs_privkey_empty,exc::cm_udp_srv,false); }

      /* server_info_.public_key has been set ? */
      if( server_info_.public_key().is_empty() ) { THR(exc::rs_pubkey_empty,exc::cm_udp_srv,false); }

      /* already started ( TODO may be finished too ) */
      //if( hello_thread_.is_started() || auth_thread_.is_started() || data_thread_.is_started() )
      //{
      //  THR(exc::rs_already_started,exc::cm_udp_srv,false);
      //}

      /* init threads */
      //hello_thread_.set_entry(hello_entry_);
      auth_thread_.set_entry(auth_entry_);
      data_thread_.set_entry(data_entry_);

      /* init entries */
      //if( !hello_entry_.start() || !auth_entry_.start() || !data_entry_.start() )
      //{
      //  return false;
      //}

      /* start hello thread */
      //if( hello_thread_.start() == false ) { THR(exc::rs_thread_start,exc::cm_udp_srv,false); }

      /* start auth thread */
      if( auth_thread_.start() == false ) { THR(exc::rs_thread_start,exc::cm_udp_srv,false); }

      /* start data thread */
      if( data_thread_.start() == false ) { THR(exc::rs_thread_start,exc::cm_udp_srv,false); }

      return ( /*hello_thread_.start_event().wait( 3000 ) &&*/
          data_thread_.start_event().wait( 3000 ) &&
          auth_thread_.start_event().wait( 3000 ) );
    }

    /* internal function to be called on new, authenticated clients */
    bool udp_srv::on_accept_auth( udp_pkt & pkt, SAI & addr )
    {
      /*  register authenticated client, its roles are:
          - generate server random
          - set salt for response
          - register client (including its random keys)
      */
      // TODO
      return true;
    }

    bool udp_srv::on_data_arrival( udp_pkt & pkt, udp_pkt::b1024_t & dta, SAI & addr )
    {
      // TODO
      return true;
    }

    udp_srv::udp_srv()
    : use_exc_(true), /*hello_entry_(*this),*/ auth_entry_(*this), data_entry_(*this)
    {
    }

    udp_srv::~udp_srv()
    {
      //hello_entry_.stop_me();
      auth_entry_.stop_me();
      data_entry_.stop_me();

      //hello_thread_.exit_event().wait( 1500 );
      data_thread_.exit_event().wait( 1500 );
      auth_thread_.exit_event().wait( 1500 );
    }

    /* private: not used */
    udp_srv::udp_srv(const udp_srv & other)
    : use_exc_(true), /*hello_entry_(*this),*/ auth_entry_(*this), data_entry_(*this)
    {
    }

    void udp_srv::valid_key_cb(cb::valid_key & c)
    {
      //hello_entry_.valid_key_cb( c );
      auth_entry_.valid_key_cb( c );
    }

    //void udp_srv::hello_cb(cb::hello & c)
    //{
    //  hello_entry_.hello_cb( c );
    //}

    void udp_srv::valid_creds_cb(cb::valid_creds & c)
    {
      auth_entry_.valid_creds_cb( c );
    }

    void udp_srv::create_session_cb(cb::create_session & c)
    {
      create_session_cb_ = &c;
    }

    void udp_srv::cleanup_session_cb(cb::cleanup_session & c)
    {
      cleanup_session_cb_ = &c;
    }

    void udp_srv::data_arrival_cb(cb::data_arrival & c)
    {
      data_arrival_cb_ = &c;
    }

    //const udp_srv::SAI & udp_srv::hello_addr() const { return hello_entry_.addr(); }
    const udp_srv::SAI & udp_srv::auth_addr() const  { return auth_entry_.addr();  }
    const udp_srv::SAI & udp_srv::data_addr() const  { return data_entry_.addr();  }

    //void udp_srv::hello_addr(const udp_srv::SAI & a) { return hello_entry_.addr(a); }
    void udp_srv::auth_addr(const udp_srv::SAI & a)  { return auth_entry_.addr(a);  }
    void udp_srv::data_addr(const udp_srv::SAI & a)  { return data_entry_.addr(a);  }
  };
};

/* EOF */
