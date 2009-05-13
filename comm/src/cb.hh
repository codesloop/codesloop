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

#include "ecdh_key.hh"
#ifdef __cplusplus
#include <string>

namespace csl
{
  using csl::sec::ecdh_key;
  using std::string;

  namespace comm
  {
    class cb
    {
      public:

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
                                  const string & host,              // in
                                  unsigned short port,              // in
                                  ecdh_key & my_public_key,         // out
                                  bool & login_required,            // out
                                  bool & pass_required ) = 0;       // out
        };

        class valid_creds
        {
          public:
            virtual ~valid_creds() {}
            virtual bool operator()( const ecdh_key & peer_public_key, // in
                                     const string & host,              // in
                                     unsigned short port,              // in
                                     const string & login,             // in
                                     const string & pass ) = 0;        // in
        };

        class accept
        {
          public:
            virtual ~accept() {}
        };
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_comm_cb_hh_included_ */
