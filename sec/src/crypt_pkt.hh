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

#ifndef _csl_sec_crypt_pkt_hh_included_
#define _csl_sec_crypt_pkt_hh_included_

/**
   @file crypt_pkt.hh

   Encryption is based on AES and UMAC
 */

#include "csl_common.hh"
#ifdef __cplusplus
#include <memory>

namespace csl
{
  namespace sec
  {
    /** @todo document me */
    class crypt_pkt : public csl::common::obj
    {
      public:
        typedef common::tbuf<8>       buf8_t;
        typedef common::tbuf<16>      buf16_t;
        typedef common::tbuf<64>      buf64_t;
        typedef common::tbuf<1024>    buf1024_t;

        typedef buf8_t      saltbuf_t;
        typedef buf64_t     keybuf_t;
        typedef buf8_t      headbuf_t;
        typedef buf1024_t   databuf_t;
        typedef buf8_t      footbuf_t;

        enum {
          header_sz = 8,
          footer_sz = 8
        };

        static inline unsigned int header_len() { return header_sz; }
        static inline unsigned int footer_len() { return footer_sz; }

        bool encrypt( const saltbuf_t & salt,
                      const keybuf_t & key,
                      headbuf_t & header,
                      databuf_t & data,
                      footbuf_t & footer );

        bool decrypt( const keybuf_t & key,
                      const headbuf_t & header,
                      databuf_t & data,
                      const footbuf_t & footer );

        inline crypt_pkt() {}
        inline virtual ~crypt_pkt() {}

      private:
        crypt_pkt(const crypt_pkt & other) {}
        crypt_pkt & operator=(const crypt_pkt & other) { return *this; }

        CSL_OBJ(csl::sec,crypt_pkt);
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_sec_crypt_pkt_hh_included_ */
