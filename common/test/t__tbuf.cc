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
   @file t__tbuf.cc
   @brief Tests to verify tbuf
 */

#include "pbuf.hh"
#include "tbuf.hh"
#include "str.hh"
#include "test_timer.h"
#include "zfile.hh"
#include "common.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <string>

using csl::common::pbuf;
using csl::common::tbuf;
using csl::common::str;

/** @brief contains tests related to tbuf */
namespace test_tbuf {

  /** @test baseline for performance comparison */
  void tbuf_baseline()
  {
    tbuf<128> b;
  }

  /** @test baseline for performance comparison */
  void pbuf_baseline()
  {
    pbuf b;
  }

  /** @test baseline for performance comparison */
  void string_baseline()
  {
    std::string b;
  }

  /** @test baseline for performance comparison */
  void str_baseline()
  {
    str b;
  }

  /** @test @todo */
  void tbuf_hello()
  {
    tbuf<128> b;
    b.set((unsigned char *)"Hello",6);
  }

  /** @test @todo */
  void pbuf_hello()
  {
    pbuf b;
    b.append((unsigned char *)"Hello",6);
  }

  /** @test @todo */
  void string_hello()
  {
    std::string b;
    b = "Hello";
  }

  /** @test @todo */
  void str_hello()
  {
    str b;
    b = L"Hello";
  }

} // end of test_tbuf

using namespace test_tbuf;

int main()
{
  csl_common_print_results( "tbuf_baseline      ", csl_common_test_timer_v0(tbuf_baseline),"" );
  csl_common_print_results( "pbuf_baseline      ", csl_common_test_timer_v0(pbuf_baseline),"" );
  csl_common_print_results( "str_baseline       ", csl_common_test_timer_v0(str_baseline),"" );
  csl_common_print_results( "string_baseline    ", csl_common_test_timer_v0(string_baseline),"" );

  csl_common_print_results( "tbuf_hello         ", csl_common_test_timer_v0(tbuf_hello),"" );
  csl_common_print_results( "pbuf_hello         ", csl_common_test_timer_v0(pbuf_hello),"" );
  csl_common_print_results( "str_hello          ", csl_common_test_timer_v0(str_hello),"" );
  csl_common_print_results( "string_hello       ", csl_common_test_timer_v0(string_hello),"" );

  return 0;
}

/* EOF */
