/*
Copyright (c) 2008,2009, David Beck, Tamas Foldi

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

#ifndef _csl_common_exc_hh_included_
#define _csl_common_exc_hh_included_

/**
   @file csl_common/src/exc.hh
   @brief common exception class for common
 */

#include "cexc.hh"
#include "str.hh"
#include "logger.hh"
#ifdef __cplusplus

namespace csl
{
  namespace common
  {
    /**
    @brief common exception class used by common classes

    this class is used by the common classes as an exception to be thrown
     */
    class exc : public csl::common::cexc
    {
      public:
        enum {
          rs_unknown,        ///<Unknown error.
          rs_invalid_param,  ///<Invalid parameter received
          rs_cannot_append,  ///<Cannot append to pbuf
          rs_cannot_get,     ///<Cannot get data
          rs_xdr_eof,        ///<End of xdr data.
          rs_xdr_invalid,    ///<Invald xdr data
          rs_empty,          ///<Empty container
          rs_conv_error,     ///<Cannot convert character
          rs_invalid_state,  ///<Component state invalid
          rs_lookup_error,   ///<Lookup error.
          rs_out_of_memory,  ///<Can not allocate memory
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
#endif /* _csl_common_exc_hh_included_ */
