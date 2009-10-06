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
   @file t__inpvec.cc
   @brief Tests to verify queue
 */

#if 0
#ifndef DEBUG
#define DEBUG
#endif /* DEBUG */
#endif

#include "codesloop/common/queue.hh"
#include "codesloop/common/test_timer.h"
#include "codesloop/common/ustr.hh"
#include "codesloop/common/logger.hh"
#include "codesloop/common/common.h"
#include <assert.h>
#include <list>
#include <string>

using namespace csl::common;

/** @brief contains tests related to queue */
namespace test_queue {

  void baseline_queue()
  {
    queue<uint64_t> q;
  }

  void baseline_stdlist()
  {
    std::list<uint64_t> q;
  }


} // end of test_queue

using namespace test_queue;

int main()
{
  csl_common_print_results( "baseline_queue    ", csl_common_test_timer_v0(baseline_queue),"" );
  csl_common_print_results( "baseline_stdlist  ", csl_common_test_timer_v0(baseline_stdlist),"" );
  return 0;
}

// EOF
