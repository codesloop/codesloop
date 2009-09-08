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

#ifndef _csl_common_hlprs_hh_included_
#define _csl_common_hlprs_hh_included_

/**
   @file hlprs.hh
   @brief Various templated helpers
 */

#include "codesloop/common/common.h"
#ifdef __cplusplus

namespace csl
{
  namespace common
  {
    template <int> struct copy_n_uchars {};
    template <> struct copy_n_uchars<1>
    {
      inline copy_n_uchars(unsigned char * dst, const unsigned char * src) { dst[0] = src[0]; }
    };
    template <> struct copy_n_uchars<2>
    {
      inline copy_n_uchars(unsigned char * dst, const unsigned char * src) { dst[0] = src[0]; dst[1] = src[1]; }
    };
    template <> struct copy_n_uchars<4>
    {
      inline copy_n_uchars(unsigned char * dst, const unsigned char * src)
      { dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2]; dst[3] = src[3]; }
    };
    template <> struct copy_n_uchars<8>
    {
      inline copy_n_uchars(unsigned char * dst, const unsigned char * src)
      { dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2]; dst[3] = src[3];
        dst[4] = src[4]; dst[5] = src[5]; dst[6] = src[6]; dst[7] = src[7]; }
    };

    const unsigned char * byte_bits();
    const unsigned char * byte_last_free();
    const unsigned char * byte_last_used();
  }
}

#endif /* __cplusplus */
#endif /* _csl_common_hlprs_hh_included_ */
