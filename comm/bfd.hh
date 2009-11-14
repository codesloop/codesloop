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
#include "codesloop/common/rdbuf.hh"
#include "codesloop/common/read_res.hh"
#include "codesloop/common/common.h"
#include "codesloop/common/obj.hh"
#ifdef __cplusplus

namespace csl
{
  using common::rdbuf;
  using common::read_res;

  namespace comm
  {
    /**
    This is a generic class that buffers read, recv and recvfrom operations on a given fd.

    It tries to read as many bytes possible to fill the internal buffer. The read-like operations
    may or may not read from the fd depending on the given function. For some it is feasible to return
    a buffer without touching the network.

    The rationale behind this is, it tries to decouple the network operations from the data needs of the
    application. For some scenarios the read multiplexing may execute unneccessary loops when not all data
    have been read from the socket. This class helps as long as it has enough space in the buffer.

    In other scenarios the data may arrive in smaller portions than it makes sense for the application.
    Buffering helps to merge small pieces together. However the application must keep in mind that it only
    have a limited buffer space, so care must be taken.

    The write-like operations are not buffered at all.

    The internal buffer space is initially 1k, this may grow dynamically up to 256k.
    */
    class bfd
    {
      public:
        bfd();
        bfd(int fd); ///<constructs the class w/ an open file descriptor

        ~bfd();

        void init(int fd); ///<initializes the class w/ an open file descriptor

        /**
        @brief read n bytes either from the buffer or the network
        @param sz is the number of bytes to be read
        @param timeout_ms is the number of miliseconds to wait for the data
        @return the buffer

        this function first checks wether there is data in the buffer. if so it returns that
        without trying to read from the network even if its size is smaller than the required amount.
        if no data is in the buffer then it tries to read big to fill the buffer.

        to enforce reading from the network use the read_some() family, and check the size() in the buffer.
        */
        read_res & read(uint64_t sz, uint32_t & timeout_ms, read_res & rr);
        read_res & recv(uint64_t sz, uint32_t & timeout_ms, read_res & rr); ///<same as read()
        read_res & recvfrom(uint64_t sz, SAI & from, uint32_t & timeout_ms, read_res & rr); ///<same as read()

        uint64_t read_some(uint32_t & timeout_ms);  ///<try to read, and return the number of bytes read
        uint64_t recv_some(uint32_t & timeout_ms);  ///<try to recv, and return the number of bytes recv'd
        uint64_t recvfrom_some(SAI & from, uint32_t & timeout_ms); ///<try to recvfrom, and return the number of bytes recv'd

        bool write(const uint8_t * data, uint64_t sz);  ///<write() to fd_ without buffering
        bool send(const uint8_t * data, uint64_t sz);   ///<send() to fd_ without buffering
        bool sendto(const uint8_t * data, uint64_t sz,const SAI & to); ///<sendto() on fd_ without buffering

        static const int ok_                =  0;
        static const int unknonwn_error_    = -1;
        static const int not_initialized_   = -2;
        static const int closed_            = -3;
        static const int fd_error_          = -4;
        static const uint64_t max_size_     = 256*1024;

        int state() const;         ///<returns the fd state
        uint64_t size() const;     ///<returns the available data size
        uint64_t n_free() const;   ///<returns how many bytes free in the buffer

        /**
        @brief checks wether data is available on the fd
        @return true if available
        @param timeout_ms tells how many milliseconds to wait for data

        timeout_ms will be modified to tell how many milliseconds left from the
        given timeout
        */
        bool can_read(uint32_t & timeout_ms);
        bool read_buf(read_res & res, uint64_t sz);  ///<reads from the buffer (no network operations)
        void shutdown();                             ///<shuts down the fd (only makes sense on sockets)
        void close();                                ///<closes the fd

        int file_descriptor() const { return fd_; }

      private:
        typedef rdbuf<512,max_size_> buf_t;

        static const int read_op_      = 1;
        static const int recv_op_      = 2;
        static const int recvfrom_op_  = 3;

        uint64_t internal_read( int op_type,
                                SAI & from,
                                uint32_t & timeout_ms );
        int        fd_;
        buf_t      buf_;

        CSL_OBJ(csl::comm,bfd);
        USE_EXC();
    };
  }
}

#endif /*__cplusplus*/
#endif /* _csl_comm_bfd_hh_included_ */
