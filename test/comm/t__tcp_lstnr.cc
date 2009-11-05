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
  @file t__tcp_lstnr.cc
  @brief @todo
*/

#if 0
#ifndef DEBUG
#define DEBUG
#define DEBUG_ENABLE_INDENT
//#define DEBUG_VERBOSE
#endif /* DEBUG */
#endif

#include "codesloop/comm/bfd.hh"
#include "codesloop/comm/tcp_lstnr.hh"
#include "codesloop/comm/initcomm.hh"
#include "codesloop/common/logger.hh"
#include "codesloop/common/common.h"
#include "codesloop/common/test_timer.h"
#include <assert.h>

using namespace csl::comm;
using namespace csl::comm::tcp;
//using namespace csl::common;
//using namespace csl::nthread;

/** @brief @todo */
namespace test_tcp_lstnr {

  /*
  ** DEBUG support --------------------------------------------------------------------
  */
  static inline const wchar_t * get_namespace()   { return L"test_tcp_lstnr"; }
  static inline const wchar_t * get_class_name()  { return L"test_tcp_lstnr::noclass"; }
  static inline const wchar_t * get_class_short() { return L"noclass"; }

  void baseline() { lstnr o; }

  class my_tcp_handler : public csl::comm::handler
  {
    public:
      virtual bool on_connected( connid_t id,
                                 const SAI & sai,
                                 bfd & buf_fd )
      {
        ENTER_FUNCTION();
        CSL_DEBUGF( L"on_connected(id:%lld, sai:(%s:%d), bfd)",
                     id,
                     inet_ntoa(sai.sin_addr),
                     ntohs(sai.sin_port) );
        RETURN_FUNCTION(true);
      }

      virtual bool on_data_arrival( connid_t id,
                                    const SAI & sai,
                                    bfd & buf_fd )
      {
        ENTER_FUNCTION();
        CSL_DEBUGF( L"on_data_arrival(id:%lld, sai:(%s:%d), bfd)",
                     id,
                     inet_ntoa(sai.sin_addr),
                     ntohs(sai.sin_port) );
        RETURN_FUNCTION(true);
      }

      virtual void on_disconnected( connid_t id,
                                    const SAI & sai )
      {
        ENTER_FUNCTION();
        CSL_DEBUGF( L"on_disconnected(id:%lld, sai:(%s:%d))",
                     id,
                     inet_ntoa(sai.sin_addr),
                     ntohs(sai.sin_port) );
        LEAVE_FUNCTION();
      }

      CSL_OBJ(test_tcp_lstnr,my_tcp_handler);
  };

  void conn()
  {
    ENTER_FUNCTION();
    in_addr_t   saddr = inet_addr("127.0.0.1");
    SAI         addr;

    ::memset( &addr,0,sizeof(addr) );
    ::memcpy( &(addr.sin_addr),&saddr,sizeof(saddr) );

    addr.sin_family  = AF_INET;
    addr.sin_port    = htons(49912);

    lstnr l;
    my_tcp_handler h;
    l.init(h, addr);
    l.start();
    CSL_DEBUGF( L"the listener has been started. wait 7 secs for connections" );
    assert( l.exit_event().wait(7000) == false );
    CSL_DEBUGF( L"the listener will be stopped on purpose after 7 seconds" );
    l.stop();
    assert( l.exit_event().wait(7000) == true );
    LEAVE_FUNCTION();
  }

} /* end of test_tcp_lstnr */

using namespace test_tcp_lstnr;

int main()
{
  initcomm w;

  csl_common_print_results( "baseline          ", csl_common_test_timer_v0(baseline),"" );
  conn();
  return 0;
}

/* EOF */

