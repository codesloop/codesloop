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
#include "csl_common.hh"
#include "csl_sec.h"
#include "crypt_pkt.hh"
#include "udp_pkt.hh"

namespace csl
{
  using common::pbuf;
  using common::xdrbuf;
  using sec::crypt_pkt;

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
    udp_pkt::udp_pkt() : use_exc_(true), debug_(false) {}

    /* internal buffer */

    /* public key */
    void udp_pkt::peer_pubkey(const ecdh_key & pk)     { peer_pubkey_ = pk; }
    ecdh_key & udp_pkt::peer_pubkey()                  { return peer_pubkey_; }

    void udp_pkt::own_pubkey(const ecdh_key & pk)      { own_pubkey_ = pk; }
    ecdh_key & udp_pkt::own_pubkey()                   { return own_pubkey_; }
    const ecdh_key & udp_pkt::own_pubkey_const() const { return own_pubkey_; }

    /* info */
    void udp_pkt::server_info(const udp_srv_info & info)    { info_ = info;      }
    udp_srv_info & udp_pkt::server_info()                   { return info_;      }
    const udp_srv_info & udp_pkt::server_info_const() const { return info_;      }

    /* private key */
    void udp_pkt::own_privkey(const bignum & pk)      { own_privkey_ = pk; }
    bignum & udp_pkt::own_privkey()                   { return own_privkey_; }
    const bignum & udp_pkt::own_privkey_const() const { return own_privkey_; }

    /* login */
    const std::string & udp_pkt::login() const  { return login_; }
    void udp_pkt::login(const std::string & l)  { login_ = l; }

    /* pass */
    const std::string & udp_pkt::pass() const  { return pass_; }
    void udp_pkt::pass(const std::string & p)  { pass_ = p; }

    /* newsalt */
    unsigned long long * udp_pkt::newsalt() { return (unsigned long long *)&newsalt_; }

    /* salt */
    unsigned long long * udp_pkt::salt() { return (unsigned long long *)&salt_; }

    /* session key */
    const std::string & udp_pkt::session_key() const { return session_key_; }
    void udp_pkt::session_key(const std::string & k) { session_key_ = k; }

    /* hello packet */
    bool udp_pkt::init_hello(unsigned int len)
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

    unsigned char * udp_pkt::prepare_hello(unsigned int & len)
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
    bool udp_pkt::init_olleh(unsigned int len)
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

        info_.public_key(peer_pubkey_);
        info_.need_login(need_login == 1);
        info_.need_pass(need_pass == 1);

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

    unsigned char * udp_pkt::prepare_olleh(unsigned int & len)
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
        if( info_.public_key().to_xdr(xbo) == false ) { THR(comm::exc::rs_xdr_error,comm::exc::cm_udp_pkt,false); }

        if( debug() ) { printf(" ++ [%ld] : ",xbo.position()); info_.public_key().print(); }

        if( outer.size() > maxlen() ) { THR(comm::exc::rs_too_big,comm::exc::cm_udp_pkt,NULL); }

        outer.copy_to(data_,maxlen());

        /* encrypted part */
        pbuf inner;
        xdrbuf xbi(inner);

        xbi << (int32_t)info_.need_login();
        xbi << (int32_t)info_.need_pass();

        if( debug() ) { printf("  ++ login: %d pass: %d\n",info_.need_login(),info_.need_pass()); }

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
    bool udp_pkt::init_uc_auth(unsigned int len)
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

    unsigned char * udp_pkt::prepare_uc_auth(unsigned int & len)
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
        else if( info_.public_key().is_empty() == false )
        {
          if( info_.public_key().gen_sha1hex_shared_key(own_privkey_,session_key) == false )
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
    bool udp_pkt::init_uc_htua(unsigned int len)
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

    unsigned char * udp_pkt::prepare_uc_htua(unsigned int & len)
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
    bool udp_pkt::init_data( unsigned int len,
                             b1024_t & dta )
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

    unsigned char * udp_pkt::prepare_data( const b1024_t & dta, // prepare before send
                                           unsigned int & len )
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
    bool udp_pkt::init_salt(unsigned int len)
    {
      return false; // TODO
    }

    unsigned char * udp_pkt::prepare_salt(unsigned int & len)
    {
      return 0; // TODO
    }
  };
};

/* EOF */
