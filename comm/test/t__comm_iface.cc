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

/**
   @file t__comm_iface.cc
   @brief Tests to verify comm_iface routines
 */

#include "udp_cli.hh"
#include "udp_srv.hh"
#include "udp_pkt.hh"
#include "test_timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//using namespace csl::common;
//using namespace csl::sec;

/** @brief contains tests related to comm interface */
namespace test_comm_iface {

  void print_hex(const char * prefix,const void * vp,size_t len)
  {
    unsigned char * hx = (unsigned char *)vp;
    printf("%s: ",prefix);
    for(size_t i=0;i<len;++i) printf("%.2X",hx[i]);
    printf("\n");
  }

  void iface0()
  {
  }

} // end of test_comm_iface

using namespace test_comm_iface;

int main()
{
  //csl_common_print_results( "simplest      ", csl_common_test_timer_i1(simplest,0),"" );

  return 0;
}

/* EOF */
