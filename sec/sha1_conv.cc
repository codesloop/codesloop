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

#include <openssl/sha.h>
#include <stdio.h>
#include "codesloop/sec/csl_sec.h"

/**
   @file sha1_conv.cc
   @brief the implementation of SHA1 hexdigest generation
*/
CSL_CDECL
void * csl_sec_sha1_conv(
        const void  * in,
        size_t        inlen,
        void        * out,
        size_t      * outlen)
{
  PARAM_NOT_NULL2(in,0);
  PARAM_NOT_ZERO2(inlen,0);
  PARAM_NOT_NULL2(out,0);
  PARAM_NOT_NULL2(outlen,0);

  *outlen = SHA1_HEX_DIGEST_STR_LENGTH;

  char tmp[SHA_DIGEST_LENGTH];
  unsigned char * ptmp = (unsigned char *)tmp;

  SHA1((const unsigned char *)in, inlen, (unsigned char *)tmp);

  char * tx = (char *)out;
  char * ptx = tx;
  static char hx[] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };

  for( int i=0;i<20;++i )
  {
    *ptx = hx[(((*ptmp)>>4)&0x0f)]; ++ptx;
    *ptx = hx[(*ptmp)&0x0f]; ++ptx; ++ptmp;
  }
  *ptx = 0;
  return tx;
}

/* EOF */
