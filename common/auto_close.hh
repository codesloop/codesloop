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

#ifndef _csl_common_auto_close_hh_included_
#define _csl_common_auto_close_hh_included_

/**
  @file auto_close.hh
  @brief @todo
*/

#include "codesloop/common/logger.hh"
#include "codesloop/common/obj.hh"
#include "codesloop/common/common.h"
#ifdef __cplusplus

namespace csl
{
  namespace common
  {
    class auto_close
    {
      public:
        auto_close() : fd_(-100) { }
        auto_close(int fd) : fd_(fd) { }

        virtual ~auto_close()
        {
          ENTER_FUNCTION();
          if( fd_ > 0 )
          {
            CSL_DEBUGF( L"closing fd:%d",fd_ );
            close_fd(fd_);
          }
          fd_ = -101;
          LEAVE_FUNCTION();
        }

        virtual void close_fd(int fd)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF( L"close_fd(%d)",fd );
          Close(fd);
          LEAVE_FUNCTION();
        }

        auto_close & operator=(int fd)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF( L"operator=(fd:%d)",fd );
          init(fd);
          RETURN_FUNCTION( *this );
        }

        void init(int fd)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF( L"init(fd:%d) [current fd:%d]",fd,fd_ );
          if( fd_ > 0 )
          {
            CSL_DEBUGF( L"closing already initialized fd:%d",fd_ );
            close_fd( fd_ );
          }
          fd_ = fd;
          LEAVE_FUNCTION();
        }

        int get() const
        {
          ENTER_FUNCTION();
          CSL_DEBUGF( L"get() => %d", fd_ );
          RETURN_FUNCTION( fd_ );
        }

      private:
        int fd_;

        CSL_OBJ(csl::common::,auto_close);
    };

    class auto_close_socket : public auto_close
    {
      public:
        auto_close_socket() : auto_close() { }
        auto_close_socket(int fd) : auto_close(fd) { }

        virtual void close_fd(int fd)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF( L"close_fd(fd:%d)",fd );
          CloseSocket(fd);
          LEAVE_FUNCTION();
        }

        CSL_OBJ(csl::common::,auto_close_socket);
    };

    class auto_close_shutdown_socket : public auto_close
    {
      public:
        auto_close_shutdown_socket() : auto_close() { }
        auto_close_shutdown_socket(int fd) : auto_close(fd) { }

        virtual void close_fd(int fd)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF( L"close_fd(fd:%d)",fd );
          ShutdownCloseSocket(fd);
          LEAVE_FUNCTION();
        }

        CSL_OBJ(csl::common::,auto_close_shutdown_socket);
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_common_auto_close_hh_included_ */
