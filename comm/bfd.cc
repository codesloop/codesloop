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
   @file bfd.cc
   @brief buffered file descriptor (fd)
 */

#if 0
#ifndef DEBUG
#define DEBUG
#define DEBUG_ENABLE_INDENT
//#define DEBUG_VERBOSE
#endif /* DEBUG */
#endif /* 0 */

#include "codesloop/comm/exc.hh"
#include "codesloop/comm/bfd.hh"
#include "codesloop/common/libev/evwrap.h"
#include "codesloop/common/logger.hh"

#ifndef BFD_DEBUG_STATE
#define BFD_DEBUG_STATE(WHICH) \
  CSL_DEBUGF(L"%s : [fd:%d buf.start:%lld buf.len:%lld buf.buflen:%lld buf.n_free:%lld]", \
             WHICH, \
             fd_, \
             buf_.len(), \
             buf_.start(), \
             buf_.buflen(), \
             buf_.n_free() );
#endif /*BFD_DEBUG_STATE*/

#ifndef BFD_DEBUG_STATE_RR
#define BFD_DEBUG_STATE_RR(WHICH,RR) \
  CSL_DEBUGF(L"%s : [fd:%d buf.start:%lld buf.len:%lld buf.buflen:%lld buf.n_free:%lld "\
              "|RR data:%p bytes:%lld timed_out:%s failed:%s]", \
             WHICH, \
             fd_, \
             buf_.len(), \
             buf_.start(), \
             buf_.buflen(), \
             buf_.n_free(),\
             (RR).data(), \
             (RR).bytes(), \
             ((RR).timed_out() == true ? "TRUE":"FALSE"), \
             ((RR).failed() == true ? "TRUE":"FALSE") \
            );
#endif /*BFD_DEBUG_STATE*/


namespace csl
{
  namespace comm
  {
    bfd::bfd() : fd_(bfd::not_initialized_), use_exc_(true) { }

    bfd::bfd(int fd) : fd_(fd), use_exc_(true)
    {
      ENTER_FUNCTION();
      CSL_DEBUGF( L"bfd(fd:%d)",fd );
      CSL_DEBUG_ASSERT( fd > 0 );
      LEAVE_FUNCTION();
    }

    bfd::~bfd()
    {
      this->close();
    }

    uint64_t bfd::internal_read( int op_type,
                                 SAI & from,
                                 uint32_t & timeout_ms )
    {
      ENTER_FUNCTION();
      CSL_DEBUGF( L"internal_read(op_type:%d,from,timeout_ms:%d,rr)", op_type, timeout_ms );
      BFD_DEBUG_STATE("old");
      int err = 0;
      uint64_t ret = 0;
      read_res tmp;

      if( fd_ <= 0 ) { CSL_DEBUGF(L"invalid fd:%d",fd_); }
      else
      {
        uint64_t read_amount = buf_t::preallocated_size_;

        // check if we can still fit into the preallocated space
        if( buf_.buflen() < buf_t::preallocated_size_ )
        {
          read_amount = buf_t::preallocated_size_ - buf_.buflen();
        }
        else if( buf_.buflen() >= buf_t::preallocated_size_ )
        {
          read_amount = ((buf_.buflen()/buf_t::preallocated_size_)+4) *
                         buf_t::preallocated_size_;
        }

        while( true )
        {
          buf_.reserve( read_amount, tmp );
          if( tmp.bytes() == 0 )
          {
            CSL_DEBUGF(L"cannot allocate more space");
            break;
          }
          else if( can_read(timeout_ms) )
          {
            err = -1;

            switch( op_type )
            {
              case read_op_:
              {
                err = ::read( fd_,tmp.data(),static_cast<size_t>(tmp.bytes()) );
                CSL_DEBUGF(L"read(fd:%d, ptr:%p, len:%lld) => %d",
                            fd_,
                            tmp.data(),
                            tmp.bytes(),
                            err );
                break;
              }

              case recv_op_:
              {
                err = ::recv( fd_,tmp.data(),static_cast<size_t>(tmp.bytes()), 0 );
                CSL_DEBUGF(L"recv(fd:%d, ptr:%p, len:%lld, 0) => %d",
                            fd_,
                            tmp.data(),
                            tmp.bytes(),
                            err );
                break;
              }

              case recvfrom_op_:
              {
                socklen_t slen = sizeof(SAI);
                err = ::recvfrom( fd_,tmp.data(),static_cast<size_t>(tmp.bytes()), 0,
                                  reinterpret_cast<struct sockaddr *>(&from),
                                  &slen );
                CSL_DEBUGF(L"recvfrom(fd:%d, ptr:%p, len:%lld, 0, from, len) => %d from [%s:%d]",
                            fd_,
                            tmp.data(),
                            tmp.bytes(),
                            err,
                            inet_ntoa(from.sin_addr),
                            ntohs(from.sin_port) );
                break;
              }

              default:
              {
                THR(comm::exc::rs_unknown_op,ret);
              }
            };

            if( err < 0 )
            {
              CSL_DEBUGF(L"closing bad socket:%d",fd_);
              CloseSocket( fd_ );
              fd_ = fd_error_;
              buf_.adjust( tmp, 0 );
              break;
            }
            else if( err == 0 )
            {
              CSL_DEBUGF(L"peer closed connection. closing socket:%d",fd_);
              CloseSocket( fd_ );
              fd_ = closed_;
              buf_.adjust( tmp, 0 );
              break;
            }
            else if( err == static_cast<int>(tmp.bytes()) )
            {
              CSL_DEBUGF(L"filled the whole reserved %lld bytes, retry reading %lld bytes",
                         read_amount, read_amount*2 );
              read_amount *= 2;
              ret += err;
            }
            else // err < tmp.bytes()
            {
              CSL_DEBUGF(L"read %d bytes into %lld bytes reserved",err,tmp.bytes());
              ret += err;
              buf_.adjust( tmp, err );
              break;
            }
          }
          else
          {
            CSL_DEBUGF(L"cannot read");
            buf_.adjust( tmp, 0 );
            break;
          }
        }
      }
      RETURN_FUNCTION( ret );
    }

    uint64_t bfd::read_some(uint32_t & timeout_ms)
    {
      ENTER_FUNCTION();
      CSL_DEBUGF( L"read_some(timeout_ms:%d)", timeout_ms );
      SAI sai;
      uint64_t ret = internal_read( read_op_,sai,timeout_ms );
      CSL_DEBUGF( L"read_some(timeout_ms:%d) => %lld", timeout_ms,ret );
      RETURN_FUNCTION( ret );
    }

    uint64_t bfd::recv_some(uint32_t & timeout_ms)
    {
      ENTER_FUNCTION();
      CSL_DEBUGF( L"recv_some(timeout_ms:%d)", timeout_ms );
      SAI sai;
      uint64_t ret = internal_read( recv_op_,sai,timeout_ms );
      CSL_DEBUGF( L"recv_some(timeout_ms:%d) => %lld", timeout_ms,ret );
      RETURN_FUNCTION( ret );
    }

    uint64_t bfd::recvfrom_some(SAI & from, uint32_t & timeout_ms)
    {
      ENTER_FUNCTION();
      CSL_DEBUGF( L"recvfrom_some(from, timeout_ms:%d)", timeout_ms );
      uint64_t ret = internal_read( recvfrom_op_,from,timeout_ms );
      CSL_DEBUGF( L"recvfrom_some(from, timeout_ms:%d) => %lld",timeout_ms,ret );
      RETURN_FUNCTION( ret );
    }

    read_res & bfd::read(uint64_t sz, uint32_t & timeout_ms, read_res & ret)
    {
      ENTER_FUNCTION();
      CSL_DEBUGF( L"read(sz:%lld, timeout_ms:%d)", sz, timeout_ms );
      BFD_DEBUG_STATE("old");

      ret.reset();

      if( size() > 0 ) { buf_.get( sz,ret ); }
      else if( fd_ <= 0 )
      {
        CSL_DEBUGF( L"invalid fd:%d",fd_ );
        ret.failed( true );
      }
      else
      {
        SAI from;
        if( internal_read( read_op_,from,timeout_ms ) > 0 )  { buf_.get( sz,ret );    }
        else if( timeout_ms == 0 )                           { ret.timed_out( true ); }
      }
      BFD_DEBUG_STATE_RR( "new",ret );
      RETURN_FUNCTION( ret );
    }

    read_res & bfd::recv(uint64_t sz, uint32_t & timeout_ms, read_res & ret)
    {
      ENTER_FUNCTION();
      CSL_DEBUGF( L"recv(sz:%lld, timeout_ms:%d)", sz, timeout_ms );
      BFD_DEBUG_STATE("old");

      ret.reset();

      if( size() > 0 ) { buf_.get( sz,ret ); }
      else if( fd_ <= 0 )
      {
        CSL_DEBUGF( L"invalid fd:%d",fd_ );
        ret.failed( true );
      }
      else
      {
        SAI from;
        if( internal_read( recv_op_,from,timeout_ms ) > 0 )  { buf_.get( sz,ret );    }
        else if( timeout_ms == 0 )                           { ret.timed_out( true ); }
      }
      BFD_DEBUG_STATE_RR( "new",ret );
      RETURN_FUNCTION( ret );
    }

    read_res & bfd::recvfrom(uint64_t sz, SAI & from, uint32_t & timeout_ms, read_res & ret)
    {
      ENTER_FUNCTION();
      CSL_DEBUGF( L"recvfrom(sz:%lld, &from, timeout_ms:%d)", sz, timeout_ms );
      BFD_DEBUG_STATE("old");

      ret.reset();

      if( size() > 0 ) { buf_.get( sz,ret ); }
      else if( fd_ <= 0 )
      {
        CSL_DEBUGF( L"invalid fd:%d",fd_ );
        ret.failed( true );
      }
      else
      {
        if( internal_read( recvfrom_op_,from,timeout_ms ) > 0 )  { buf_.get( sz,ret );    }
        else if( timeout_ms == 0 )                               { ret.timed_out( true ); }
      }
      BFD_DEBUG_STATE_RR( "new",ret );
      RETURN_FUNCTION( ret );
    }

    bool bfd::read_buf(read_res & res, uint64_t sz)
    {
      ENTER_FUNCTION();
      CSL_DEBUGF( L"read_buf(res,sz:%lld)",sz );
      BFD_DEBUG_STATE("old");
      buf_.get( sz, res );
      bool ret = (res.bytes() > 0);
      BFD_DEBUG_STATE_RR( "new",res );
      CSL_DEBUGF( L"read_buf(res,sz:%lld) => %s",sz,(ret==true?"TRUE":"FALSE") );
      RETURN_FUNCTION( ret );
    }

    bool bfd::can_read(uint32_t & timeout_ms)
    {
      ENTER_FUNCTION();
      CSL_DEBUGF( L"can_read(timeout_ms:%d)  fd_:%d",timeout_ms,fd_ );

      if( fd_ <= 0 )
      {
        CSL_DEBUGF( L"invalid fd: %d",fd_ );
        RETURN_FUNCTION( false );
      }

      struct timeval start_time,end_time;

      if( timeout_ms > 0 ) gettimeofday( &start_time, NULL );

      fd_set         fds;
      unsigned long  timeout_sec    = timeout_ms/1000;
      unsigned long  timeout_usec   = (timeout_ms%1000)*1000;
      struct timeval tv             = { timeout_sec, timeout_usec };

      FD_ZERO( &fds );
      FD_SET( fd_, &fds );

      int err = ::select( fd_+1,&fds,NULL,NULL,&tv );

      if( timeout_ms > 0 )
      {
        gettimeofday( &end_time, NULL );

        // diff seconds
        long diff = 1000000*(static_cast<long>(end_time.tv_sec)-
                             static_cast<long>(start_time.tv_sec));

        // diff usecs
        diff += (static_cast<long>(end_time.tv_usec)-
                 static_cast<long>(start_time.tv_usec));

        // convert to ms
        diff /= 1000;

        uint32_t dt = static_cast<uint32_t>(diff);

        // set remaining time
        if( dt > timeout_ms ) timeout_ms = 0;
        else                  timeout_ms -= dt;
      }

      if( err < 0 ) /* error */
      {
        CSL_DEBUGF( L"select(...) => ERROR %d [%s] (closing socket)",
                    err, strerror(errno) );

        CloseSocket( fd_ );
        fd_ = fd_error_;
        RETURN_FUNCTION( false );
      }
      else if( err == 0 ) /* timeout */
      {
        CSL_DEBUGF( L"timed out" );
        RETURN_FUNCTION( false );
      }
      else /* ok, can read */
      {
        CSL_DEBUGF( L"remaining time: %d",timeout_ms );
        RETURN_FUNCTION( true );
      }
    }

    uint64_t bfd::n_free() const { return buf_.n_free(); }
    uint64_t bfd::size()   const { return buf_.len();    }

    void bfd::shutdown()
    {
      ENTER_FUNCTION();
      CSL_DEBUGF( L"shutdown() fd:%d",fd_ );
      if( fd_ > 0 ) { ShutdownSocket(fd_); }
      LEAVE_FUNCTION();
    }

    void bfd::close()
    {
      ENTER_FUNCTION();
      CSL_DEBUGF( L"close() fd:%d",fd_ );
      if( fd_ > 0 ) { CloseSocket(fd_); }
      fd_ = closed_;
      LEAVE_FUNCTION();
    }

    void bfd::init(int fd)
    {
      ENTER_FUNCTION();
      CSL_DEBUGF( L"init(fd:%d)",fd );
      CSL_DEBUG_ASSERT( fd > 0 );
      fd_ = fd;
      LEAVE_FUNCTION();
    }

    int bfd::state() const
    {
      if( fd_ > 0 ) return ok_;
      else          return fd_;
    }

    bool bfd::write(const uint8_t * data, uint64_t sz)
    {
      ENTER_FUNCTION();
      int32_t err = 0;
      bool    ret = false;
      CSL_DEBUGF( L"write(data:%p, sz:%lld)",data,sz );

      if( !data || !sz ) { CSL_DEBUGF( L"invalid params");    goto bail; }
      if( fd_ <= 0 )     { CSL_DEBUGF( L"invalid fd:%d",fd_); goto bail; }

      err = ::write( fd_, data, static_cast<size_t>(sz) );

      if( err < 0 )
      {
        CSL_DEBUGF( L"write(fd:%d, ptr:%p, sz:%lld) ERROR %d [%s]",
                    fd_, data, sz, err,strerror(errno) );

        CloseSocket( fd_ );
        fd_ = fd_error_;
      }
      else if( err == 0 )
      {
        CSL_DEBUGF( L"write(fd:%d, ptr:%p, sz:%lld) ERROR (returned 0)", fd_, data, sz );
      }
      else
      {
        ret = true;
      }

    bail:
      CSL_DEBUGF( L"write(data:%p, sz:%lld) => %s",data,sz,(ret==true?"TRUE":"FALSE") );
      RETURN_FUNCTION( ret );
    }

    bool bfd::send(const uint8_t * data, uint64_t sz)
    {
      ENTER_FUNCTION();
      int32_t err = 0;
      bool    ret = false;
      CSL_DEBUGF( L"send(data:%p, sz:%lld)",data,sz );

      if( !data || !sz ) { CSL_DEBUGF( L"invalid params"); goto bail; }
      if( fd_ <= 0 )     { CSL_DEBUGF( L"invalid fd:%d",fd_); goto bail; }

      err = ::send( fd_, data, static_cast<size_t>(sz), 0 );

      if( err < 0 )
      {
        CSL_DEBUGF( L"send(fd:%d, ptr:%p, sz:%lld, 0) ERROR (returned %d)", fd_, data, sz, err );
        ShutdownCloseSocket( fd_ );
        fd_ = fd_error_;
      }
      else if( err == 0 )
      {
        CSL_DEBUGF( L"send(fd:%d, ptr:%p, sz:%lld, 0) SOCKET CLOSED (returned 0)", fd_, data, sz );
        ShutdownCloseSocket( fd_ );
        fd_ = closed_;
      }
      else
      {
        ret = true;
      }

    bail:
      CSL_DEBUGF( L"send(data:%p, sz:%lld) => %s",data,sz,(ret==true?"TRUE":"FALSE") );
      RETURN_FUNCTION( ret );
    }

    bool bfd::sendto(const uint8_t * data, uint64_t sz,const SAI & to)
    {
      ENTER_FUNCTION();
      int32_t   err  = 0;
      bool      ret  = false;
      socklen_t slen = sizeof(SAI);
      CSL_DEBUGF( L"sendto(data:%p, sz:%lld, to:%s:%d)",data,sz,inet_ntoa(to.sin_addr),ntohs(to.sin_port) );

      if( !data || !sz ) { CSL_DEBUGF( L"invalid params"); goto bail; }
      if( fd_ <= 0 )     { CSL_DEBUGF( L"invalid fd:%d",fd_); goto bail; }


      err = ::sendto( fd_, data, static_cast<size_t>(sz), 0,
                      reinterpret_cast<const struct sockaddr *>(&to),
                      slen );

      if( err < 0 )
      {
        CSL_DEBUGF( L"sendto(fd:%d, ptr:%p, sz:%lld, 0, %s:%d) ERROR %d [%s]",
                    fd_, data, sz, err, inet_ntoa(to.sin_addr),ntohs(to.sin_port),
                    strerror(errno) );

        ShutdownCloseSocket( fd_ );
        fd_ = fd_error_;
      }
      else if( err == 0 )
      {
        CSL_DEBUGF( L"send(fd:%d, ptr:%p, sz:%lld, 0, %s:%d) SOCKET CLOSED (returned 0)",
                    fd_, data, sz, inet_ntoa(to.sin_addr),ntohs(to.sin_port) );
        ShutdownCloseSocket( fd_ );
        fd_ = closed_;
      }
      else
      {
        ret = true;
      }

    bail:
      CSL_DEBUGF( L"sendto(data:%p, sz:%d, to:%s:%d) => %s",
                  data,sz,inet_ntoa(to.sin_addr),ntohs(to.sin_port),
                  (ret==true?"TRUE":"FALSE") );
      RETURN_FUNCTION( ret );
    }
  }
}

/* EOF */
