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
#include "udp_auth.hh"
#include "mutex.hh"
#include "crypt_pkt.hh"
#include "csl_sec.h"
#include "pbuf.hh"
#include "xdrbuf.hh"
#include "common.h"
#include <string>

namespace csl
{
  using namespace nthread;
  using namespace sec;
  using namespace common;

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
    namespace udp
    {
      /**********************************************************************
      **
      **                         Auth Handler
      **
      **********************************************************************/

      /* auth packet */
      bool udp::auth_handler::init_auth( ecdh_key & peer_public_key,
                                         string & login,
                                         string & pass,
                                         string & sesskey,
                                         saltbuf_t & slt,
                                         const msg & m )
      {
        try
        {
          /* unencrypted part */
          pbuf    outer;
          outer.append(m.data_,m.size_);
          xdrbuf  xbo(outer);

          int32_t packet_type = 0;
          xbo >> packet_type;

          if( debug() ) { printf(" -- [%ld] : packet_type : %d\n",xbo.position(),packet_type ); }

          if( packet_type != msg::unicast_auth_p )
          {
            THR(comm::exc::rs_invalid_packet_type,comm::exc::cm_udp_auth_handler,false);
          }

          if( peer_public_key.from_xdr(xbo) == false ) { THR(comm::exc::rs_xdr_error,comm::exc::cm_udp_auth_handler,false); }

          if( debug() ) { printf(" -- [%ld] : ",xbo.position()); peer_public_key.print(); }

          const unsigned char * ptrp = m.data_ + xbo.position();
          unsigned int          lenp = m.size_ - xbo.position();

          /* encrypted part */

          /* generate session key */
          std::string session_key;

          if( !peer_public_key.gen_sha1hex_shared_key(private_key(),session_key) )
          {
            THR(comm::exc::rs_sec_error,comm::exc::cm_udp_auth_handler,false);
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
            THR(comm::exc::rs_crypt_pkt_error,comm::exc::cm_udp_auth_handler,false);
          }

          /* xdr deserialize */
          pbuf inner;
          inner.append( data.data(), data.size() );
          xdrbuf xbi(inner);

          unsigned int sz=0;

          if( xbi.get_data( slt,sz,salt_size_v ) == false )
          {
            THR(comm::exc::rs_xdr_error,comm::exc::cm_udp_auth_handler,false);
          }

          if( sz != salt_size_v )
          {
            THR(comm::exc::rs_xdr_error,comm::exc::cm_udp_auth_handler,false);
          }

          if( debug() ) { print_hex("  -- RAND ",slt.data(),slt.size()); }

          xbi >> login;

          if( debug() ) { printf("  -- [%ld] login: '%s'\n",xbi.position(),login.c_str()); }

          xbi >> pass;

          if( debug() ) { printf("  -- [%ld] pass: '%s'\n",xbi.position(),pass.c_str()); }

          xbi >> sesskey;

          if( debug() ) { printf("  -- [%ld] sesskey: '%s'\n",xbi.position(),sesskey.c_str()); }

          return true;
        }
        catch( common::exc e )
        {
          std::string s;
          e.to_string(s);
          fprintf(stderr,"Exception caught: %s\n",s.c_str());
          THR(comm::exc::rs_common_error,comm::exc::cm_udp_auth_handler,false);
        }
      }

      /* htua packet */
      bool udp::auth_handler::prepare_htua( const saltbuf_t & pkt_salt,
                                            const saltbuf_t & comm_salt,
                                            const string & sesskey,
                                            msg & m )
      {
        try
        {
          /* unencrypted part */
          pbuf    outer;
          xdrbuf  xbo(outer);

          xbo << (int32_t)msg::unicast_htua_p;

          if( debug() ) { printf(" ++ [%ld] : packet_type : %d\n",xbo.position(),msg::unicast_htua_p ); }

          if( outer.size() > m.max_len() ) { THR(comm::exc::rs_too_big,comm::exc::cm_udp_auth_handler,false); }

          outer.copy_to(m.data_,m.max_len());

          /* encrypted part */
          pbuf inner;
          xdrbuf xbi(inner);

          xbi << xdrbuf::bindata_t( comm_salt.data(),comm_salt.size() );

          if( debug() ) { printf("  ++ Session Key: '%s'\n",sesskey.c_str()); }

          /* compile packet */
          crypt_pkt::saltbuf_t  salt(pkt_salt);
          crypt_pkt::keybuf_t   key;
          crypt_pkt::headbuf_t  head;
          crypt_pkt::databuf_t  data;
          crypt_pkt::footbuf_t  foot;

          key.set( (unsigned char *)sesskey.c_str(), sesskey.size()+1 );
          inner.t_copy_to(data);

          crypt_pkt pk;

          if( !pk.encrypt( salt,key,head,data,foot ) )
          {
            THR(comm::exc::rs_crypt_pkt_error,comm::exc::cm_udp_auth_handler,false);
          }

          /* output packet */
          unsigned char * outputp = m.data_+outer.size();
          unsigned int    retlen  = outer.size()+head.size()+data.size()+foot.size();

          if( retlen > m.max_len() ) { THR(comm::exc::rs_too_big,comm::exc::cm_udp_auth_handler,false); }

          memcpy( outputp, head.data(), head.size() ); outputp += head.size();
          memcpy( outputp, data.data(), data.size() ); outputp += data.size();
          memcpy( outputp, foot.data(), foot.size() ); outputp += foot.size();

          if( debug() )
          {
            print_hex("  ++ UCHTUA PROL ",m.data_,outer.size());
            print_hex("  ++ UCHTUA HEAD ",head.data(),head.size() );
            print_hex("  ++ UCHTUA DATA ",data.data(),data.size() );
            print_hex("  ++ UCHTUA FOOT ",foot.data(),foot.size() );
          }

          /* return the data */
          m.size_ = retlen;
          return true;
        }
        catch( common::exc e )
        {
          std::string s;
          e.to_string(s);
          fprintf(stderr,"Exception caught: %s\n",s.c_str());
          THR(comm::exc::rs_common_error,comm::exc::cm_udp_auth_handler,false);
        }
        return false;
      }

      /**/
      void udp::auth_handler::operator()()
      {
        msg ms;

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
            m.copy_to(ms);
          }
          else
          {
            /* no message to process */
            return;
          }
        }

        try
        {
          ecdh_key   peer_public_key;
          ecdh_key   my_public_key(public_key_);
          bignum     my_private_key(private_key_);

          string     login;
          string     pass;
          string     sesskey;
          saltbuf_t  peer_salt;

          if( init_auth(peer_public_key, login, pass, sesskey, peer_salt, ms) == false ) { return; }

          /* check peer key */
          if( valid_key_cb_ && (*valid_key_cb_)(peer_public_key) == false ) { return; }

          /* validate credentials */
          if( valid_creds_cb_ &&
              (*valid_creds_cb_)( peer_public_key, ms.sender_, login, pass) == false )
          {
            return;
          }

          /* generate my and pkt salt */
          saltbuf_t  pkt_salt;
          saltbuf_t  my_salt;

          csl_sec_gen_rand( pkt_salt.allocate(8), 8 );
          csl_sec_gen_rand( my_salt.allocate(8), 8 );

          /* register authenticated client */
          if( register_auth_cb_ &&
              (*register_auth_cb_)( ms.sender_, login, pass, sesskey, peer_salt, my_salt ) == false )
          {
            return;
          }

          /* prepare response htua packet */
          if( prepare_htua( pkt_salt, my_salt, sesskey, ms) == false )
          {
            fprintf(stderr,"[%s:%d] Error in prepare_olleh()\n",__FILE__,__LINE__);
            return;
          }

          /* send data back */
          socklen_t len = sizeof(ms.sender_);

          if( sendto( socket_, (const char *)ms.data_, ms.size_, 0,
              (struct sockaddr *)&(ms.sender_), len ) != (int)(ms.size_) )
          {
            fprintf(stderr,"[%s:%d] Error in sendto(%d)\n",__FILE__,__LINE__,socket_);
            perror("sendto");
            return;
          }
        }
        catch( common::exc e )
        {
          std::string s;
          e.to_string(s);
          fprintf(stderr,"Error [%s:%d]: %s\n",__FILE__,__LINE__,s.c_str());
        }
        catch( comm::exc e )
        {
          std::string s;
          e.to_string(s);
          fprintf(stderr,"Error [%s:%d]: %s\n",__FILE__,__LINE__,s.c_str());
        }
      }

      /**********************************************************************
      **
      **                         Auth Server
      **
      **********************************************************************/

      bool auth_srv::start()
      {
        /* set debug values */
        bool dbg = debug();
        bool uex = use_exc();

        receiver_.debug(dbg);
        handler_.debug(dbg);

        /* set exception usage */
        receiver_.use_exc(uex);
        handler_.use_exc(uex);

        /* init receivers */
        if( receiver_.start(1,4,1000,10,handler_) == false ) { THR(exc::rs_thread_start,exc::cm_udp_auth_srv,false); }

        /* set thread entries */
        thread_.set_entry( receiver_ );
        handler_.socket( receiver_.socket() );

        /* launch receiver threads */
        if( thread_.start() == false ) { THR(exc::rs_thread_start,exc::cm_udp_auth_srv,false); }

        return true;
      }

      bool auth_srv::stop()
      {
        int ret = true;

        if( !receiver_.stop() )
        {
          ret = false;
        }

        if( !thread_.exit_event().wait(1000) )
        {
          ret = false;
          thread_.stop();
        }

        return ret;
      }

      auth_srv::auth_srv() : use_exc_(true), debug_(false)
      {
      }

      auth_srv::~auth_srv()
      {
        stop();
      }

      /**********************************************************************
      **
      **                         Auth Client
      **
      **********************************************************************/

      auth_cli::auth_cli() : use_exc_(true), debug_(false), sock_(-1)
      {
        memset( &addr_,0,sizeof(addr_) );
        addr_.sin_family        = AF_INET;
        addr_.sin_addr.s_addr   = htonl(INADDR_LOOPBACK);
      }

      auth_cli::~auth_cli()
      {
        if( sock_ > 0 ) ShutdownCloseSocket( sock_ );
        sock_ = -1;
      }

      bool auth_cli::prepare_auth( msg & m )
      {
        try
        {
          pbuf pb;
          xdrbuf xb(pb);

          xb << (int32_t)msg::unicast_auth_p;

          if( debug() ) { printf(" ++ [%ld] : packet_type : %d\n",xb.position(),msg::unicast_auth_p ); }

          if( public_key_.to_xdr(xb) == false ) { THR(comm::exc::rs_xdr_error,comm::exc::cm_udp_auth_cli,false); }

          if( debug() ) { printf(" ++ [%ld] : ",xb.position()); public_key_.print(); }

          pb.copy_to( m.data_,m.max_len() );
          m.size_ = pb.size();
        }
        catch( common::exc e )
        {
          std::string s;
          e.to_string(s);
          fprintf(stderr,"Exception caught: %s\n",s.c_str());
          THR(comm::exc::rs_common_error,comm::exc::cm_udp_auth_cli,false);
        }
        return true;
      }

      bool auth_cli::init_htua( const msg & m )
      {
        try
        {
          /* unencrypted part */
          pbuf    outer;
          outer.append(m.data_,m.size_);
          xdrbuf  xbo(outer);

          int32_t packet_type = 0;
          xbo >> packet_type;

          if( debug() ) { printf(" -- [%ld] : packet_type : %d\n",xbo.position(),packet_type ); }

          if( packet_type != msg::unicast_htua_p )
          {
            THR(comm::exc::rs_invalid_packet_type,comm::exc::cm_udp_auth_cli,false);
          }

          const unsigned char * ptrp = m.data_ + xbo.position();
          unsigned int          lenp = m.size_ - xbo.position();

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
            THR(comm::exc::rs_crypt_pkt_error,comm::exc::cm_udp_auth_cli,false);
          }

          /* xdr deserialize */
          pbuf inner;
          inner.append( data.data(), data.size() );
          xdrbuf xbi(inner);

          unsigned int sz=0;

          if( xbi.get_data( server_salt_,sz,saltbuf_t::preallocated_size ) == false )
          {
            THR(comm::exc::rs_xdr_error,comm::exc::cm_udp_auth_cli,false);
          }

          if( sz != saltbuf_t::preallocated_size )
          {
            THR(comm::exc::rs_xdr_error,comm::exc::cm_udp_auth_cli,false);
          }

          if( debug() ) { print_hex("  -- RAND ",server_salt_.data(),server_salt_.size()); }

          //memcpy( this->salt(), head.data(), crypt_pkt::header_len() );

          return true;
        }
        catch( common::exc e )
        {
          std::string s;
          e.to_string(s);
          fprintf(stderr,"Exception caught: %s\n",s.c_str());
          THR(comm::exc::rs_common_error,comm::exc::cm_udp_auth_cli,false);
        }
        return false;
      }


      namespace
      {
        int init_sock(SAI & addr)
        {
          int sock = ::socket( AF_INET, SOCK_DGRAM, 0 );
          if( sock <= 0 ) { return sock; }

          socklen_t len = sizeof(addr);

          if( ::connect(sock, (struct sockaddr *)&addr, len) == -1 )
          {
            ShutdownCloseSocket(sock);
            return -1;
          }
          return sock;
        }
      }

      bool auth_cli::init()
      {
        if( sock_ > 0 ) return true;

        /* public_key has been set ? */
        if( public_key().is_empty() ) { THR(exc::rs_pubkey_empty,exc::cm_udp_auth_cli,false); }

        sock_ = init_sock( addr_ );

        if( sock_ <= 0 ) { THRC(exc::rs_socket_failed,exc::cm_udp_auth_cli,false); }

        return true;
      }

      namespace
      {
        static void gen_sess_key(std::string & k)
        {
          char s[64];
          csl_sec_gen_rand(s,sizeof(s));
          char   res[SHA1_HEX_DIGEST_STR_LENGTH];
          size_t ol = 0;
          csl_sec_sha1_conv( &s, sizeof(s), res, &ol );
          k = res;
        }
      }

      bool auth_cli::auth( unsigned int timeout_ms )
      {
        int err = 0;

        if( !init() ) return false;

        if( public_key().is_empty() ) { THR(exc::rs_hello_nocall,exc::cm_udp_auth_cli,false); }

        saltbuf_t client_salt;

        gen_sess_key(session_key_);
        csl_sec_gen_rand( client_salt.allocate(8), client_salt.size() );

        /* prepare auth packet */
        msg m;

        if( !prepare_auth( m ) ) { THR(exc::rs_pkt_error,exc::cm_udp_auth_cli,false); }

        if( (err=::send( sock_, (const char *)m.data_, m.size_ , 0 )) != (int)(m.size_) )
        {
          THRC(exc::rs_send_failed,exc::cm_udp_auth_cli,false);
        }

        fd_set rfds;
        struct timeval tv = { 0,0 };
        struct timeval * ptv = 0;

        if( timeout_ms )
        {
          ptv = &tv;
          tv.tv_sec  = timeout_ms/1000;
          tv.tv_usec = (timeout_ms%1000)*1000;
        }

        FD_ZERO(&rfds);
        FD_SET(sock_,&rfds);

        err = ::select(sock_+1,&rfds,NULL,NULL,ptv);

        if( err > 0 )
        {
          err = ::recv(sock_,(char *)m.data_, m.max_len(), 0);
          //
          if( err > 0 )
          {
            if( init_htua( m ) == false )
            {
              THR(exc::rs_pkt_error,exc::cm_udp_auth_cli,false);
            }

            /* TODO
            chann_.use_exc(this->use_exc());

            if( chann_.init(pkt_, data_sock_, data_addr_, client_salt, server_salt) == false )
            {
              THR(exc::rs_channel_init,exc::cm_udp_auth_cli,false);
            }
            */
            return true;
          }
          else
          {
            THRC(exc::rs_recv_failed,exc::cm_udp_auth_cli,false);
          }
        }
        else if( err == 0 )
        {
          /* timed out */
          THR(exc::rs_timeout,exc::cm_udp_auth_cli,false);
        }
        else
        {
          /* error */
          THRC(exc::rs_select_failed,exc::cm_udp_auth_cli,false);
        }
      }
    } /* end of udp namespace */
  } /* end of comm namespace */
} /* end of csl namespace */

/* EOF */
