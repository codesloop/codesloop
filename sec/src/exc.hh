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

#ifndef _csl_sec_exc_hh_included_
#define _csl_sec_exc_hh_included_

/**
   @file csl_sec/src/exc.hh
   @brief common exception class for csl::sec
 */

#include "csl_common.hh"
#ifdef __cplusplus

namespace csl
{
  namespace sec
  {
    /**
    @brief common exception class used by csl::sec classes

    this class is used by the csl::sec classes as an exception to be thrown
     */
    class exc : public csl::common::cexc
    {
      public:
        enum {
          rs_unknown,        ///<Unknown error.
          rs_salt_size,      ///<Invalid salt size.
          rs_null_key,       ///<Null key received.
          rs_too_big,        ///<Data too big.
          rs_rand_failed,    ///<Cannot generate random.
          rs_header_size,    ///<Invalid header size.
          rs_footer_size,    ///<Invalid ooter size.
          rs_null_data,      ///<Data too short for encryption.
          rs_cksum,          ///<Checksum mismatch.
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
#endif /* _csl_sec_exc_hh_included_ */
