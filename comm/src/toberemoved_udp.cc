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
#include "udp.hh"
#include "mutex.hh"
#include "crypt_pkt.hh"
#include "csl_sec.h"
#include "common.h"

namespace csl
{
  using namespace nthread;
  using namespace sec;

  namespace
  {
    static void print_hex(const char * prefix,const void * vp,size_t len)
    {
      unsigned char * hx = (unsigned char *)vp;
      printf("%s [%04d] : ",prefix,len);
      for(size_t i=0;i<len;++i) printf("%.2X",hx[i]);
      printf("\n");
    }
  }

  namespace comm
  {

    bool udp::recvr::start( unsigned int min_threads,
                            unsigned int max_threads,
                            unsigned int timeout_ms,
                            unsigned int attempts,
                            msg_handler & cb )
    {
      /* init thread pool and handlers */
      cb.set_msgs(msgs_);

      if( !thread_pool_.init( min_threads, max_threads,
                              timeout_ms, attempts,
                              msgs_.ev_, cb ) ) return false;

      /* init udp */
      if( socket_ != -1 ) { ShutdownCloseSocket(socket_); }

      int sock = ::socket( AF_INET, SOCK_DGRAM, 0 );
      if( sock <= 0 ) { THRC(exc::rs_socket_failed,exc::cm_udp_recvr,false); }

      /* copy addr to temporary */
      SAI addr = this->addr();

      /* bind socket */
      if( ::bind(sock,(struct sockaddr *)&addr, sizeof(addr)) )
      {
        ShutdownCloseSocket( sock );
        THRC(exc::rs_bind_failed,exc::cm_udp_recvr,false);
      }

      /* check internal address */
      socklen_t len = sizeof(addr);

      if( ::getsockname(sock,(struct sockaddr *)&addr,&len) )
      {
        ShutdownCloseSocket( sock );
        THRC(exc::rs_getsockname_failed,exc::cm_udp_recvr,false);
      }

      /* copy the address back, in case of OS chosen port/address */
      this->addr( addr );
      socket_ = sock;

      return true;
    }

    void udp::recvr::operator()(void)
    {
      unsigned long timeout_ms    = thread_pool_.timeout();
      unsigned long timeout_sec   = timeout_ms/1000;
      unsigned long timeout_usec  = (timeout_ms%1000)*1000;

      /* packet loop */
      while( stop_me() == false )
      {
        /* wait for incoming packets */
        fd_set fds;
        FD_ZERO( &fds );
        FD_SET( socket_, &fds );
        struct timeval tv = { timeout_sec, timeout_usec };

        /* wait for new packet to arrive */
        int err = ::select( socket_+1,&fds,NULL,NULL,&tv );
        int recvd = 0;

        if( err < 0 )       { THRNORET(exc::rs_select_failed,exc::cm_udp_recvr); break; }
        else if( err == 0 ) { continue; }

        /* temporary lock messages for getting an entry from it */
        msg * tm = 0;
        {
          scoped_mutex mm(msgs_.mtx_);
          tm = &(msgs_.prepare());
        }
        msg & m(*tm);

        socklen_t len = sizeof(m.sender_);

        /* receive packet */
        recvd = recvfrom( socket_, (char *)m.data_, m.max_len(), 0, (struct sockaddr *)&(m.sender_), &len );

        if( recvd < 0 ) { THRNORET(exc::rs_recv_failed,exc::cm_udp_recvr); break; }
        else if( recvd == 0 )
        {
          m.size_ = 0;
          /* temporary lock messages for putting back unneccessary entry */
          {
            scoped_mutex mm(msgs_.mtx_);
            msgs_.rollback( m );
          }
          continue;
        }
        else
        {
          m.size_    = recvd;
          /* temporary lock messages for committing new message */
          {
            scoped_mutex mm(msgs_.mtx_);
            /*
            ** when commit() is called, the waiting threads in the thread pool will be
            ** signaled to check for the new message arrived
            */
            msgs_.commit( m );
          }
        }
      }

      if( thread_pool_.graceful_stop() == false )
      {
        thread_pool_.unpolite_stop();
      }
    }

    bool udp::recvr::stop()
    {
      stop_me(true);
      if( thread_pool_.graceful_stop() == false )
      {
        return thread_pool_.unpolite_stop();
      }
      else
      {
        return true;
      }
    }

    udp::recvr::~recvr()
    {
      if( socket_ > 0 ) ShutdownCloseSocket( socket_ );
      socket_ = -1;
    }

    udp::recvr::recvr() : socket_(-1), stop_me_(false), use_exc_(false), debug_(false)
    {
    }

    bool udp::recvr::use_exc()
    {
      bool ret = false;
      {
        scoped_mutex m(mtx_);
        ret = use_exc_;
      }
      return ret;
    }

    void udp::recvr::use_exc(bool yesno)
    {
      scoped_mutex m(mtx_);
      use_exc_ = yesno;
    }

    bool udp::recvr::stop_me()
    {
      bool ret = false;
      {
        scoped_mutex m(mtx_);
        ret = stop_me_;
      }
      return ret;
    }

    void udp::recvr::stop_me(bool yesno)
    {
      scoped_mutex m(mtx_);
      stop_me_ = yesno;
    }

    udp::SAI udp::recvr::addr()
    {
      SAI ret;
      {
        scoped_mutex m(mtx_);
        ret = addr_;
      }
      return ret;
    }

    void udp::recvr::addr(const udp::SAI & a)
    {
      scoped_mutex m(mtx_);
      addr_ = a;
    }

    void udp::recvr::msg_handler::set_msgs(msgs & m)
    {
      scoped_mutex mm(mtx_);
      msgs_ = &m;
    }

    void udp::recvr::msg_handler::init_pkt(const bignum & privk, const srv_info & info, int sock)
    {
      socket_ = sock;
      pkt_.own_privkey(privk);
      pkt_.own_pubkey(info.public_key_);
      pkt_.server_info(info);
    }

    udp::pkt::pkt() : use_exc_(true), debug_(false) {}

    /* internal buffer */

    /* public key */
    void udp::pkt::peer_pubkey(const ecdh_key & pk)     { peer_pubkey_ = pk; }
    ecdh_key & udp::pkt::peer_pubkey()                  { return peer_pubkey_; }

    void udp::pkt::own_pubkey(const ecdh_key & pk)      { own_pubkey_ = pk; }
    ecdh_key & udp::pkt::own_pubkey()                   { return own_pubkey_; }
    const ecdh_key & udp::pkt::own_pubkey_const() const { return own_pubkey_; }

    /* info */
    void udp::pkt::server_info(const udp::srv_info & info)    { info_ = info; }
    udp::srv_info & udp::pkt::server_info()                   { return info_; }
    const udp::srv_info & udp::pkt::server_info_const() const { return info_; }

    /* private key */
    void udp::pkt::own_privkey(const bignum & pk)      { own_privkey_ = pk; }
    bignum & udp::pkt::own_privkey()                   { return own_privkey_; }
    const bignum & udp::pkt::own_privkey_const() const { return own_privkey_; }

    /* login */
    const std::string & udp::pkt::login() const  { return login_; }
    void udp::pkt::login(const std::string & l)  { login_ = l; }

    /* pass */
    const std::string & udp::pkt::pass() const  { return pass_; }
    void udp::pkt::pass(const std::string & p)  { pass_ = p; }

    /* newsalt */
    unsigned long long * udp::pkt::newsalt() { return (unsigned long long *)&newsalt_; }

    /* salt */
    unsigned long long * udp::pkt::salt() { return (unsigned long long *)&salt_; }

    /* session key */
    const std::string & udp::pkt::session_key() const { return session_key_; }
    void udp::pkt::session_key(const std::string & k) { session_key_ = k; }

    /* hello packet */
    bool udp::pkt::init_hello(unsigned int len)
    {
      try
      {
        pbuf pb;
        pb.append(data_,len);
        xdrbuf xb(pb);

        int32_t packet_type;
        xb >> packet_type;

        if( debug() ) { printf(" -- [%ld] : packet_type : %d\n",xb.position(),packet_type ); }

        if( packet_type != hello_p ) { THR(comm::exc::rs_invalid_packet_type,comm::exc::cm_udp_pkt,false); }

        if( peer_pubkey_.from_xdr(xb) == false ) { THR(comm::exc::rs_xdr_error,comm::exc::cm_udp_pkt,false); }

        if( debug() ) { printf(" -- [%ld] : ",xb.position()); peer_pubkey_.print(); }
      }
      catch( common::exc e )
      {
        std::string s;
        e.to_string(s);
        fprintf(stderr,"Exception caught: %s\n",s.c_str());
        THR(comm::exc::rs_common_error,comm::exc::cm_udp_pkt,false);
      }
      return true;
    }

    unsigned char * udp::pkt::prepare_hello(unsigned int & len)
    {
      try
      {
        pbuf pb;
        xdrbuf xb(pb);

        xb << (int32_t)hello_p;

        if( debug() ) { printf(" ++ [%ld] : packet_type : %d\n",xb.position(),hello_p ); }

        if( own_pubkey_.to_xdr(xb) == false ) { THR(comm::exc::rs_xdr_error,comm::exc::cm_udp_pkt,false); }

        if( debug() ) { printf(" ++ [%ld] : ",xb.position()); own_pubkey_.print(); }

        pb.copy_to( data_,maxlen() );
        len = pb.size();
      }
      catch( common::exc e )
      {
        std::string s;
        e.to_string(s);
        fprintf(stderr,"Exception caught: %s\n",s.c_str());
        THR(comm::exc::rs_common_error,comm::exc::cm_udp_pkt,false);
      }
      return data_;
    }

    /* olleh packet */
    bool udp::pkt::init_olleh(unsigned int len)
    {
      try
      {
        /* unencrypted part */
        pbuf    outer;
        outer.append(data_,len);
        xdrbuf  xbo(outer);

        int32_t packet_type = 0;
        xbo >> packet_type;

        if( debug() ) { printf(" -- [%ld] : packet_type : %d\n",xbo.position(),packet_type ); }

        if( packet_type != olleh_p ) { THR(comm::exc::rs_invalid_packet_type,comm::exc::cm_udp_pkt,false); }

        if( peer_pubkey_.from_xdr(xbo) == false ) { THR(comm::exc::rs_xdr_error,comm::exc::cm_udp_pkt,false); }

        if( debug() ) { printf(" -- [%ld] : ",xbo.position()); peer_pubkey_.print(); }

        unsigned char * ptrp = data_ + xbo.position();
        unsigned int    lenp = len - xbo.position();

        /* encrypted part */

        /* generate session key */
        std::string session_key;

        if( !peer_pubkey_.gen_sha1hex_shared_key(own_privkey_,session_key) )
        {
          THR(comm::exc::rs_sec_error,comm::exc::cm_udp_pkt,false);
        }

        if( debug() ) { printf("  -- Session Key: '%s'\n",session_key.c_str()); }

        /* de-compile packet */
        crypt_pkt::keybuf_t   key;
        crypt_pkt::headbuf_t  head;
        crypt_pkt::databuf_t  data;
        crypt_pkt::footbuf_t  foot;

        head.set(ptrp,crypt_pkt::header_len());
        foot.set(ptrp+(lenp-crypt_pkt::footer_len()),crypt_pkt::footer_len());
        data.set(ptrp+(crypt_pkt::header_len()),
                 lenp-crypt_pkt::footer_len()-crypt_pkt::header_len());

        key.set((unsigned char *)session_key.c_str(),session_key.size()+1);

        crypt_pkt pk;

        if( pk.decrypt( key,head,data,foot ) == false )
        {
          THR(comm::exc::rs_crypt_pkt_error,comm::exc::cm_udp_pkt,false);
        }

        /* xdr deserialize */
        pbuf inner;
        inner.append( data.data(), data.size() );
        xdrbuf xbi(inner);

        int32_t need_login, need_pass;

        xbi >> need_login;

        if( debug() ) { printf("  -- [%ld] login: %d\n",xbi.position(),need_login); }

        xbi >> need_pass;

        if( debug() ) { printf("  -- [%ld] pass: %d\n",xbi.position(),need_pass); }

        info_.public_key_ = peer_pubkey_;
        info_.need_login_ = (need_login == 1);
        info_.need_pass_ = (need_pass == 1);

        return true;
      }
      catch( common::exc e )
      {
        std::string s;
        e.to_string(s);
        fprintf(stderr,"Exception caught: %s\n",s.c_str());
        THR(comm::exc::rs_common_error,comm::exc::cm_udp_pkt,false);
      }
      return false;
    }

    unsigned char * udp::pkt::prepare_olleh(unsigned int & len)
    {
      try
      {
        /* unencrypted part */
        pbuf    outer;
        xdrbuf  xbo(outer);

        xbo << (int32_t)olleh_p;

        if( debug() ) { printf(" ++ [%ld] : packet_type : %d\n",xbo.position(),olleh_p ); }

        /* ***
        hello callback may change the server info, and
        thus the public key based on the received public key
        */
        if( info_.public_key_.to_xdr(xbo) == false ) { THR(comm::exc::rs_xdr_error,comm::exc::cm_udp_pkt,false); }

        if( debug() ) { printf(" ++ [%ld] : ",xbo.position()); info_.public_key_.print(); }

        if( outer.size() > maxlen() ) { THR(comm::exc::rs_too_big,comm::exc::cm_udp_pkt,NULL); }

        outer.copy_to(data_,maxlen());

        /* encrypted part */
        pbuf inner;
        xdrbuf xbi(inner);

        xbi << (int32_t)info_.need_login_;
        xbi << (int32_t)info_.need_pass_;

        if( debug() ) { printf("  ++ login: %d pass: %d\n",info_.need_login_,info_.need_pass_); }

        /* generate session key */
        std::string session_key;

        /* ***
        hello callback may change the private key to be used
        */
        if( !peer_pubkey_.gen_sha1hex_shared_key(own_privkey_,session_key) )
        {
          THR(comm::exc::rs_sec_error,comm::exc::cm_udp_pkt,NULL);
        }

        if( debug() ) { printf("  ++ Session Key: '%s'\n",session_key.c_str()); }

        /* compile packet */
        crypt_pkt::saltbuf_t  salt;
        crypt_pkt::keybuf_t   key;
        crypt_pkt::headbuf_t  head;
        crypt_pkt::databuf_t  data;
        crypt_pkt::footbuf_t  foot;

        csl_sec_gen_rand( this->salt(), maxsalt() );
        salt.set( (unsigned char *)this->salt(), maxsalt() );
        key.set( (unsigned char *)session_key.c_str(), session_key.size()+1 );
        inner.t_copy_to( data );

        crypt_pkt pk;

        if( !pk.encrypt( salt,key,head,data,foot ) )
        {
          THR(comm::exc::rs_crypt_pkt_error,comm::exc::cm_udp_pkt,NULL);
        }

        /* output packet */
        unsigned char * outputp = data_+outer.size();
        unsigned int retlen = outer.size()+head.size()+data.size()+foot.size();

        if( retlen > maxlen() ) { THR(comm::exc::rs_too_big,comm::exc::cm_udp_pkt,NULL); }

        memcpy( outputp, head.data(), head.size() ); outputp += head.size();
        memcpy( outputp, data.data(), data.size() ); outputp += data.size();
        memcpy( outputp, foot.data(), foot.size() ); outputp += foot.size();

        if( debug() )
        {
          print_hex("  ++ OLLEH PROL ",data_,outer.size());
          print_hex("  ++ OLLEH HEAD ",head.data(),head.size() );
          print_hex("  ++ OLLEH DATA ",data.data(),data.size() );
          print_hex("  ++ OLLEH FOOT ",foot.data(),foot.size() );
        }

        /* return the data */
        len = retlen;
        return data_;
      }
      catch( common::exc e )
      {
        std::string s;
        e.to_string(s);
        fprintf(stderr,"Exception caught: %s\n",s.c_str());
        THR(comm::exc::rs_common_error,comm::exc::cm_udp_pkt,false);
      }

      return 0;
    }

    /* auth packet */
    bool udp::pkt::init_uc_auth(unsigned int len)
    {
      try
      {
        /* unencrypted part */
        pbuf    outer;
        outer.append(data_,len);
        xdrbuf  xbo(outer);

        int32_t packet_type = 0;
        xbo >> packet_type;

        if( debug() ) { printf(" -- [%ld] : packet_type : %d\n",xbo.position(),packet_type ); }

        if( packet_type != unicast_auth_p ) { THR(comm::exc::rs_invalid_packet_type,comm::exc::cm_udp_pkt,false); }

        if( peer_pubkey_.from_xdr(xbo) == false ) { THR(comm::exc::rs_xdr_error,comm::exc::cm_udp_pkt,false); }

        if( debug() ) { printf(" -- [%ld] : ",xbo.position()); peer_pubkey_.print(); }

        unsigned char * ptrp = data_ + xbo.position();
        unsigned int    lenp = len - xbo.position();

        /* encrypted part */

        /* generate session key */
        std::string session_key;

        if( !peer_pubkey_.gen_sha1hex_shared_key(own_privkey_,session_key) )
        {
          THR(comm::exc::rs_sec_error,comm::exc::cm_udp_pkt,false);
        }

        if( debug() ) { printf("  -- Session Key: '%s'\n",session_key.c_str()); }

        /* de-compile packet */
        crypt_pkt::keybuf_t   key;
        crypt_pkt::headbuf_t  head;
        crypt_pkt::databuf_t  data;
        crypt_pkt::footbuf_t  foot;

        head.set(ptrp,crypt_pkt::header_len());
        foot.set(ptrp+(lenp-crypt_pkt::footer_len()),crypt_pkt::footer_len());
        data.set(ptrp+(crypt_pkt::header_len()),
                 lenp-crypt_pkt::footer_len()-crypt_pkt::header_len());

        key.set((unsigned char *)session_key.c_str(),session_key.size()+1);

        crypt_pkt pk;

        if( pk.decrypt( key,head,data,foot ) == false )
        {
          THR(comm::exc::rs_crypt_pkt_error,comm::exc::cm_udp_pkt,false);
        }

        /* xdr deserialize */
        pbuf inner;
        inner.append( data.data(), data.size() );
        xdrbuf xbi(inner);

        unsigned int sz=0;

        if( xbi.get_data( (unsigned char *)this->newsalt(),sz,maxsalt() ) == false )
        {
          THR(comm::exc::rs_xdr_error,comm::exc::cm_udp_pkt,false);
        }

        if( sz != 8 )
        {
          THR(comm::exc::rs_xdr_error,comm::exc::cm_udp_pkt,false);
        }

        if( debug() ) { print_hex("  -- RAND ",this->newsalt(),maxsalt()); }

        xbi >> login_;

        if( debug() ) { printf("  -- [%ld] login: '%s'\n",xbi.position(),login_.c_str()); }

        xbi >> pass_;

        if( debug() ) { printf("  -- [%ld] pass: '%s'\n",xbi.position(),pass_.c_str()); }

        xbi >> session_key_;

        if( debug() ) { printf("  -- [%ld] sesskey: '%s'\n",xbi.position(),session_key_.c_str()); }

        return true;
      }
      catch( common::exc e )
      {
        std::string s;
        e.to_string(s);
        fprintf(stderr,"Exception caught: %s\n",s.c_str());
        THR(comm::exc::rs_common_error,comm::exc::cm_udp_pkt,false);
      }
      return false;
    }

    unsigned char * udp::pkt::prepare_uc_auth(unsigned int & len)
    {
      try
      {
        /* unencrypted part */
        pbuf    outer;
        xdrbuf  xbo(outer);

        xbo << (int32_t)unicast_auth_p;

        if( debug() ) { printf(" ++ [%ld] : packet_type : %d\n",xbo.position(),unicast_auth_p ); }

        if( own_pubkey_.to_xdr(xbo) == false ) { THR(comm::exc::rs_xdr_error,comm::exc::cm_udp_pkt,false); }

        if( debug() ) { printf(" ++ [%ld] : ",xbo.position()); own_pubkey_.print(); }

        if( outer.size() > maxlen() ) { THR(comm::exc::rs_too_big,comm::exc::cm_udp_pkt,NULL); }

        outer.copy_to(data_,maxlen());

        /* encrypted part */
        pbuf inner;
        xdrbuf xbi(inner);

        xbi << xdrbuf::bindata_t( (const unsigned char *)this->newsalt(),(unsigned int)maxsalt() );
        xbi << login_;
        xbi << pass_;
        xbi << session_key_;

        if( debug() )
        {
          printf("  ++ login: '%s' pass: '%s' sesskey: '%s'\n",
                 login_.c_str(),pass_.c_str(),session_key_.c_str());
        }

        /* generate session key */
        std::string session_key;

        if( peer_pubkey_.is_empty() == false )
        {
          if( peer_pubkey_.gen_sha1hex_shared_key(own_privkey_,session_key) == false )
          {
            THR(comm::exc::rs_sec_error,comm::exc::cm_udp_pkt,NULL);
          }
        }
        else if( info_.public_key_.is_empty() == false )
        {
          if( info_.public_key_.gen_sha1hex_shared_key(own_privkey_,session_key) == false )
          {
            THR(comm::exc::rs_sec_error,comm::exc::cm_udp_pkt,NULL);
          }
        }
        else
        {
          THR(comm::exc::rs_pubkey_empty,comm::exc::cm_udp_pkt,NULL);
        }

        if( debug() ) { printf("  ++ Session Key: '%s'\n",session_key.c_str()); }

        /* compile packet */
        crypt_pkt::saltbuf_t  salt;
        crypt_pkt::keybuf_t   key;
        crypt_pkt::headbuf_t  head;
        crypt_pkt::databuf_t  data;
        crypt_pkt::footbuf_t  foot;

        csl_sec_gen_rand( this->salt(), maxsalt() );
        salt.set( (unsigned char *)this->salt(), maxsalt() );
        key.set( (unsigned char *)session_key.c_str(), session_key.size()+1 );
        inner.t_copy_to(data);

        crypt_pkt pk;

        if( !pk.encrypt( salt,key,head,data,foot ) )
        {
          THR(comm::exc::rs_crypt_pkt_error,comm::exc::cm_udp_pkt,NULL);
        }

        /* output packet */
        unsigned char * outputp = data_+outer.size();
        unsigned int retlen = outer.size()+head.size()+data.size()+foot.size();

        if( retlen > maxlen() ) { THR(comm::exc::rs_too_big,comm::exc::cm_udp_pkt,NULL); }

        memcpy( outputp, head.data(), head.size() ); outputp += head.size();
        memcpy( outputp, data.data(), data.size() ); outputp += data.size();
        memcpy( outputp, foot.data(), foot.size() ); outputp += foot.size();

        if( debug() )
        {
          print_hex("  ++ UCAUTH PROL ",data_,outer.size());
          print_hex("  ++ UCAUTH HEAD ",head.data(),head.size() );
          print_hex("  ++ UCAUTH DATA ",data.data(),data.size() );
          print_hex("  ++ UCAUTH FOOT ",foot.data(),foot.size() );
        }

        /* return the data */
        len = retlen;
        return data_;
      }
      catch( common::exc e )
      {
        std::string s;
        e.to_string(s);
        fprintf(stderr,"Exception caught: %s\n",s.c_str());
        THR(comm::exc::rs_common_error,comm::exc::cm_udp_pkt,false);
      }
      return 0;
    }

    /* htua packet */
    bool udp::pkt::init_uc_htua(unsigned int len)
    {
      try
      {
        /* unencrypted part */
        pbuf    outer;
        outer.append(data_,len);
        xdrbuf  xbo(outer);

        int32_t packet_type = 0;
        xbo >> packet_type;

        if( debug() ) { printf(" -- [%ld] : packet_type : %d\n",xbo.position(),packet_type ); }

        if( packet_type != unicast_htua_p ) { THR(comm::exc::rs_invalid_packet_type,comm::exc::cm_udp_pkt,false); }

        unsigned char * ptrp = data_ + xbo.position();
        unsigned int    lenp = len - xbo.position();

        /* encrypted part */
        if( debug() ) { printf("  -- Session Key: '%s'\n",session_key_.c_str()); }

        /* de-compile packet */
        crypt_pkt::keybuf_t   key;
        crypt_pkt::headbuf_t  head;
        crypt_pkt::databuf_t  data;
        crypt_pkt::footbuf_t  foot;

        head.set(ptrp,crypt_pkt::header_len());
        foot.set(ptrp+(lenp-crypt_pkt::footer_len()),crypt_pkt::footer_len());
        data.set(ptrp+(crypt_pkt::header_len()),
                 lenp-crypt_pkt::footer_len()-crypt_pkt::header_len());

        key.set((unsigned char *)session_key_.c_str(),session_key_.size()+1);

        crypt_pkt pk;

        if( pk.decrypt( key,head,data,foot ) == false )
        {
          THR(comm::exc::rs_crypt_pkt_error,comm::exc::cm_udp_pkt,false);
        }

        /* xdr deserialize */
        pbuf inner;
        inner.append( data.data(), data.size() );
        xdrbuf xbi(inner);

        unsigned int sz=0;

        if( xbi.get_data( (unsigned char *)this->newsalt(),sz,maxsalt() ) == false )
        {
          THR(comm::exc::rs_xdr_error,comm::exc::cm_udp_pkt,false);
        }

        if( sz != maxsalt() )
        {
          THR(comm::exc::rs_xdr_error,comm::exc::cm_udp_pkt,false);
        }

        if( debug() ) { print_hex("  -- RAND ",this->newsalt(),maxsalt()); }

        memcpy( this->salt(), head.data(), crypt_pkt::header_len() );

        return true;
      }
      catch( common::exc e )
      {
        std::string s;
        e.to_string(s);
        fprintf(stderr,"Exception caught: %s\n",s.c_str());
        THR(comm::exc::rs_common_error,comm::exc::cm_udp_pkt,false);
      }
      return false;
    }

    unsigned char * udp::pkt::prepare_uc_htua(unsigned int & len)
    {
      try
      {
        /* unencrypted part */
        pbuf    outer;
        xdrbuf  xbo(outer);

        xbo << (int32_t)unicast_htua_p;

        if( debug() ) { printf(" ++ [%ld] : packet_type : %d\n",xbo.position(),unicast_htua_p ); }

        if( outer.size() > maxlen() ) { THR(comm::exc::rs_too_big,comm::exc::cm_udp_pkt,NULL); }

        outer.copy_to(data_,maxlen());

        /* encrypted part */
        pbuf inner;
        xdrbuf xbi(inner);

        xbi << xdrbuf::bindata_t( (const unsigned char *)this->newsalt(),(unsigned int)maxsalt() );

        if( debug() ) { printf("  ++ Session Key: '%s'\n",session_key_.c_str()); }

        /* compile packet */
        crypt_pkt::saltbuf_t  salt;
        crypt_pkt::keybuf_t   key;
        crypt_pkt::headbuf_t  head;
        crypt_pkt::databuf_t  data;
        crypt_pkt::footbuf_t  foot;

        salt.set( (unsigned char *)this->salt(), maxsalt() );
        key.set( (unsigned char *)session_key_.c_str(), session_key_.size()+1 );
        inner.t_copy_to(data);

        crypt_pkt pk;

        if( !pk.encrypt( salt,key,head,data,foot ) )
        {
          THR(comm::exc::rs_crypt_pkt_error,comm::exc::cm_udp_pkt,NULL);
        }

        /* output packet */
        unsigned char * outputp = data_+outer.size();
        unsigned int retlen = outer.size()+head.size()+data.size()+foot.size();

        if( retlen > maxlen() ) { THR(comm::exc::rs_too_big,comm::exc::cm_udp_pkt,NULL); }

        memcpy( outputp, head.data(), head.size() ); outputp += head.size();
        memcpy( outputp, data.data(), data.size() ); outputp += data.size();
        memcpy( outputp, foot.data(), foot.size() ); outputp += foot.size();

        if( debug() )
        {
          print_hex("  ++ UCHTUA PROL ",data_,outer.size());
          print_hex("  ++ UCHTUA HEAD ",head.data(),head.size() );
          print_hex("  ++ UCHTUA DATA ",data.data(),data.size() );
          print_hex("  ++ UCHTUA FOOT ",foot.data(),foot.size() );
        }

        /* return the data */
        len = retlen;
        return data_;
      }
      catch( common::exc e )
      {
        std::string s;
        e.to_string(s);
        fprintf(stderr,"Exception caught: %s\n",s.c_str());
        THR(comm::exc::rs_common_error,comm::exc::cm_udp_pkt,false);
      }
      return 0;
    }

    /* data packet */
    bool udp::pkt::init_data( unsigned int len, b1024_t & dta )
    {
      try
      {
        /* unencrypted part */
        pbuf    outer;
        outer.append(data_,len);
        xdrbuf  xbo(outer);

        int32_t packet_type = 0;
        xbo >> packet_type;

        if( debug() ) { printf(" -- [%ld] : packet_type : %d\n",xbo.position(),packet_type ); }

        if( packet_type != data_p ) { THR(comm::exc::rs_invalid_packet_type,comm::exc::cm_udp_pkt,false); }

        unsigned char * ptrp = data_ + xbo.position();
        unsigned int    lenp = len - xbo.position();

        /* encrypted part */
        if( debug() ) { printf("  -- Session Key: '%s'\n",session_key_.c_str()); }

        /* de-compile packet */
        crypt_pkt::keybuf_t   key;
        crypt_pkt::headbuf_t  head;
        crypt_pkt::databuf_t  data;
        crypt_pkt::footbuf_t  foot;

        head.set(ptrp,crypt_pkt::header_len());
        foot.set(ptrp+(lenp-crypt_pkt::footer_len()),crypt_pkt::footer_len());
        data.set(ptrp+(crypt_pkt::header_len()),
                 lenp-crypt_pkt::footer_len()-crypt_pkt::header_len());

        key.set((unsigned char *)session_key_.c_str(),session_key_.size()+1);

        crypt_pkt pk;

        if( pk.decrypt( key,head,data,foot ) == false )
        {
          THR(comm::exc::rs_crypt_pkt_error,comm::exc::cm_udp_pkt,false);
        }

        /* xdr deserialize */
        pbuf inner;
        inner.append( data.data(), data.size() );
        xdrbuf xbi(inner);

        unsigned int sz=0;

        if( xbi.get_data( (unsigned char *)newsalt(),sz,maxsalt() ) == false )
        {
          THR(comm::exc::rs_xdr_error,comm::exc::cm_udp_pkt,false);
        }

        if( sz != 8 )
        {
          THR(comm::exc::rs_xdr_error,comm::exc::cm_udp_pkt,false);
        }

        if( debug() ) { print_hex("  -- newsalt ",newsalt(),maxsalt()); }

        if( xbi.get_data( dta,sz,maxlen() ) == false )
        {
          THR(comm::exc::rs_xdr_error,comm::exc::cm_udp_pkt,false);
        }

        if( debug() ) { print_hex("  -- data ",dta.data(),dta.size()); }

        memcpy( this->salt(), head.data(), crypt_pkt::header_len() );

        return true;
      }
      catch( common::exc e )
      {
        std::string s;
        e.to_string(s);
        fprintf(stderr,"Exception caught: %s\n",s.c_str());
        THR(comm::exc::rs_common_error,comm::exc::cm_udp_pkt,false);
      }
      return false;
    }

    unsigned char * udp::pkt::prepare_data( const b1024_t & dta, unsigned int & len )
    {
      try
      {
        /* unencrypted part */
        pbuf    outer;
        xdrbuf  xbo(outer);

        xbo << (int32_t)data_p;

        if( debug() ) { printf(" ++ [%ld] : packet_type : %d\n",xbo.position(),data_p ); }

        if( outer.size() > maxlen() ) { THR(comm::exc::rs_too_big,comm::exc::cm_udp_pkt,NULL); }
        if( dta.size() == 0 )         { THR(comm::exc::rs_null_param,comm::exc::cm_udp_pkt,NULL); }

        outer.copy_to(data_,maxlen());

        /* encrypted part */
        pbuf inner;
        xdrbuf xbi(inner);

        xbi << xdrbuf::bindata_t( (const unsigned char *)newsalt(),maxsalt() );
        xbi << xdrbuf::bindata_t( dta.data(),dta.size() );

        if( debug() ) { printf("  ++ Session Key: '%s'\n",session_key_.c_str()); }

        /* compile packet */
        crypt_pkt::saltbuf_t  salt;
        crypt_pkt::keybuf_t   key;
        crypt_pkt::headbuf_t  head;
        crypt_pkt::databuf_t  data;
        crypt_pkt::footbuf_t  foot;

        salt.set( (unsigned char *)this->salt(), maxsalt() );
        key.set( (unsigned char *)session_key_.c_str(), session_key_.size()+1 );
        inner.t_copy_to(data);

        crypt_pkt pk;

        if( !pk.encrypt( salt,key,head,data,foot ) )
        {
          THR(comm::exc::rs_crypt_pkt_error,comm::exc::cm_udp_pkt,NULL);
        }

        /* output packet */
        unsigned char * outputp = data_+outer.size();
        unsigned int retlen = outer.size()+head.size()+data.size()+foot.size();

        if( retlen > maxlen() ) { THR(comm::exc::rs_too_big,comm::exc::cm_udp_pkt,NULL); }

        memcpy( outputp, head.data(), head.size() ); outputp += head.size();
        memcpy( outputp, data.data(), data.size() ); outputp += data.size();
        memcpy( outputp, foot.data(), foot.size() ); outputp += foot.size();

        if( debug() )
        {
          print_hex("  ++ DATA PROL ",data_,outer.size());
          print_hex("  ++ DATA HEAD ",head.data(),head.size() );
          print_hex("  ++ DATA DATA ",data.data(),data.size() );
          print_hex("  ++ DATA FOOT ",foot.data(),foot.size() );
        }

        /* return the data */
        len = retlen;
        return data_;
      }
      catch( common::exc e )
      {
        std::string s;
        e.to_string(s);
        fprintf(stderr,"Exception caught: %s\n",s.c_str());
        THR(comm::exc::rs_common_error,comm::exc::cm_udp_pkt,false);
      }
      return 0;
    }

    /* salt packet */
    bool udp::pkt::init_salt(unsigned int len)
    {
      return false; // TODO
    }

    unsigned char * udp::pkt::prepare_salt(unsigned int & len)
    {
      return 0; // TODO
    }

    void udp::hello_handler::operator()()
    {
      pkt &         p(pkt_);
      unsigned int  recvd = 0;
      udp::SAI      cliaddr;
      int           sock = socket_;
      bool          dbg = debug();

      p.debug(dbg);
      p.use_exc(use_exc());

      {
        scoped_mutex mm(msgs_->mtx_);
        if( msgs_->n_items() > 0 )
        {
          /* pop message from the circular buffer */
          msg & m(msgs_->pop());
          if( m.size_ == 0 )
          {
            /* empty message */
            return;
          }
          memcpy( p.data(),m.data_,m.size_ );
          memcpy( &cliaddr,&(m.sender_), sizeof(cliaddr) );
          recvd = m.size_;
        }
        else
        {
          /* no message to process */
          return;
        }
      }

      try
      {
        if( p.init_hello(recvd) == false ) { return; }

        /* check peer key */
        if( valid_key_cb_ && (*valid_key_cb_)(p.peer_pubkey()) == false ) { return; }

        /* hello callback */
        if( hello_cb_ && (*hello_cb_)( p.peer_pubkey(), cliaddr, p.server_info(), p.own_privkey()) == false )
        {
          return;
        }

        /* prepare response olleh packet */
        unsigned int olleh_len = 0;
        unsigned char * olleh = p.prepare_olleh(olleh_len);

        if( !olleh_len || !olleh ) { return; }

        /* send data back */
        socklen_t len = sizeof(cliaddr);

        if( sendto( sock, (const char *)olleh, olleh_len, 0,
            (struct sockaddr *)&cliaddr, len ) != (int)olleh_len )
        {
          return;
        }
      }
      catch( common::exc e )
      {
        fprintf(stderr,"Error [%s:%d]\n",__FILE__,__LINE__);
      }
      catch( comm::exc e )
      {
        fprintf(stderr,"Error [%s:%d]\n",__FILE__,__LINE__);
      }
    }

    void udp::auth_handler::operator()()
    {
      fprintf(stderr,"A");
    }

    void udp::data_handler::operator()()
    {
      fprintf(stderr,"D");
    }

    bool udp::srv::start()
    {
      /* private_key has been set ? */
      if( private_key_.is_empty() ) { THR(exc::rs_privkey_empty,exc::cm_udp_srv,false); }

      /* server_info_.public_key has been set ? */
      if( public_info_.public_key_.is_empty() ) { THR(exc::rs_pubkey_empty,exc::cm_udp_srv,false); }

      /* set debug values */
      bool dbg = debug();
      bool uex = use_exc();

      hello_receiver_.debug(dbg);  auth_receiver_.debug(dbg);  data_receiver_.debug(dbg);
      hello_handler_.debug(dbg);   auth_handler_.debug(dbg);   data_handler_.debug(dbg);

      /* set exception usage */
      hello_receiver_.use_exc(uex);  auth_receiver_.use_exc(uex);  data_receiver_.use_exc(uex);
      hello_handler_.use_exc(uex);   auth_handler_.use_exc(uex);   data_handler_.use_exc(uex);

      /* init receivers */
      if( hello_receiver_.start(1,4,1000,10,hello_handler_) == false ) { THR(exc::rs_thread_start,exc::cm_udp_srv,false); }
      if( auth_receiver_.start(1,4,1000,10,auth_handler_) == false ) { THR(exc::rs_thread_start,exc::cm_udp_srv,false); }
      if( data_receiver_.start(1,4,1000,10,data_handler_) == false ) { THR(exc::rs_thread_start,exc::cm_udp_srv,false); }

      /* set thread entries */
      hello_thread_.set_entry( hello_receiver_ );
      auth_thread_.set_entry( auth_receiver_ );
      data_thread_.set_entry( data_receiver_ );

      /* set keys */
      hello_handler_.init_pkt(private_key_,public_info_,hello_receiver_.socket());
      auth_handler_.init_pkt(private_key_,public_info_,auth_receiver_.socket());
      data_handler_.init_pkt(private_key_,public_info_,data_receiver_.socket());

      /* launch receiver threads */
      if( hello_thread_.start() == false ) { THR(exc::rs_thread_start,exc::cm_udp_srv,false); }
      if( auth_thread_.start() == false )  { THR(exc::rs_thread_start,exc::cm_udp_srv,false); }
      if( data_thread_.start() == false )  { THR(exc::rs_thread_start,exc::cm_udp_srv,false); }

      return true;
    }

    bool udp::srv::stop()
    {
      int ret = true;
      if( !hello_receiver_.stop() ) ret = false;
      if( !auth_receiver_.stop() ) ret = false;
      if( !data_receiver_.stop() ) ret = false;
      if( hello_thread_.exit_event().wait(1000) == false ) { ret = false; hello_thread_.stop(); }
      if( auth_thread_.exit_event().wait(1000) == false )  { ret = false; auth_thread_.stop(); }
      if( data_thread_.exit_event().wait(1000) == false )  { ret = false; data_thread_.stop(); }
      return ret;
    }

    udp::srv::srv() : create_session_cb_(0), cleanup_session_cb_(0), data_arrival_cb_(0), use_exc_(true), debug_(false)
    {
    }

    udp::srv::~srv()
    {
      stop();
    }

    void udp::srv::valid_key_cb(udp::valid_key & c)
    {
      //hello_entry_.valid_key_cb( c );  TODO
      //auth_entry_.valid_key_cb( c );  TODO
    }

    void udp::srv::hello_cb(udp::hello & c)
    {
      //hello_entry_.hello_cb( c );  TODO
    }

    void udp::srv::valid_creds_cb(udp::valid_creds & c)
    {
      //auth_entry_.valid_creds_cb( c );  TODO
    }

    void udp::srv::create_session_cb(udp::create_session & c)
    {
      //create_session_cb_ = &c;  TODO
    }

    void udp::srv::cleanup_session_cb(udp::cleanup_session & c)
    {
      //cleanup_session_cb_ = &c;  TODO
    }

    void udp::srv::data_arrival_cb(udp::data_arrival & c)
    {
      //data_arrival_cb_ = &c;  TODO
    }
  };
};

/* EOF */
