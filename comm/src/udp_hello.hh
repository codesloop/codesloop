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

#ifndef _csl_comm_udp_hello_hh_included_
#define _csl_comm_udp_hello_hh_included_

#include "exc.hh"
#include "ecdh_key.hh"
#include "common.h"
#ifdef __cplusplus

namespace csl
{
  using sec::ecdh_key;

  namespace comm
  {
    class udp_hello
    {
      public:
        bool init( const unsigned char * buf,
                   unsigned int size );

        inline udp_hello() : use_exc_(true) {}

      private:
        /* internal */
        bool use_exc_;

        /* to be init()-ed */
        ecdh_key public_key_;

      public:
        /* public key */
        inline const ecdh_key & public_key() const { return public_key_; }

        /** @brief Specifies whether param should throw comm::exc exceptions
        @param yesno is the desired value to be set

        the default value for use_exc() is true, so it throws exceptions by default */
        inline void use_exc(bool yesno) { use_exc_ = yesno; }

        /** @brief Returns the current value of use_exc
        @return true if exc exceptions are used */
        inline bool use_exc() const { return use_exc_; }
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_comm_udp_hello_hh_included_ */
