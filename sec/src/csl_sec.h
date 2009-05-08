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

#ifndef _csl_sec_csl_sec_h_included_
#define _csl_sec_csl_sec_h_included_

/**
   @file csl_sec.h
   @brief Some common security related functions
*/

#include "common.h"

/* sha1 support functions */

#ifndef SHA1_HEX_DIGEST_STR_LENGTH
#  define SHA1_HEX_DIGEST_STR_LENGTH 41
#endif /*SHA1_HEX_DIGEST_STR_LENGTH*/

#ifndef CSL_EC_STRENGTH_MIN
#  define CSL_EC_STRENGTH_MIN 192
#endif /*CSL_EC_STRENGTH_MIN*/

#ifndef CSL_EC_STRENGTH_MAX
#  define CSL_EC_STRENGTH_MAX 521
#endif /*CSL_EC_STRENGTH_MAX*/

/**
    @brief Generates an SHA1 hexdigest of the given buffer
    @param in input buffer to be used
    @param inlen the length of the input buffer
    @param out where to place the result
    @param outlen the length of the generated data placed here (always SHA1_HEX_DIGEST_STR_LENGTH)
    @return the same as out or NULL if failed
*/
CSL_CDECL
void * csl_sec_sha1_conv(
        const void *in,
        size_t inlen,
        void *out,
        size_t *outlen);


#endif /* _csl_sec_csl_sec_h_included_ */
