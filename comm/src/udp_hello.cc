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
#include "udp_hello.hh"
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
      **                         Hello Handler
      **
      **********************************************************************/

      /* hello packet */
      bool udp::hello_handler::init_hello( ecdh_key & peer_public_key, const msg & m )
      {
        try
        {
          pbuf pb;
          pb.append(m.data_,m.size_);
          xdrbuf xb(pb);

          int32_t packet_type;
          xb >> packet_type;

          if( debug() ) { printf(" -- [%ld] : packet_type : %d\n",xb.position(),packet_type ); }

          if( packet_type != msg::hello_p ) { THR(comm::exc::rs_invalid_packet_type,comm::exc::cm_udp_hello_handler,false); }

          if( peer_public_key.from_xdr(xb) == false ) { THR(comm::exc::rs_xdr_error,comm::exc::cm_udp_hello_handler,false); }

          if( debug() ) { printf(" -- [%ld] : ",xb.position()); peer_public_key.print(); }
        }
        catch( common::exc e )
        {
          std::string s;
          e.to_string(s);
          fprintf(stderr,"Exception caught: %s\n",s.c_str());
          THR(comm::exc::rs_common_error,comm::exc::cm_udp_hello_handler,false);
        }
        return true;
      }

      /* olleh packet */
      bool udp::hello_handler::prepare_olleh( const ecdh_key & my_public_key,
                                              bool need_login,
                                              bool need_pass,
                                              const bignum & my_private_key,
                                              const ecdh_key & peer_public_key,
                                              msg & m )
      {
        try
        {
          /* unencrypted part */
          pbuf    outer;
          xdrbuf  xbo(outer);

          xbo << (int32_t)msg::olleh_p;

          if( debug() ) { printf(" ++ [%ld] : packet_type : %d\n",xbo.position(),msg::olleh_p ); }

          /* ***
            hello callback may change the server info, and
            thus the public key based on the received public key
          */

          if( my_public_key.to_xdr(xbo) == false ) { THR(comm::exc::rs_xdr_error,comm::exc::cm_udp_hello_handler,false); }

          if( debug() ) { printf(" ++ [%ld] : ",xbo.position()); my_public_key.print(); }

          if( outer.size() > m.max_len() ) { THR(comm::exc::rs_too_big,comm::exc::cm_udp_hello_handler,false); }

          outer.copy_to(m.data_,m.max_len());

          /* encrypted part */
          pbuf inner;
          xdrbuf xbi(inner);

          xbi << (int32_t)need_login;
          xbi << (int32_t)need_pass;

          if( debug() ) { printf("  ++ login: %d pass: %d\n",need_login,need_pass); }

          /* generate session key */
          std::string session_key;

        /* ***
          hello callback may change the private key to be used
        */
          if( !peer_public_key.gen_sha1hex_shared_key(my_private_key,session_key) )
          {
            THR(comm::exc::rs_sec_error,comm::exc::cm_udp_hello_handler,false);
          }

          if( debug() ) { printf("  ++ Session Key: '%s'\n",session_key.c_str()); }

          /* compile packet */
          crypt_pkt::saltbuf_t  salt;
          crypt_pkt::keybuf_t   key;
          crypt_pkt::headbuf_t  head;
          crypt_pkt::databuf_t  data;
          crypt_pkt::footbuf_t  foot;

          csl_sec_gen_rand( salt.allocate(8), 8 );
          //salt.set( (unsigned char *)this->salt(), maxsalt() );
          key.set( (unsigned char *)session_key.c_str(), session_key.size()+1 );
          inner.t_copy_to( data );

          crypt_pkt pk;
          pk.use_exc(use_exc());

          if( !pk.encrypt( salt,key,head,data,foot ) )
          {
            THR(comm::exc::rs_crypt_pkt_error,comm::exc::cm_udp_hello_handler,false);
          }

          /* output packet */
          unsigned char * outputp = m.data_+outer.size();
          unsigned int retlen = outer.size()+head.size()+data.size()+foot.size();

          if( retlen > m.max_len() ) { THR(comm::exc::rs_too_big,comm::exc::cm_udp_hello_handler,false); }

          memcpy( outputp, head.data(), head.size() ); outputp += head.size();
          memcpy( outputp, data.data(), data.size() ); outputp += data.size();
          memcpy( outputp, foot.data(), foot.size() ); outputp += foot.size();

          if( debug() )
          {
            print_hex("  ++ OLLEH PROL ",m.data_,outer.size());
            print_hex("  ++ OLLEH HEAD ",head.data(),head.size() );
            print_hex("  ++ OLLEH DATA ",data.data(),data.size() );
            print_hex("  ++ OLLEH FOOT ",foot.data(),foot.size() );
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
          THR(comm::exc::rs_common_error,comm::exc::cm_udp_hello_handler,false);
        }

        return false;
      }

      /**/
      void udp::hello_handler::operator()()
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

          bool need_login  = need_login_;
          bool need_pass   = need_pass_;

          if( init_hello(peer_public_key, ms) == false ) { return; }

          /* check peer key */
          if( valid_key_cb_ && (*valid_key_cb_)(peer_public_key) == false ) { return; }

          /* hello callback */
          if( hello_cb_ && 
              (*hello_cb_)( peer_public_key, ms.sender_, my_public_key,
                            need_login, need_pass, my_private_key) == false )
          {
            return;
          }

          /* prepare response olleh packet */
          if( prepare_olleh( my_public_key, need_login, need_pass, my_private_key, peer_public_key, ms) == false )
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
      **                         Hello Server
      **
      **********************************************************************/

      bool hello_srv::start()
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
        if( receiver_.start(1,4,1000,10,handler_) == false ) { THR(exc::rs_thread_start,exc::cm_udp_hello_srv,false); }

        /* set thread entries */
        thread_.set_entry( receiver_ );
        handler_.socket( receiver_.socket() );

        /* launch receiver threads */
        if( thread_.start() == false ) { THR(exc::rs_thread_start,exc::cm_udp_hello_srv,false); }

        return true;
      }

      bool hello_srv::stop()
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

      hello_srv::hello_srv() : use_exc_(true), debug_(false)
      {
      }

      hello_srv::~hello_srv()
      {
        stop();
      }

      /**********************************************************************
      **
      **                         Hello Client
      **
      **********************************************************************/

      hello_cli::hello_cli() : use_exc_(true), debug_(false), sock_(-1)
      {
        memset( &addr_,0,sizeof(addr_) );
        addr_.sin_family        = AF_INET;
        addr_.sin_addr.s_addr   = htonl(INADDR_LOOPBACK);
      }

      hello_cli::~hello_cli()
      {
        if( sock_ > 0 ) ShutdownCloseSocket( sock_ );
        sock_ = -1;
      }

      bool hello_cli::prepare_hello( msg & m )
      {
        try
        {
          pbuf pb;
          xdrbuf xb(pb);

          xb << (int32_t)msg::hello_p;

          if( debug() ) { printf(" ++ [%ld] : packet_type : %d\n",xb.position(),msg::hello_p ); }

          if( public_key_.to_xdr(xb) == false ) { THR(comm::exc::rs_xdr_error,comm::exc::cm_udp_hello_cli,false); }

          if( debug() ) { printf(" ++ [%ld] : ",xb.position()); public_key_.print(); }

          pb.copy_to( m.data_,m.max_len() );
          m.size_ = pb.size();
        }
        catch( common::exc e )
        {
          std::string s;
          e.to_string(s);
          fprintf(stderr,"Exception caught: %s\n",s.c_str());
          THR(comm::exc::rs_common_error,comm::exc::cm_udp_hello_cli,false);
        }
        return true;
      }

      bool hello_cli::init_olleh( const msg & m )
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

          if( packet_type != msg::olleh_p ) { THR(comm::exc::rs_invalid_packet_type,comm::exc::cm_udp_hello_cli,false); }

          if( server_public_key_.from_xdr(xbo) == false ) { THR(comm::exc::rs_xdr_error,comm::exc::cm_udp_hello_cli,false); }

          if( debug() ) { printf(" -- [%ld] : ",xbo.position()); server_public_key_.print(); }

          const unsigned char * ptrp = m.data_ + xbo.position();
          unsigned int          lenp = m.size_ - xbo.position();

          /* encrypted part */

          /* generate session key */
          std::string session_key;

          if( !server_public_key_.gen_sha1hex_shared_key(private_key_,session_key) )
          {
            THR(comm::exc::rs_sec_error,comm::exc::cm_udp_hello_cli,false);
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
          pk.use_exc(use_exc());

          if( pk.decrypt( key,head,data,foot ) == false )
          {
            THR(comm::exc::rs_crypt_pkt_error,comm::exc::cm_udp_hello_cli,false);
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

          need_login_ = (need_login == 1);
          need_pass_ = (need_pass == 1);

          return true;
        }
        catch( common::exc e )
        {
          std::string s;
          e.to_string(s);
          fprintf(stderr,"Exception caught: %s\n",s.c_str());
          THR(comm::exc::rs_common_error,comm::exc::cm_udp_hello_cli,false);
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

      bool hello_cli::init()
      {
        if( sock_ > 0 ) return true;

        /* public_key has been set ? */
        if( public_key().is_empty() ) { THR(exc::rs_pubkey_empty,exc::cm_udp_hello_cli,false); }

        sock_ = init_sock( addr_ );

        if( sock_ <= 0 ) { THRC(exc::rs_socket_failed,exc::cm_udp_hello_cli,false); }

        return true;
      }

      bool hello_cli::hello( unsigned int timeout_ms )
      {
        int err = 0;

        msg ms;

        if( !init() ) return false;

        /* prepare hello packet */
        if( prepare_hello( ms) == false )
        {
          THR(exc::rs_pkt_error,exc::cm_udp_hello_cli,false);
        }

        if( (err=::send( sock_, (const char *)ms.data_, ms.size_, 0 )) != (int)(ms.size_) )
        {
          THRC(exc::rs_send_failed,exc::cm_udp_hello_cli,false);
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
          err = ::recv(sock_,(char *)ms.data_, ms.max_len(), 0);
          //
          if( err > 0 )
          {
            ms.size_ = err;

            if( init_olleh( ms ) == false )
            {
              THR(exc::rs_pkt_error,exc::cm_udp_hello_cli,false);
            }
            return true;
          }
          else
          {
            THRC(exc::rs_recv_failed,exc::cm_udp_hello_cli,false);
          }
        }
        else if( err == 0 )
        {
          /* timed out */
          THR(exc::rs_timeout,exc::cm_udp_hello_cli,false);
        }
        else
        {
          /* error */
          THRC(exc::rs_select_failed,exc::cm_udp_hello_cli,false);
        }
      }

    } /* end of udp namespace */
  } /* end of comm namespace */
} /* end of csl namespace */

/* EOF */
