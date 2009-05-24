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

#ifndef _csl_comm_cb_hh_included_
#define _csl_comm_cb_hh_included_

#include "udp_srv_info.hh"
#include "udp_chann.hh"
#include "bignum.hh"
#include "ecdh_key.hh"
#include "session_data.hh"
#include "tbuf.hh"
#ifdef __cplusplus
#include <string>

namespace csl
{
  using csl::sec::ecdh_key;
  using csl::sec::bignum;
  using std::string;

  namespace comm
  {
    class cb
    {
      public:
        typedef struct sockaddr_in SAI;

        class valid_key
        {
          public:
            virtual ~valid_key() {}
            virtual bool operator()( const ecdh_key & peer_public_key ) = 0; // in
        };

        class hello
        {
          public:
            virtual ~hello() {}
            virtual bool operator()( const ecdh_key & peer_public_key, // in
                                     const SAI & addr,                 // in
                                     udp_srv_info & info,              // out
                                     bignum & my_private_key ) = 0;    // out
        };

        class valid_creds
        {
          public:
            virtual ~valid_creds() {}
            virtual bool operator()( const ecdh_key & peer_public_key,    // in
                                     const SAI & addr,                    // in
                                     const string & login,                // in
                                     const string & pass) = 0;            // in
        };

        class create_session
        {
          public:
            virtual ~create_session() {}
            virtual session_data * operator()( const ecdh_key & peer_public_key,  // in
                                               const ecdh_key & own_public_key,   // in
                                               const bignum & own_private_key,    // in
                                               const SAI & addr,                  // in
                                               const string & login,              // in
                                               const string & pass,               // in
                                               const string & session_key ) = 0;  // in
        };

        class cleanup_session
        {
          public:
            virtual ~cleanup_session() {}
            virtual void operator()( session_data * sess ) = 0;   // in
        };

        class data_arrival
        {
          public:
            typedef common::tbuf<1024> b1024_t;

            virtual ~data_arrival() {}
            virtual bool operator()( session_data * sess,         // in
                                     udp_chann & channel_,        // in
                                     const b1024_t & data ) = 0;  // in
        };
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_comm_cb_hh_included_ */
