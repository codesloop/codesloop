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

#ifndef _csl_comm_exc_hh_included_
#define _csl_comm_exc_hh_included_

/**
   @file comm/src/exc.hh
   @brief comm exception class for common
 */

#include "codesloop/common/str.hh"
#include "codesloop/common/cexc.hh"
#ifdef __cplusplus

namespace csl
{
  namespace comm
  {
    /**
    @brief common exception class used by com classes

    this class is used by the comm classes as an exception to be thrown
     */
    class exc : public csl::common::cexc
    {
      public:
        enum {
          rs_unknown,             ///<Unknown error.
          rs_socket_failed,       ///<socket() call failed
          rs_bind_failed,         ///<bind() call failed
          rs_listen_failed,       ///<listen() call failed
          rs_accept_failed,       ///<accept() call failed
          rs_connect_failed,      ///<connect() call failed
          rs_pubkey_empty,        ///<Required public key parameter not set.
          rs_privkey_empty,       ///<Required private key parameter not set.
          rs_xdr_error,           ///<XDR error.
          rs_send_failed,         ///<send() call failed.
          rs_recv_failed,         ///<recv() call failed.
          rs_timeout,             ///<Timed out.
          rs_select_failed,       ///<select() call failed.
          rs_thread_start,        ///<Thread start failed.
          rs_wsa_startup,         ///<Windows WSA Startup failed
          rs_getsockname_failed,  ///<getsockname() call failed.
          rs_invalid_packet_type, ///<invalid packet type
          rs_common_error,        ///<Error from common:: routines
          rs_sec_error,           ///<Error from sec:: routines
          rs_crypt_pkt_error,     ///<Error in crypt pkt
          rs_too_big,             ///<Too big data to be placed into the given buffer
          rs_pkt_error,           ///<Cannot encode/decode packet
          rs_null_param,          ///<NULL parameter supplied.
          rs_not_inited,          ///<Not initialized.
          rs_init_failed,         ///<Cannot initialize.
          rs_need_login,          ///<Need login.
          rs_need_pass,           ///<Need pass.
          rs_sesskey_empty,       ///<session key is not set
          rs_salt_size,           ///<invalid salt size
          rs_not_implemented,     ///<Function is not implemented
        };

        enum {
          cm_unknown,            ///<Unknown component
          cm_wsa,                ///<WSA
          cm_udp_recvr,          ///<UDP receiver
          cm_udp_hello_handler,  ///<UDP hello handler
          cm_udp_hello_cli,      ///<UDP hello client
          cm_udp_hello_srv,      ///<UDP hello server
          cm_udp_auth_handler,   ///<UDP auth handler
          cm_udp_auth_cli,       ///<UDP auth client
          cm_udp_auth_srv,       ///<UDP auth server
          cm_udp_data_handler,   ///<UDP data handler
          cm_udp_data_cli,       ///<UDP data client
          cm_udp_data_srv,       ///<UDP data server
        };

        /** @brief converts reason code to string */
        static const wchar_t * reason_string(int rc);

        /** @brief constructor
         *   @param component that caused the exception
         */
        exc(const wchar_t * component)
        : csl::common::cexc(component) {}

        /** @brief constructor
         *   @param reason is to tell why
         *   @param component that cause the exception
         */
        exc(int reason, const wchar_t * component)
        : csl::common::cexc(reason,component) {}

        /** @brief constructor
         *   @param reason is to tell why
         *   @param component that cause the exception
         *   @param txt provides some explanation
         */
        exc(int reason, const wchar_t * component, const wchar_t * txt)
        : csl::common::cexc(reason,component,txt) {}

        /** @brief constructor
         *   @param reason is to tell why
         *   @param component that cause the exception
         *   @param txt provides some explanation
         *   @param file tells which source file caused the error
         *   @param lin tells which line cause the error
         */
        exc(int reason, const wchar_t * component, const wchar_t * txt, const wchar_t * file, unsigned int line)
        : csl::common::cexc(reason,component,txt,file,line) {}
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_comm_exc_hh_included_ */
