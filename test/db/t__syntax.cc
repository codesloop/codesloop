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
   @file t__syntax.cc
   @brief @todo
 */

#include "codesloop/db/exc.hh"
#include "codesloop/db/conn.hh"
#include "codesloop/db/driver.hh"
#include "codesloop/common/test_timer.h"
#include "codesloop/common/ustr.hh"
#include "codesloop/common/int64.hh"
#include "codesloop/common/common.h"
#include <assert.h>

using namespace csl::common;
using namespace csl::db;

namespace test_syntax {

  void conn_syntax_sqlite3()
  {
    conn c( driver::d_sqlite3 );

    /* conn features */
    bool open_ret = c.open("test.db");
    uint64_t liid = c.last_insert_id();
    uint64_t chcn = c.change_count();
    /*void*/ c.reset_change_count();
    bool close_ret = c.close();
  }

} // end of test_syntax

using namespace test_syntax;

int main()
{
  conn_syntax_sqlite3();
  return 0;
}

// EOF
