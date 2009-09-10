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

#ifndef _csl_comm_bfd_hh_included_
#define _csl_comm_bfd_hh_included_

/**
   @file bfd.hh
   @brief buffered file descriptor (fd)
 */

#include "codesloop/comm/sai.hh"
#include "codesloop/comm/read_res.hh"
#include "codesloop/common/common.h"
#include "codesloop/common/obj.hh"
#ifdef __cplusplus

namespace csl
{
  namespace comm
  {
    class bfd
    {
      public:
        bfd();
        bfd(int fd);

        ~bfd();

        void init(int fd);

        read_res read(uint32_t sz, uint32_t timeout_ms);
        read_res recv(uint32_t sz, uint32_t timeout_ms);
        read_res recvfrom(uint32_t sz, SAI & from, uint32_t timeout_ms);

        read_res & read(uint32_t sz, uint32_t timeout_ms, read_res & rr);
        read_res & recv(uint32_t sz, uint32_t timeout_ms, read_res & rr);
        read_res & recvfrom(uint32_t sz, SAI & from, uint32_t timeout_ms, read_res & rr);

        bool write(uint8_t * data, uint32_t sz);
        bool send(uint8_t * data, uint32_t sz);
        bool sendto(uint8_t * data, uint32_t sz,const SAI & to);

        static const int ok_                =  0;
        static const int unknonwn_error_    = -1;
        static const int not_initialized_   = -2;
        static const int closed_            = -3;
        static const int fd_error_          = -4;

        int state() const;
        uint32_t size() const;
        uint32_t n_free() const;

        bool can_read(uint32_t timeout_ms);
        bool read_buf(read_res & res, uint32_t sz);

        void shutdown();

      private:
        int        fd_;
        uint16_t   start_;
        uint16_t   len_;
        uint8_t    buf_[65536]; // 64k

        CSL_OBJ(csl::comm,bfd);
    };
  }
}

#endif /*__cplusplus*/
#endif /* _csl_comm_bfd_hh_included_ */
