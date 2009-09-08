/*
Copyright (c) 2008,2009, CodeSLoop Team

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

#include "codesloop/comm/exc.hh"
#include "codesloop/common/str.hh"
#include "codesloop/common/common.h"

/**
  @file comm/src/exc.cc
  @brief implementation of comm::exc
 */

namespace csl
{
  namespace comm
  {
    const wchar_t * exc::reason_string(int rc)
    {
      switch( rc )
      {
        case rs_socket_failed:        return L"socket() call failed";
        case rs_connect_failed:       return L"connect() call failed";
        case rs_bind_failed:          return L"bind() call failed";
        case rs_listen_failed:        return L"listen() call failed";
        case rs_accept_failed:        return L"accept() call failed";
        case rs_pubkey_empty:         return L"Required public key parameter not set.";
        case rs_privkey_empty:        return L"Required private key parameter not set.";
        case rs_xdr_error:            return L"XDR error.";
        case rs_send_failed:          return L"send() call failed.";
        case rs_recv_failed:          return L"recv() call failed.";
        case rs_timeout:              return L"Timed out.";
        case rs_select_failed:        return L"select() call failed.";
        case rs_thread_start:         return L"Thread start failed.";
        case rs_wsa_startup:          return L"WSAStartup failed.";
        case rs_getsockname_failed:   return L"getsockname() call failed.";
        case rs_invalid_packet_type:  return L"invalid packet type";
        case rs_common_error:         return L"common:: error";
        case rs_sec_error:            return L"sec:: error";
        case rs_crypt_pkt_error:      return L"error in crypt_pkt";
        case rs_too_big:              return L"data too big for the given buffer";
        case rs_pkt_error:            return L"Cannot encode/decode packet";
        case rs_null_param:           return L"NULL parameter supplied.";
        case rs_not_inited:           return L"Not initialized.";
        case rs_init_failed:          return L"Cannot initialize.";
        case rs_need_login:           return L"Need login.";
        case rs_need_pass:            return L"Need pass.";
        case rs_sesskey_empty:        return L"session key is not set";
        case rs_salt_size:            return L"invalid salt size";
        case rs_not_implemented:      return L"Function is not implemented.";
        case rs_unknown:
          default:                    return L"Unknown reason";
      };
    }
  };
};

/* EOF */
