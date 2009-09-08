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
   @file t__schedule.cc
   @brief Tests to check and measure various sched::schedule features
 */

#include "codesloop/common/test_timer.h"
#include "codesloop/sched/schedule.hh"
#include "exc.hh"
#include "codesloop/common/common.h"
#include "codesloop/common/mpool.hh"
#include <assert.h>
#include <stdlib.h>

using namespace csl::slt3;
using namespace csl::sched;

/** @brief contains tests related to sched::schedule */
namespace test_schedule {

  /** @todo document me */
  void baseline()
  {
    schedule s;
  }

#if 0
  /** @todo document me */
  void usage1()
  {
    peer p;
    bool caught = false;
    try
    {
      assert( p.create() == false );
    }
    catch( csl::sched::exc e )
    {
      caught = true;
    }
    assert( caught == true );
  }

  /** @todo document me */
  void usage2()
  {
    peer p;
    assert( p.find_by_id(199999999) == false );
    assert( p.find_by_cn("Whatever I put here") == false );
    assert( p.find_by_cn(std::string("More garbage")) == false );
  }

  /** @todo document me */
  void usage3()
  {
    peer p;
    ecdh_key pubk;
    pubk.algname("prime192v3");
    bignum   privk;
    assert( pubk.gen_keypair(privk) == true );
    p.public_key(pubk);
    p.private_key(privk);
    p.common_name("Hello");
    p.create();

    peer p2;
    assert( p2.find_by_cn("Hello") == true );
    assert( p2.public_key().has_data() == true );
    assert( p2.private_key().is_empty() == false );

    peer p3;
    assert( p3.find_by_id(p2.id()) == true );
    assert( p3.public_key().has_data() == true );
    assert( p3.private_key().is_empty() == false );
  }

  /** @todo document me */
  void usage4()
  {
    peer p2;
    assert( p2.find_by_cn("Hello") == true );
    assert( p2.public_key().has_data() == true );
    assert( p2.private_key().is_empty() == false );
  }

  /** @todo document me */
  void usage5()
  {
    peer p2;
    assert( p2.find_by_cn("Hello") == true );

    peer p3;
    assert( p3.find_by_pubkey(p2.public_key()) == true );
    assert( p3.public_key().is_empty() == false );
    assert( p3.private_key().has_data() == true );
    assert( p3.public_key().algname() == "prime192v3" );
    assert( p3.common_name() == "Hello" );
    assert( p3.id() == p2.id() );
  }
#endif
} // end of test_schedule

using namespace test_schedule;

int main()
{
  csl_common_print_results( "baseline           ", csl_common_test_timer_v0(baseline),"" );
  /*
  csl_common_print_results( "usage1             ", csl_common_test_timer_v0(usage1),"" );
  csl_common_print_results( "usage2             ", csl_common_test_timer_v0(usage2),"" );
  csl_common_print_results( "usage3             ", csl_common_test_timer_v0(usage3),"" );
  csl_common_print_results( "usage4             ", csl_common_test_timer_v0(usage4),"" );
  csl_common_print_results( "usage5             ", csl_common_test_timer_v0(usage5),"" );
  */
  return 0;
}

/* EOF */
