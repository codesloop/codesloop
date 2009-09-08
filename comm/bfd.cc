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

/**
   @file conn.cc
   @brief @todo
 */

#include "codesloop/comm/bfd.hh"
#include "codesloop/common/libev/evwrap.h"

namespace csl
{
  namespace comm
  {
    bfd::bfd() : fd_(bfd::not_initialized_), start_(0), len_(0), use_exc_(false) {}

    bfd::~bfd()
    {
      if( fd_ > 0 ) { CloseSocket( fd_ ); }
      fd_ = closed_;
    }

    read_res bfd::read(size_t sz, uint32_t timeout_ms)
    {
      read_res ret;
      if( fd_ <= 0 ) { ret.failed_=true; return ret; }
      if( can_read(timeout_ms) )
      {
      }
      else { ret.timed_out_=true; }
      return ret;
    }

    bool bfd::write(uint8_t * data, size_t sz)
    {
      return false;
    }

    read_res bfd::recv(size_t sz, uint32_t timeout_ms)
    {
      read_res ret;
      if( fd_ <= 0 ) { ret.failed_=true; return ret; }
      if( can_read(timeout_ms) )
      {
      }
      else { ret.timed_out_=true; }
      return ret;
    }

    bool bfd::send(uint8_t * data, size_t sz)
    {
      return false;
    }

    read_res bfd::recvfrom(size_t sz, SAI & from, uint32_t timeout_ms)
    {
      read_res ret;
      if( fd_ <= 0 ) { ret.failed_=true; return ret; }
      if( can_read(timeout_ms) )
      {
      }
      else { ret.timed_out_=true; }
      return ret;
    }

    bool bfd::sendto(uint8_t * data, size_t sz,const SAI & to)
    {
      return false;
    }

    int bfd::state() const
    {
      if( fd_ > 0 ) return ok_;
      else          return fd_;
    }

    bool bfd::can_read(uint32_t timeout_ms)
    {
      if( fd_ <= 0 ) return false;

      fd_set         fds;
      unsigned long  timeout_sec    = timeout_ms/1000;
      unsigned long  timeout_usec   = (timeout_ms%1000)*1000;
      struct timeval tv             = { timeout_sec, timeout_usec };

      FD_ZERO( &fds );
      FD_SET( fd_, &fds );

      int err = ::select( fd_+1,&fds,NULL,NULL,&tv );

      if( err < 0 ) /* error */
      {
        CloseSocket( fd_ );
        fd_ = fd_error_;
        return false;
      }
      else if( err == 0 ) /* timeout */
      {
        return false;
      }
      else /* ok, can read */
      {
        return true;
      }
    }

    /* not implemented
    bool bfd::can_write(uint32_t timeout_ms)
    {
      if( fd_ <= 0 ) return false;
      return false;
    }*/

    size_t bfd::size() const
    {
      return len_;
    }

    size_t bfd::n_free() const
    {
      return (sizeof(buf_)-(len_+start_));
    }
  }
}

/* EOF */
