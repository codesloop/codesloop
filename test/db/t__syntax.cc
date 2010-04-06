/*
Copyright (c) 2008,2009,2010, CodeSLoop Team

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
#include "codesloop/db/tran.hh"
#include "codesloop/db/query.hh"
#include "codesloop/db/driver.hh"
#include "codesloop/common/test_timer.h"
#include "codesloop/common/ustr.hh"
#include "codesloop/common/int64.hh"
#include "codesloop/common/common.h"
#include <assert.h>

using namespace csl::common;
using namespace csl::db;

namespace test_syntax {

  void conn_syntax_dummy()
  {
    csl::db::conn c( driver::d_dummy );

    /* conn features */
    bool open_ret = c.open("test.db");
    uint64_t liid = c.last_insert_id();
    uint64_t chcn = c.change_count();
    /*void*/ c.reset_change_count();
    bool close_ret = c.close();
  }

  void tran_syntax_dummy()
  {
    conn c( driver::d_dummy );
    bool open_ret = c.open("test.db");

    /* tran features */
    {
      csl::db::tran t(c);
      t.commit_on_destruct();
      t.commit_on_destruct(false);
      t.rollback_on_destruct();
      t.rollback_on_destruct(true);
      t.commit();
      t.rollback();
    }

    /* subtransactions */
    {
      csl::db::tran t0(c);
      csl::db::tran t1(t0);
    }
  }

  void insert_syntax_dummy()
  {
    conn c( driver::d_dummy );
    bool open_ret = c.open("test.db");

    tran t(c);

    ustr first("1st");
    ustr second("2nd");

    {
      /* first INSERT usage scenario */
      csl::db::query q(t);
      q.INSERT_INTO("hello_table").VAL("f1",first).VAL("f2",second).GO();
    }

    {
      /* second INSERT usage scenario */
      csl::db::query q(t);
      syntax::insert_column & ic(q.INSERT_INTO("hello_table").VAL("f1",first).VAL("f2",second));
      ic.GO();

      first  = "1st-2";
      second = "2nd-2";
      ic.GO();
    }
  }

  void select_syntax_dummy()
  {
    conn c( driver::d_dummy );
    bool open_ret = c.open("test.db");

    tran t(c);

    {
      /**/
      ustr r1,r2,r3;
      query q(t);
      q.SELECT().FROM(TABLE("T1").FIELD("F1",r1).FIELD("F2",r2)).
                 FROM(TABLE("T2").FIELD("G1",r1).FIELD("G2",r2)).GO();
    }
  }

} // end of test_syntax

using namespace test_syntax;

int main()
{
  conn_syntax_dummy();
  tran_syntax_dummy();
  insert_syntax_dummy();
  select_syntax_dummy();
  return 0;
}

// EOF
