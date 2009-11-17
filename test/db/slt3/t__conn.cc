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
   @file t__conn.cc
   @brief Tests to check and measure various slt3::conn features
 */

#include "codesloop/common/test_timer.h"
#include "codesloop/db/slt3/conn.hh"
#include "codesloop/db/exc.hh"
#include "codesloop/common/common.h"
#include <assert.h>
#include <stdlib.h>

using namespace csl::db;

/** @brief contains tests related to slt3::conn */
namespace test_conn {

  /** @test open invalid file w/o throwing exception */
  void open_fail_nothrow()
  {
    slt3::conn c;
    c.use_exc(false);
    FPRINTF(stderr,L"Must get an error message here:\n");
    assert( c.open(".") == false );
    assert( c.close() == false );
  }

  /** @test open invalid db and throw exception */
  void open_fail_throw()
  {
    bool thrown = false;
    slt3::conn c;
    c.use_exc(true);

    try
    {
      assert( c.open(".") == false );
      assert( c.close() == false );
    }
    catch( exc e )
    {
      thrown = true;
    }
    assert( thrown == true );
    assert( c.close() == false );
  }

  /** @test open and close db */
  void open_close()
  {
    slt3::conn c;
    assert( c.close() == false );
    assert( c.open("test.db") == true );
    assert( c.close() == true );
    assert( c.close() == false );
  }

  /** @test set and check use_exc */
  void set_exc()
  {
    slt3::conn c;
    c.use_exc(true);
    assert( c.use_exc() == true );
    c.use_exc(false);
    assert( c.use_exc() == false );
  }

  /** @test baseline for performance comparison */
  void baseline()
  {
    slt3::conn c;
  }

} // end of test_conn

using namespace test_conn;

int main()
{
  open_fail_nothrow();
  open_fail_throw();
  open_close();

  UNLINK( "test.db" );
  csl_common_print_results( "baseline      ", csl_common_test_timer_v0(baseline),"" );
  csl_common_print_results( "open_close    ", csl_common_test_timer_v0(open_close),"" );
  csl_common_print_results( "set_exc       ", csl_common_test_timer_v0(set_exc),"" );
  UNLINK( "test.db" );
  return 0;
}

/* EOF */
