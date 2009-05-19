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
#include "udp_pkt.hh"
#include "udp_srv.hh"
#include "udp_hello_entry.hh"
#include "csl_common.hh"
#include "ecdh_key.hh"
#include "crypt_pkt.hh"
#include "common.h"
#include <sys/types.h>
#include <string.h>

namespace csl
{
  using common::pbuf;
  using common::xdrbuf;
  using sec::ecdh_key;
  using sec::crypt_pkt;

  namespace comm
  {
    void udp_hello_entry::operator()(void)
    {
      SAI             cliaddr;
      socklen_t       len = sizeof(cliaddr);
      int             recvd;
      unsigned char   buffer[65536];

      while( stop_me() == false )
      {
        fd_set fds;
        FD_ZERO( &fds );
        FD_SET( socket(), &fds );
        struct timeval tv = { 1, 0 };

        int err = ::select( socket()+1,&fds,NULL,NULL,&tv );

        if( err < 0 )       break;    // TODO : error handling
        else if( err == 0 ) continue; // TODO : error handling

        /* receive packet */
        recvd = recvfrom( socket(), (char *)buffer, sizeof(buffer), 0, (struct sockaddr *)&cliaddr, &len );
        fprintf( stderr,"Received %d bytes [%s]\n",recvd,buffer);

        if( !recvd ) continue; // error handling

        try
        {
          /* decode packet */
          ecdh_key peer_public_key;

          pbuf pb;
          pb.append(buffer,recvd);
          xdrbuf xb(pb);

          int32_t packet_type;
          xb >> packet_type;

          if( packet_type != udp_pkt::hello_p ) continue; // TODO error handling
          if( !peer_public_key.from_xdr(xb) ) continue; // TODO error handling

          /* check peer key */
          if( valid_key_cb_ &&
              (*valid_key_cb_)(peer_public_key) == false ) continue; // TODO error handling

          /* gather packet data */
          udp_srv_info   inf(srv().server_info());
          ecdh_key       public_key(srv().server_info().public_key());
          bignum         private_key(srv().private_key());
          string         peer_host(inet_ntoa(cliaddr.sin_addr));

          /* hello callback */
          if( hello_cb_ &&
              (*hello_cb_)(
                peer_public_key,
                peer_host,
                cliaddr.sin_port,
                inf,
                private_key) == false ) continue; // TODO error handling

          /* generate session key */
          std::string session_key;

          if( !peer_public_key.gen_sha1hex_shared_key(private_key,session_key) )
          {
            continue; // TODO error handling
          }

          /* unencrypted part */
          crypt_pkt::databuf_t odta;
          pbuf   outer;
          xdrbuf xbo(outer);

          xbo << (int32_t)udp_pkt::olleh_p;
          public_key.to_xdr(xbo);
          outer.t_copy_to(odta);

          /* encrypted part */
          pbuf inner;
          xdrbuf xbi(inner);

          xbi << (int32_t)inf.need_login();
          xbi << (int32_t)inf.need_pass();

          /* compile packet */
          crypt_pkt::saltbuf_t  salt;
          crypt_pkt::keybuf_t   key;
          crypt_pkt::headbuf_t  head;
          crypt_pkt::databuf_t  data;
          crypt_pkt::footbuf_t  foot;

          salt.set((unsigned char *)"00000000",8);
          key.set((unsigned char *)session_key.c_str(),session_key.size());
          inner.t_copy_to(data);

          crypt_pkt pk;
          if( !pk.encrypt( salt,key,head,data,foot ) )
          {
            continue; // TODO error handling
          }

          /* output packet */
          crypt_pkt::databuf_t output;

          unsigned char * outputp = output.allocate( odta.size()+head.size()+data.size()+foot.size() );

          memcpy( outputp, odta.data(), odta.size() ); outputp += odta.size();
          memcpy( outputp, head.data(), head.size() ); outputp += head.size();
          memcpy( outputp, data.data(), data.size() ); outputp += data.size();
          memcpy( outputp, foot.data(), foot.size() ); outputp += foot.size();

          /* send data back */
          if( sendto( socket(),
              (const char *)output.data(), output.size(), 0,
               (struct sockaddr *)&cliaddr, len ) != (int)output.size() )
          {
            continue; // TODO error
          }
        }
        catch( common::exc e )
        {
          printf("Error [%s:%d]\n",__FILE__,__LINE__);
        }
        catch( comm::exc e )
        {
          printf("Error [%s:%d]\n",__FILE__,__LINE__);
        }
      }
    }

    udp_hello_entry::~udp_hello_entry()
    {
    }

    udp_hello_entry::udp_hello_entry( udp_srv & srv )
    : udp_srv_entry(srv), valid_key_cb_(0), hello_cb_(0)
    {
    }

    void udp_hello_entry::valid_key_cb( cb::valid_key & vkcb )
    {
      valid_key_cb_ = &vkcb;
    }

    void udp_hello_entry::hello_cb( cb::hello & hcb )
    {
      hello_cb_ = &hcb;
    }
  };
};

/* EOF */
