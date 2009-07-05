/*
Copyright (c) 2008,2009, David Beck, Tamas Foldi

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
#include "str.hh"

namespace csl
{
  using namespace nthread;
  using namespace sec;
  using namespace common;

  namespace
  {
    static void print_hex(const wchar_t * prefix,const void * vp,size_t len)
    {
      const unsigned char * hx = reinterpret_cast<const unsigned char *>(vp);
      PRINTF(L"%ls [%04d] : ",prefix,len);
      for(size_t i=0;i<len;++i) PRINTF(L"%.2X",hx[i]);
      PRINTF(L"\n");
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
                                         ustr & login,
                                         ustr & pass,
                                         ustr & sesskey,
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

          if( packet_type != msg::unicast_auth_p )
          {
            THR(comm::exc::rs_invalid_packet_type,false);
          }

          if( peer_public_key.from_xdr(xbo) == false ) { THR(comm::exc::rs_xdr_error,false); }

          const unsigned char * ptrp = m.data_ + xbo.position();
          unsigned int          lenp = m.size_ - xbo.position();

          /* encrypted part */

          /* generate session key */
          ustr session_key;

          if( !peer_public_key.gen_sha1hex_shared_key(private_key(),session_key) )
          {
            THR(comm::exc::rs_sec_error,false);
          }

          if( debug() )
          {
            PRINTF(L" -- [%ld] : packet_type : %d\n",xbo.position(),packet_type );
            PRINTF(L" -- [%ld] : ",xbo.position()); peer_public_key.print();
            PRINTF(L"  -- Session Key: '%s'\n",session_key.c_str());
          }

          /* de-compile packet */
          crypt_pkt::keybuf_t   key;
          crypt_pkt::headbuf_t  head;
          crypt_pkt::databuf_t  data;
          crypt_pkt::footbuf_t  foot;

          head.set(ptrp,crypt_pkt::header_len());
          foot.set(ptrp+(lenp-crypt_pkt::footer_len()),crypt_pkt::footer_len());
          data.set(ptrp+(crypt_pkt::header_len()),
                   lenp-crypt_pkt::footer_len()-crypt_pkt::header_len());

          key.set( reinterpret_cast<const unsigned char *>(session_key.c_str()),
            (session_key.size()+1) );

          crypt_pkt pk;
          pk.use_exc(use_exc());

          if( pk.decrypt( key,head,data,foot ) == false )
          {
            THR(comm::exc::rs_crypt_pkt_error,false);
          }

          /* xdr deserialize */
          pbuf inner;
          inner.append( data.data(), data.size() );
          xdrbuf xbi(inner);

          unsigned int sz=0;

          if( xbi.get_data( slt,sz,salt_size_v ) == false )
          {
            THR(comm::exc::rs_xdr_error,false);
          }

          if( sz != salt_size_v )
          {
            THR(comm::exc::rs_xdr_error,false);
          }

          xbi >> login;
          xbi >> pass;
          xbi >> sesskey;

          if( debug() )
          {
            print_hex(L"  -- RAND ",slt.data(),slt.size());
            PRINTF(   L"  -- [%ld] login:   '%ls'\n",xbi.position(),login.c_str());
            PRINTF(   L"  -- [%ld] pass:    '%ls'\n",xbi.position(),pass.c_str());
            PRINTF(   L"  -- [%ld] sesskey: '%ls'\n",xbi.position(),sesskey.c_str());
          }

          return true;
        }
        catch( common::exc e )
        {
          str s;
          e.to_string(s);
          FPRINTF(stderr,L"Exception caught: %ls\n",s.c_str());
          THR(comm::exc::rs_common_error,false);
        }
      }

      /* htua packet */
      bool udp::auth_handler::prepare_htua( const saltbuf_t & pkt_salt,
                                            const saltbuf_t & comm_salt,
                                            const ustr & sesskey,
                                            msg & m )
      {
        try
        {
          if( pkt_salt.size() != salt_size_v )  { THR(comm::exc::rs_salt_size,false); }
          if( comm_salt.size() != salt_size_v ) { THR(comm::exc::rs_salt_size,false); }
          if( sesskey.size() == 0 )             { THR(comm::exc::rs_sesskey_empty,false); }

          /* unencrypted part */
          pbuf    outer;
          xdrbuf  xbo(outer);

          xbo << static_cast<int32_t>(msg::unicast_htua_p);

          if( outer.size() > m.max_len() ) { THR(comm::exc::rs_too_big,false); }

          outer.copy_to(m.data_,m.max_len());

          /* encrypted part */
          pbuf inner;
          xdrbuf xbi(inner);

          xbi << xdrbuf::bindata_t( comm_salt.data(),comm_salt.size() );

          if( debug() )
          {
            PRINTF(L" ++ [%ld] : packet_type : %d\n",xbo.position(),msg::unicast_htua_p );
            PRINTF(L"  ++ Session Key: '%s'\n",sesskey.c_str());
          }

          /* compile packet */
          crypt_pkt::saltbuf_t  salt(pkt_salt);
          crypt_pkt::keybuf_t   key;
          crypt_pkt::headbuf_t  head;
          crypt_pkt::databuf_t  data;
          crypt_pkt::footbuf_t  foot;

          key.set( reinterpret_cast<const unsigned char *>(sesskey.c_str()),
            (sesskey.size()+1) );

          inner.t_copy_to(data);

          crypt_pkt pk;
          pk.use_exc(use_exc());

          if( !pk.encrypt( salt,key,head,data,foot ) )
          {
            THR(comm::exc::rs_crypt_pkt_error,false);
          }

          /* output packet */
          unsigned char * outputp = m.data_+outer.size();
          unsigned int    retlen  = outer.size()+head.size()+data.size()+foot.size();

          if( retlen > m.max_len() ) { THR(comm::exc::rs_too_big,false); }

          memcpy( outputp, head.data(), head.size() ); outputp += head.size();
          memcpy( outputp, data.data(), data.size() ); outputp += data.size();
          memcpy( outputp, foot.data(), foot.size() ); outputp += foot.size();

          if( debug() )
          {
            print_hex(L"  ++ UCHTUA PROL ",m.data_,outer.size());
            print_hex(L"  ++ UCHTUA HEAD ",head.data(),head.size() );
            print_hex(L"  ++ UCHTUA DATA ",data.data(),data.size() );
            print_hex(L"  ++ UCHTUA FOOT ",foot.data(),foot.size() );
            PRINTF(  L"  ++ UCHTUA size: %d\n",retlen );
          }

          /* return the data */
          m.size_ = retlen;
          return true;
        }
        catch( common::exc e )
        {
          str s;
          e.to_string(s);
          FPRINTF(stderr,L"Exception caught: %ls\n",s.c_str());
          THR(comm::exc::rs_common_error,false);
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

          ustr        login;
          ustr        pass;
          ustr        sesskey;
          saltbuf_t   peer_salt;

          if( init_auth(peer_public_key, login, pass, sesskey, peer_salt, ms) == false ) { return; }

          /* check peer key */
          if( valid_key_cb_ && (*valid_key_cb_)(peer_public_key) == false )
          {
            return;
          }

          /* validate credentials */
          if( valid_creds_cb_ &&
              (*valid_creds_cb_)( peer_public_key, ms.sender_, login, pass) == false )
          {
            return;
          }

          /* generate my and pkt salt */
          saltbuf_t  pkt_salt;
          saltbuf_t  my_salt;

          csl_sec_gen_rand( pkt_salt.allocate(saltbuf_t::preallocated_size), saltbuf_t::preallocated_size );
          csl_sec_gen_rand( my_salt.allocate(saltbuf_t::preallocated_size), saltbuf_t::preallocated_size );

          /* register authenticated client */
          if( register_auth_cb_ &&
              (*register_auth_cb_)( ms.sender_, login, pass, sesskey, peer_salt, my_salt ) == false )
          {
            return;
          }

          /* prepare response htua packet */
          if( prepare_htua( pkt_salt, my_salt, sesskey, ms) == false )
          {
            FPRINTF(stderr,L"[%s:%d] Error in prepare_htua()\n",L""__FILE__,__LINE__);
            return;
          }

          /* send data back */
          socklen_t len = sizeof(ms.sender_);

          if( ::sendto( socket_, reinterpret_cast<const char *>(ms.data_), ms.size_, 0,
              reinterpret_cast<const struct sockaddr *>(&(ms.sender_)), len ) != static_cast<int>(ms.size_) )
          {
            FPRINTF(stderr,L"[%ls:%d] Error in sendto(%d)\n",L""__FILE__,__LINE__,socket_);
            perror("sendto");
            return;
          }
        }
        catch( common::exc e )
        {
          str s;
          e.to_string(s);
          FPRINTF(stderr,L"Error [%ls:%d]: %s\n",L""__FILE__,__LINE__,s.c_str());
        }
        catch( comm::exc e )
        {
          str s;
          e.to_string(s);
          FPRINTF(stderr,L"Error [%ls:%d]: %s\n",L""__FILE__,__LINE__,s.c_str());
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
        if( receiver_.start(min_threads_,max_threads_,timeout_ms_,retries_,handler_) == false ) 
        {
          THR(exc::rs_thread_start,false);
        }

        /* set thread entries */
        thread_.set_entry( receiver_ );
        handler_.socket( receiver_.socket() );

        /* launch receiver threads */
        if( thread_.start() == false ) { THR(exc::rs_thread_start,false); }

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

      auth_srv::auth_srv()
        : debug_(false), min_threads_(1), max_threads_(4), timeout_ms_(1000), retries_(10)
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

      auth_cli::auth_cli() : debug_(false), sock_(-1)
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
          /* unencrypted part */
          pbuf    outer;
          xdrbuf  xbo(outer);

          xbo << static_cast<int32_t>(msg::unicast_auth_p);

          if( public_key_.to_xdr(xbo) == false ) { THR(comm::exc::rs_xdr_error,false); }

          if( debug() )
          {
            PRINTF(L" ++ [%ld] : packet_type : %d\n",xbo.position(),msg::unicast_auth_p );
            PRINTF(L" ++ [%ld] : ",xbo.position()); public_key_.print();
          }

          if( outer.size() > m.max_len() ) { THR(comm::exc::rs_too_big,false); }

          outer.copy_to(m.data_,m.max_len());

          /* encrypted part */
          pbuf inner;
          xdrbuf xbi(inner);

          /* salt and session key is generated in auth() */
          xbi << xdrbuf::bindata_t( reinterpret_cast<const unsigned char *>(my_salt_.data()),static_cast<unsigned int>(saltbuf_t::preallocated_size) );
          xbi << login_;
          xbi << pass_;
          xbi << session_key_;

          if( debug() )
          {
            PRINTF(L"  ++ login: '%s' pass: '%s' sesskey: '%s'\n",
                   login_.c_str(),pass_.c_str(),session_key_.c_str());
          }

          /* generate session key */
          ustr session_key_v;

          if( server_public_key_.is_empty() == false )
          {
            if( server_public_key_.gen_sha1hex_shared_key(private_key_,session_key_v) == false )
            {
              THR(comm::exc::rs_sec_error,false);
            }
          }
          else
          {
            THR(comm::exc::rs_pubkey_empty,false);
          }

          if( debug() ) { PRINTF(L"  ++ Session Key: '%s'\n",session_key_v.c_str()); }

          /* compile packet */
          crypt_pkt::saltbuf_t  salt;
          crypt_pkt::keybuf_t   key;
          crypt_pkt::headbuf_t  head;
          crypt_pkt::databuf_t  data;
          crypt_pkt::footbuf_t  foot;

          csl_sec_gen_rand( salt.allocate(saltbuf_t::preallocated_size), saltbuf_t::preallocated_size );
          key.set( reinterpret_cast<const unsigned char *>(session_key_v.c_str()), (session_key_v.size()+1) );
          inner.t_copy_to(data);

          crypt_pkt pk;
          pk.use_exc(use_exc());

          if( !pk.encrypt( salt,key,head,data,foot ) )
          {
            THR(comm::exc::rs_crypt_pkt_error,false);
          }

          /* output packet */
          unsigned char * outputp = m.data_+outer.size();
          unsigned int retlen = outer.size()+head.size()+data.size()+foot.size();

          if( retlen > m.max_len() ) { THR(comm::exc::rs_too_big,false); }

          memcpy( outputp, head.data(), head.size() ); outputp += head.size();
          memcpy( outputp, data.data(), data.size() ); outputp += data.size();
          memcpy( outputp, foot.data(), foot.size() ); outputp += foot.size();

          if( debug() )
          {
            print_hex(L"  ++ UCAUTH PROL ",m.data_,outer.size());
            print_hex(L"  ++ UCAUTH HEAD ",head.data(),head.size() );
            print_hex(L"  ++ UCAUTH DATA ",data.data(),data.size() );
            print_hex(L"  ++ UCAUTH FOOT ",foot.data(),foot.size() );
            PRINTF(  L"  ++ UCAUTH size: %d\n",retlen );
          }

          /* return the data */
          m.size_ = retlen;
          return true;
        }
        catch( common::exc e )
        {
          str s;
          e.to_string(s);
          FPRINTF(stderr,L"Exception caught: %ls\n",s.c_str());
          THR(comm::exc::rs_common_error,false);
        }
        return false;
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

          if( debug() ) { PRINTF(L" -- [%ld] : packet_type : %d  size : %d\n",xbo.position(),packet_type,m.size_ ); }

          if( packet_type != msg::unicast_htua_p )
          {
            THR(comm::exc::rs_invalid_packet_type,false);
          }

          const unsigned char * ptrp = m.data_ + xbo.position();
          unsigned int          lenp = m.size_ - xbo.position();

          /* encrypted part */
          if( debug() ) { PRINTF(L"  -- Session Key: '%s'\n",session_key_.c_str()); }

          /* de-compile packet */
          crypt_pkt::keybuf_t   key;
          crypt_pkt::headbuf_t  head;
          crypt_pkt::databuf_t  data;
          crypt_pkt::footbuf_t  foot;

          head.set(ptrp,crypt_pkt::header_len());
          foot.set(ptrp+(lenp-crypt_pkt::footer_len()),crypt_pkt::footer_len());
          data.set(ptrp+(crypt_pkt::header_len()),
                   lenp-crypt_pkt::footer_len()-crypt_pkt::header_len());

          key.set( reinterpret_cast<const unsigned char *>(session_key_.c_str()),
            (session_key_.size()+1) );

          if( debug() )
          {
            print_hex(L"  ++ UCHTUA PROL ",m.data_,outer.size());
            print_hex(L"  ++ UCHTUA HEAD ",head.data(),head.size() );
            print_hex(L"  ++ UCHTUA DATA ",data.data(),data.size() );
            print_hex(L"  ++ UCHTUA FOOT ",foot.data(),foot.size() );
            print_hex(L"  ++ UCHTUA KEY  ",key.data(),key.size() );
          }

          crypt_pkt pk;
          pk.use_exc(use_exc());

          if( pk.decrypt( key,head,data,foot ) == false )
          {
            THR(comm::exc::rs_crypt_pkt_error,false);
          }

          /* xdr deserialize */
          pbuf inner;
          inner.append( data.data(), data.size() );
          xdrbuf xbi(inner);

          unsigned int sz=0;

          if( xbi.get_data( server_salt_,sz,saltbuf_t::preallocated_size ) == false )
          {
            THR(comm::exc::rs_xdr_error,false);
          }

          if( sz != saltbuf_t::preallocated_size )
          {
            THR(comm::exc::rs_xdr_error,false);
          }

          if( debug() ) { print_hex(L"  -- RAND ",server_salt_.data(),server_salt_.size()); }

          return true;
        }
        catch( common::exc e )
        {
          str s;
          e.to_string(s);
          FPRINTF(stderr,L"Exception caught: %ls\n",s.c_str());
          THR(comm::exc::rs_common_error,false);
        }
        return false;
      }

      namespace
      {
        static int init_sock(SAI & addr)
        {
          int sock = ::socket( AF_INET, SOCK_DGRAM, 0 );
          if( sock <= 0 ) { return sock; }

          socklen_t len = sizeof(addr);

          if( ::connect(sock, reinterpret_cast<const struct sockaddr *>(&addr), len) == -1 )
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
        if( public_key().is_empty() ) { THR(exc::rs_pubkey_empty,false); }

        sock_ = init_sock( addr_ );

        if( sock_ <= 0 ) { THRC(exc::rs_socket_failed,false); }

        return true;
      }

      namespace
      {
        static void gen_sess_key(common::ustr & k)
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

        if( public_key().is_empty() ) { THR(exc::rs_pubkey_empty,false); }

        /* generate salt and session key */
        csl_sec_gen_rand( my_salt_.allocate(saltbuf_t::preallocated_size), saltbuf_t::preallocated_size );
        gen_sess_key(session_key_);

        /* prepare auth packet */
        msg m;

        if( !prepare_auth( m ) ) { THR(exc::rs_pkt_error,false); }

        if( (err=::send( sock_, reinterpret_cast<const char *>(m.data_), m.size_ , 0 )) != static_cast<int>(m.size_) )
        {
          THRC(exc::rs_send_failed,false);
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
          err = ::recv(sock_,reinterpret_cast<char *>(m.data_), m.max_len(), 0);
          //
          if( err > 0 )
          {
            m.size_ = err;
            if( init_htua( m ) == false )
            {
              THR(exc::rs_pkt_error,false);
            }

            return true;
          }
          else
          {
            THRC(exc::rs_recv_failed,false);
          }
        }
        else if( err == 0 )
        {
          /* timed out */
          THR(exc::rs_timeout,false);
        }
        else
        {
          /* error */
          THRC(exc::rs_select_failed,false);
        }
      }
    } /* end of udp namespace */
  } /* end of comm namespace */
} /* end of csl namespace */

/* EOF */
