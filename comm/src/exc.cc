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
#include "str.hh"
#include "common.h"

/**
  @file csl_comm/src/exc.cc
  @brief implementation of comm::exc
 */

namespace csl
{
  namespace comm
  {
    const char * exc::reason_string(int rc)
    {
      switch( rc )
      {
        case rs_socket_failed:        return "<socket() call failed";
        case rs_connect_failed:       return "connect() call failed";
        case rs_bind_failed:          return "bind() call failed";
        case rs_listen_failed:        return "listen() call failed";
        case rs_accept_failed:        return "accept() call failed";
        case rs_pubkey_empty:         return "Required public key parameter not set.";
        case rs_privkey_empty:        return "Required private key parameter not set.";
        case rs_xdr_error:            return "XDR error.";
        case rs_send_failed:          return "send() call failed.";
        case rs_recv_failed:          return "recv() call failed.";
        case rs_timeout:              return "Timed out.";
        case rs_select_failed:        return "select() call failed.";
        case rs_thread_start:         return "Thread start failed.";
        case rs_wsa_startup:          return "WSAStartup failed.";
        case rs_getsockname_failed:   return "getsockname() call failed.";
        case rs_invalid_packet_type:  return "invalid packet type";
        case rs_common_error:         return "common:: error";
        case rs_sec_error:            return "sec:: error";
        case rs_crypt_pkt_error:      return "error in crypt_pkt";
        case rs_too_big:              return "data too big for the given buffer";
        case rs_pkt_error:            return "Cannot encode/decode packet";
        case rs_null_param:           return "NULL parameter supplied.";
        case rs_not_inited:           return "Not initialized.";
        case rs_init_failed:          return "Cannot initialize.";
        case rs_need_login:           return "Need login.";
        case rs_need_pass:            return "Need pass.";
        case rs_sesskey_empty:        return "session key is not set";
        case rs_salt_size:            return "invalid salt size";
        case rs_unknown:
          default:                    return "Unknown reason";
      };
    }

    const char * exc::component_string(int cm)
    {
      switch( cm )
      {
        case cm_wsa:                return "comm::wsa";
        case cm_udp_recvr:          return "comm::udp::recvr";
        case cm_udp_hello_handler:  return "comm::udp::hello_handler";
        case cm_udp_hello_cli:      return "comm::udp::hello_cli";
        case cm_udp_hello_srv:      return "comm::udp::hello_srv";
        case cm_udp_auth_handler:   return "comm::udp::auth_handler";
        case cm_udp_auth_cli:       return "comm::udp::auth_cli";
        case cm_udp_auth_srv:       return "comm::udp::auth_srv";
        case cm_udp_data_handler:   return "comm::udp::data_handler";
        case cm_udp_data_cli:       return "comm::udp::data_cli";
        case cm_udp_data_srv:       return "comm::udp::data_srv";
        case cm_unknown:
          default:                return "unknown component";
      };
    }

    void exc::to_string(common::str & res)
    {
      common::str t("Exception");
      if( file_.size() > 0 && line_ > 0 )
      {
        char tx[200];
        SNPRINTF(tx,199,"(%s:%d): ",file_.c_str(),line_);
        t += tx;
      }
      t += " [";
      t += component_string(component_);
      t += "] [";
      t += reason_string(reason_);
      t += "] ";
      if( text_.size() > 0 ) t+= text_;
      res.swap(t);
    }

    exc::exc() : reason_(rs_unknown), component_(cm_unknown) {}
    exc::~exc() {}
    /* public interface */
  };
};

/* EOF */
