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
#include "udp_data.hh"
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

  using std::string;

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
      **                         User Data Handler
      **
      **********************************************************************/

      bool handle_data_callback::send_reply( const saltbuf_t & old_salt,
                                             const saltbuf_t & new_salt,
                                             const SAI & addr,
                                             const std::string & sesskey,
                                             int sock,
                                             const b1024_t & data )
      {
        /* prepare data packet */
        msg m;
        data_handler helper;

        if( helper.prepare_data(old_salt,new_salt,sesskey,data,m) == false )
        {
          fprintf(stderr,"[%s:%d] prepare_data failed\n",__FILE__,__LINE__);
          return false;
        }

        if( (::sendto( sock, (const char *)m.data_, m.size_ , 0, (const struct sockaddr *)&addr, sizeof(addr) )) != (int)(m.size_) )
        {
          fprintf(stderr,"[%s:%d] sendto failed\n",__FILE__,__LINE__);
          return false;
        }

        return true;
      }

      /**********************************************************************
      **
      **                         Data Handler
      **
      **********************************************************************/

      /* data packet */
      bool udp::data_handler::get_salt( saltbuf_t & old_salt,
                                        const msg & m )
      {
        if( m.size_ < (sizeof(int32_t)) )  { THR(comm::exc::rs_null_param,comm::exc::cm_udp_data_handler,false); }

        pbuf    outer;
        outer.append(m.data_,(sizeof(int32_t)));
        xdrbuf  xbo(outer);

        int32_t packet_type = 0;
        xbo >> packet_type;

        if( packet_type != msg::data_p ) { THR(comm::exc::rs_invalid_packet_type,comm::exc::cm_udp_data_handler,false); }

        const unsigned char  * ptrp = m.data_ + xbo.position();

        old_salt.set(ptrp,crypt_pkt::header_len());

        return true;
      }

      bool udp::data_handler::init_data( saltbuf_t & new_salt,
                                         const string & sesskey,
                                         const msg & m,
                                         b1024_t & recvdta )
      {
        try
        {
          if( m.size_ < (sizeof(int32_t)) ) { THR(comm::exc::rs_null_param,comm::exc::cm_udp_data_handler,false); }
          if( sesskey.size() == 0 )         { THR(comm::exc::rs_sesskey_empty,comm::exc::cm_udp_data_handler,false); }

          /* unencrypted part */
          pbuf    outer;
          outer.append(m.data_,(sizeof(int32_t)));
          xdrbuf  xbo(outer);

          int32_t packet_type = 0;
          xbo >> packet_type;

          if( packet_type != msg::data_p ) { THR(comm::exc::rs_invalid_packet_type,comm::exc::cm_udp_data_handler,false); }

          const unsigned char  * ptrp = m.data_ + xbo.position();
          unsigned int           lenp = m.size_ - xbo.position();

          /* encrypted part */
          if( debug() )
          {
            printf(" -- [%ld] : packet_type : %d\n",xbo.position(),packet_type );
            printf("  -- Session Key: '%s'\n",sesskey.c_str());
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

          key.set((unsigned char *)sesskey.c_str(),sesskey.size()+1);

          crypt_pkt pk;
          pk.use_exc(use_exc());

          if( pk.decrypt( key,head,data,foot ) == false )
          {
            THR(comm::exc::rs_crypt_pkt_error,comm::exc::cm_udp_data_handler,false);
          }

          /* xdr deserialize */
          pbuf inner;
          inner.append( data.data(), data.size() );
          xdrbuf xbi(inner);

          unsigned int sz=0;

          if( xbi.get_data( new_salt,sz,salt_size_v) == false )
          {
            THR(comm::exc::rs_xdr_error,comm::exc::cm_udp_data_handler,false);
          }

          if( sz != 8 )
          {
            THR(comm::exc::rs_xdr_error,comm::exc::cm_udp_data_handler,false);
          }

          if( xbi.get_data( recvdta,sz,m.max_len() ) == false )
          {
            THR(comm::exc::rs_xdr_error,comm::exc::cm_udp_data_handler,false);
          }

          if( debug() )
          {
            print_hex("  -- oldsalt ",head.data(),head.size());
            print_hex("  -- newsalt ",new_salt.data(),new_salt.size());
            print_hex("  -- data    ",recvdta.data(),recvdta.size());
          }

          return true;
        }
        catch( common::exc e )
        {
          std::string s;
          e.to_string(s);
          fprintf(stderr,"Exception caught: %s\n",s.c_str());
          THR(comm::exc::rs_common_error,comm::exc::cm_udp_data_handler,false);
        }
        return false;
      }

      /* data packet */
      bool udp::data_handler::prepare_data( const saltbuf_t & old_salt,
                                            const saltbuf_t & new_salt,
                                            const string & sesskey,
                                            const b1024_t & senddta,
                                            msg & m )
      {
        try
        {
          if( senddta.size() == 0 ) { THR(comm::exc::rs_null_param,comm::exc::cm_udp_data_handler,false); }
          if( old_salt.size() != salt_size_v ) { THR(comm::exc::rs_salt_size,comm::exc::cm_udp_data_handler,false); }
          if( new_salt.size() != salt_size_v ) { THR(comm::exc::rs_salt_size,comm::exc::cm_udp_data_handler,false); }
          if( sesskey.size() == 0 ) { THR(comm::exc::rs_sesskey_empty,comm::exc::cm_udp_data_handler,false); }

          /* unencrypted part */
          pbuf    outer;
          xdrbuf  xbo(outer);

          xbo << (int32_t)msg::data_p;

          if( outer.size() > m.max_len() ) { THR(comm::exc::rs_too_big,comm::exc::cm_udp_data_handler,false); }

          outer.copy_to(m.data_,m.max_len());

          /* encrypted part */
          pbuf inner;
          xdrbuf xbi(inner);

          xbi << xdrbuf::bindata_t( (const unsigned char *)new_salt.data(),new_salt.size() );
          xbi << xdrbuf::bindata_t( senddta.data(),senddta.size() );

          if( debug() )
          {
            printf(" ++ [%ld] : packet_type : %d\n",xbo.position(),msg::data_p );
            printf("  ++ Session Key: '%s'\n",sesskey.c_str());
          }

          /* compile packet */
          crypt_pkt::saltbuf_t  salt(old_salt);
          crypt_pkt::keybuf_t   key;
          crypt_pkt::headbuf_t  head;
          crypt_pkt::databuf_t  data;
          crypt_pkt::footbuf_t  foot;

          key.set( (unsigned char *)sesskey.c_str(), sesskey.size()+1 );
          inner.t_copy_to(data);

          crypt_pkt pk;
          pk.use_exc(use_exc());

          if( !pk.encrypt( salt,key,head,data,foot ) )
          {
            THR(comm::exc::rs_crypt_pkt_error,comm::exc::cm_udp_data_handler,false);
          }

          /* output packet */
          unsigned char * outputp = m.data_+outer.size();
          unsigned int     retlen = outer.size()+head.size()+data.size()+foot.size();

          if( retlen > m.max_len() ) { THR(comm::exc::rs_too_big,comm::exc::cm_udp_data_handler,false); }

          memcpy( outputp, head.data(), head.size() ); outputp += head.size();
          memcpy( outputp, data.data(), data.size() ); outputp += data.size();
          memcpy( outputp, foot.data(), foot.size() ); outputp += foot.size();

          if( debug() )
          {
            print_hex("  ++ DATA PROL ",m.data_,outer.size());
            print_hex("  ++ DATA HEAD ",head.data(),head.size() );
            print_hex("  ++ DATA DATA ",data.data(),data.size() );
            print_hex("  ++ DATA FOOT ",foot.data(),foot.size() );
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
          THR(comm::exc::rs_common_error,comm::exc::cm_udp_data_handler,false);
        }
        return false;
      }

      /**/
      void udp::data_handler::operator()()
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
          saltbuf_t old_salt;
          saltbuf_t new_salt;
          string    sesskey;
          b1024_t   recvdta;

          if( get_salt(old_salt,ms) == false ) { return; }

          /* lookup session key */
          if( lookup_session_cb_ &&
              (*lookup_session_cb_)(old_salt, ms.sender_, sesskey) == false )
          {
            return;
          }

          if( init_data(new_salt, sesskey, ms, recvdta) == false ) { return; }

          /* handle data */
          if( handle_data_cb_ &&
              (*handle_data_cb_)(old_salt, new_salt, ms.sender_, sesskey, socket_, recvdta) == false )
          {
            return;
          }

          /* register new salt for looking up session key */
          if( update_session_cb_ &&
              (*update_session_cb_)(old_salt, new_salt, ms.sender_, sesskey) == false )
          {
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
      **                         Data Server
      **
      **********************************************************************/

      bool data_srv::start()
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
          THR(exc::rs_thread_start,exc::cm_udp_data_srv,false);
        }

        /* set thread entries */
        thread_.set_entry( receiver_ );
        handler_.socket( receiver_.socket() );

        /* launch receiver threads */
        if( thread_.start() == false ) { THR(exc::rs_thread_start,exc::cm_udp_data_srv,false); }

        return true;
      }

      bool data_srv::stop()
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

      data_srv::data_srv()
        : use_exc_(true), debug_(false), min_threads_(1), max_threads_(20), timeout_ms_(1000), retries_(10)
      {
      }

      data_srv::~data_srv()
      {
        stop();
      }

      /**********************************************************************
      **
      **                         Data Client
      **
      **********************************************************************/

      bool data_cli::recv(b1024_t & data,unsigned int timeout_ms)
      {
        if( init() == false ) { THR(exc::rs_init_failed,exc::cm_udp_data_cli,false); }

        /* wait for data arrival */
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

        int err = ::select(sock_+1,&rfds,NULL,NULL,ptv);

        if( err > 0 )
        {
          msg m;

          err = ::recv(sock_,(char *)m.data_, m.max_len(), 0);
          //
          if( err > 0 )
          {
            m.size_ = err;
            data_handler helper;
            saltbuf_t new_salt;

            if( helper.init_data( new_salt, session_key_, m, data  ) == false )
            {
              THR(exc::rs_pkt_error,exc::cm_udp_data_cli,false);
            }

            server_salt_ = new_salt;
            return true;
          }
          else
          {
            THRC(exc::rs_recv_failed,exc::cm_udp_data_cli,false);
          }
        }
        else if( err == 0 )
        {
          /* timed out */
          THR(exc::rs_timeout,exc::cm_udp_data_cli,false);
        }
        else
        {
          /* error */
          THRC(exc::rs_select_failed,exc::cm_udp_data_cli,false);
        }
      }

      bool data_cli::send(const b1024_t & data)
      {
        if( init() == false ) { THR(exc::rs_init_failed,exc::cm_udp_data_cli,false); }
        if( my_salt_.size() != saltbuf_t::preallocated_size )
        {
          { THR(exc::rs_salt_size,exc::cm_udp_data_cli,false); }
        }

        /* prepare data packet */
        saltbuf_t new_salt;
        csl_sec_gen_rand( new_salt.allocate(saltbuf_t::preallocated_size), saltbuf_t::preallocated_size );

        msg m;
        data_handler helper;

        if( helper.prepare_data(my_salt_,new_salt,session_key_,data,m) == false )
        {
          THR(exc::rs_pkt_error,exc::cm_udp_data_cli,false);
        }

        if( (::send( sock_, (const char *)m.data_, m.size_ , 0 )) != (int)(m.size_) )
        {
          THRC(exc::rs_send_failed,exc::cm_udp_data_cli,false);
        }

        my_salt_ = new_salt;
        return true;
      }

      data_cli::data_cli() : use_exc_(true), debug_(false), sock_(-1)
      {
        memset( &addr_,0,sizeof(addr_) );
        addr_.sin_family        = AF_INET;
        addr_.sin_addr.s_addr   = htonl(INADDR_LOOPBACK);
      }

      data_cli::~data_cli()
      {
        if( sock_ > 0 ) ShutdownCloseSocket( sock_ );
        sock_ = -1;
      }

      namespace
      {
        static int init_sock(SAI & addr)
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

      bool data_cli::init()
      {
        if( sock_ > 0 ) return true;

        /* session key has been set ? */
        if( session_key_.size() == 0 ) { THR(exc::rs_sesskey_empty,exc::cm_udp_data_cli,false); }

        sock_ = init_sock( addr_ );

        if( sock_ <= 0 ) { THRC(exc::rs_socket_failed,exc::cm_udp_data_cli,false); }

        return true;
      }

    } /* end of udp namespace */
  } /* end of comm namespace */
} /* end of csl namespace */

/* EOF */
