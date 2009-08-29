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

// TODO : tovabbi tesztek kellenek

/**
   @file t__logger.cc
   @brief Tests to verify logger
 */

#ifndef DEBUG
#define DEBUG
#endif /* DEBUG */

#include "logger.hh"
#include "obj.hh"
#include "exc.hh"
#include "common.h"
#include "str.hh"
#include <assert.h>
#include "test_timer.h"
#include <sys/stat.h>

#define LOG_TEST_MSG L"logger test"

using csl::common::exc;
using csl::common::str;

/**@brief test namespace for logger */
namespace test_logger
{
  /**@brief test class for logging */
  class logtest : public csl::common::obj
  {
      CSL_OBJ( test_logger, logtest );

    public:

      /**@test return value from RETURN_FUNCTION */
      int int_function(const wchar_t * sz)
      {
        ENTER_FUNCTION();

        CSL_DEBUGF( L"parameter: %ls", sz  );

        RETURN_FUNCTION( 0x12345678 );
      }

      /**@test return with exception from THROW_EXCEPTION */
      void exception()
      {
        ENTER_FUNCTION();
        THRR( exc::rs_unknown, LOG_TEST_MSG, );
        LEAVE_FUNCTION();
      }

      static void static_void() 
      {
        ENTER_FUNCTION();
        LEAVE_FUNCTION();
      }
  };

};

int main()
{
  struct stat st;
  unlink( CSL_LOGFILE );

  // assert def values
  assert( csl::common::logger::enable_trace_ == true );

  // check return code from RETURN_FUNCTION 
  test_logger::logtest l;
  l.use_exc(true);
  assert( l.int_function(L"param1") == 0x12345678 );  

  // check created logfile
  assert( stat(CSL_LOGFILE,&st) == 0 );

  // test static file
  unlink( CSL_LOGFILE );
  test_logger::logtest::static_void();
  assert( stat(CSL_LOGFILE,&st) == 0 );


  // check THROW_EXCEPTION
  unlink( CSL_LOGFILE );
  try {
    l.exception();
    assert( "Exception not thrown" == NULL );
  } catch ( exc e ) {
    assert( e.text_ == LOG_TEST_MSG );
  }
  assert( stat(CSL_LOGFILE,&st) == 0 );


  return 0;
}

/* EOF */
