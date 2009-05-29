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
#include "udp_data_entry.hh"
#include "common.h"
#include <sys/types.h>
#include <string.h>

namespace csl
{
  namespace comm
  {
    void udp_data_entry::operator()(void)
    {
      SAI                 cliaddr;
      socklen_t           len = sizeof(cliaddr);
      int                 recvd;
      udp_pkt             pkt;
      udp_pkt::b1024_t    dtabuf;

      /* init packet handler */
      pkt.use_exc(false);
      pkt.server_info(srv().server_info());
      pkt.own_privkey(srv().private_key());

      /* packet loop */
      while( stop_me() == false )
      {
        /* wait for incoming packets */
        fd_set fds;
        FD_ZERO( &fds );
        FD_SET( socket(), &fds );
        struct timeval tv = { 1, 0 };

        int err = ::select( socket()+1,&fds,NULL,NULL,&tv );

        if( err < 0 )
        {
          fprintf(stderr,"Error [%s:%d]\n",__FILE__,__LINE__);
          break;    // TODO : error handling
        }
        else if( err == 0 )
        {
          // select timed out
          continue;
        }

        /* receive packet */
        recvd = recvfrom( socket(), (char *)pkt.data(), pkt.maxlen(), 0, (struct sockaddr *)&cliaddr, &len );

        if( !recvd )
        {
          fprintf(stderr,"Error [%s:%d]\n",__FILE__,__LINE__);
          continue; // TODO : error handling
        }

        try
        {
          /* process incoming data packet */
          if( pkt.init_data(recvd,dtabuf) == false ) // TODO this may got to a separate thread
          {
            fprintf(stderr,"Error [%s:%d]\n",__FILE__,__LINE__);
            continue; // TODO : error handling
          }

          if( srv().on_data_arrival( pkt, dtabuf, cliaddr ) == false )
          {
            fprintf(stderr,"Error [%s:%d]\n",__FILE__,__LINE__);
            continue; // TODO error handling
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
    }

    udp_data_entry::~udp_data_entry()
    {
    }

    udp_data_entry::udp_data_entry( udp_srv & srv ) : udp_srv_entry(srv)
    {
    }
  };
};

/* EOF */
