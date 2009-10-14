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
   @file t__rdbuf.cc
   @brief @todo
 */

//#if 0
#ifndef DEBUG
#define DEBUG
#define DEBUG_ENABLE_INDENT
//#define DEBUG_VERBOSE
#endif /* DEBUG */
//#endif

#include "codesloop/comm/rdbuf.hh"
#include "codesloop/common/logger.hh"
#include "codesloop/common/common.h"
#include "codesloop/common/test_timer.h"
#include <assert.h>

using namespace csl::comm;
using namespace csl::common;
//using namespace csl::nthread;

/** @brief @todo */
namespace test_rdbuf {

  /*
  ** DEBUG support --------------------------------------------------------------------
  */
  static inline const wchar_t * get_namespace()   { return L"test_rdbuf"; }
  static inline const wchar_t * get_class_name()  { return L"test_rdbuf::noclass"; }
  static inline const wchar_t * get_class_short() { return L"noclass"; }

  void baseline() { rdbuf<> o; }

} /* end of test_rdbuf */

using namespace test_rdbuf;

int main()
{
  csl_common_print_results( "baseline          ", csl_common_test_timer_v0(baseline),"" );
  return 0;
}

/* EOF */

