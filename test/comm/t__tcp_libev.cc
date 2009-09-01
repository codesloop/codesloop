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
   @file t__tcp_libev.cc
   @brief @todo
 */

#if 0
#ifndef DEBUG
#define DEBUG
#define DEBUG_ENABLE_INDENT
//#define DEBUG_VERBOSE
#endif /* DEBUG */
#endif

#include "libev/evwrap.h"
#include "wsa.hh"
#include "logger.hh"
#include "common.h"
#include <assert.h>

using namespace csl::comm;
//using namespace csl::common;

/** @brief @todo */
namespace test_tcp_libev {

#define PRINT_BACKEND(V,B) printf("  backend: %26s [%s]\n",#B,(((V&B)!=0)?"OK":"--"))

  void print_backends(unsigned int b)
  {
    PRINT_BACKEND(b,EVBACKEND_SELECT);
    PRINT_BACKEND(b,EVBACKEND_POLL);
    PRINT_BACKEND(b,EVBACKEND_EPOLL);
    PRINT_BACKEND(b,EVBACKEND_KQUEUE);
    PRINT_BACKEND(b,EVBACKEND_DEVPOLL);
    PRINT_BACKEND(b,EVBACKEND_PORT);
  }

  void which_backends()
  {
    printf("\nev_supported_backends()\n");    print_backends(ev_supported_backends());
    printf("\nev_recommended_backends()\n");  print_backends(ev_recommended_backends());
    printf("\nev_embeddable_backends()\n");   print_backends(ev_embeddable_backends());

    struct ev_loop * evs = ev_loop_new (EVFLAG_AUTO);
    printf("\nEVFLAG_AUTO\n"); print_backends(ev_backend(evs));
    ev_loop_destroy(evs);
    /* on linux this gave me EVBACKEND_EPOLL, which is what I desired */
  }

  void timed_listen()
  {
    //ev_timer_init( &timer,
  }

} // end of test_tcp_libev

using namespace test_tcp_libev;

int main()
{
  wsa w;
  which_backends();
  return 0;
}
/* EOF */
