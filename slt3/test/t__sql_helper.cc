/*
Copyright (c) 2008,2009, David Beck, Tamas Foldi

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
   @file t__sql_helper.cc
   @brief Tests to check and measure various slt3::sql::helper features
 */

#include "test_timer.h"
#include "sql.hh"
#include "obj.hh"
#include "reg.hh"
#include "tran.hh"
#include "common.h"
#include "mpool.hh"
#include <assert.h>
#include <stdlib.h>

using namespace csl::slt3;

/** @brief contains tests related to slt3::sql::helper */
namespace test_sql_helper {

  /** @test baseline for comparison */
  void baseline()
  {
    sql::helper h("xtable");
    (void)h.init_sql();
  }

  /** tester class for basic ORM functionality */
  class X : public obj
  {
    public:
      virtual ~X() {}
      X() : id_("id",*this,"PRIMARY KEY ASC AUTOINCREMENT"), name_("name",*this,"NOT NULL"),
        height_("height",*this,"DEFAULT (0.1)"), pk_("pk",*this) {}

      virtual conn & db() { return reg_.db(); }
      virtual sql::helper & sql_helper() const { return sql_helper_; }

      static sql::helper  sql_helper_;
      static reg::helper  reg_;

      intvar      id_;
      strvar      name_;
      doublevar   height_;
      blobvar     pk_;
  };

  sql::helper X::sql_helper_("Xtable");
  reg::helper X::reg_("test_mapper","test_mapper.db");

  /** @test calls init_sql() */
  void usage1()
  {
    X x;
    (void)x.sql_helper().init_sql();
  }

  /** @test verifies a more complex ORM scenario */
  void usage2()
  {
    X x,x2,x3;

    conn & db(x.db());
    {
      tran t(db);
      x.init(t);
      assert( x.create(t) == true );
      x.height_ = 3.14;
      assert( x.save(t) == true );

      x2.id_ = 100000000;
      assert( x2.find_by_id(t) == false );
      assert( x2.height_.get() != 3.14 );
      assert( x.find_by_id(t) == true );

      x2.id_ = x.id_.get();
      assert( x2.find_by_id(t) == true );
      assert( x2.height_.get() == 3.14 );
      assert( x2.remove(t) == true );
      assert( x.remove(t) == false );

      assert( x2.create(t) == true );
      x.height_ = 3.14;
      assert( x2.save(t) == true );

      x3.height_ = 3.14333;
      assert( x3.find_by(t,2) == false );
      x3.height_ = 3.14;
      assert( x3.find_by(t,2) == true );
    }
  }

  /** @test verify create() and remove() object to/from database */
  void crdelete()
  {
    X x;

    conn & db(x.db());
    {
      tran t(db);
      t.rollback_on_destruct(true);
      t.commit_on_destruct(false);
      assert( x.create(t) == true );
      assert( x.remove(t) == true );
    }
  }

} // end of test_sql_helper

using namespace test_sql_helper;

int main()
{
  csl_common_print_results( "baseline           ", csl_common_test_timer_v0(baseline),"" );
  csl_common_print_results( "usage1             ", csl_common_test_timer_v0(usage1),"" );
  csl_common_print_results( "usage2             ", csl_common_test_timer_v0(usage2),"" );
  csl_common_print_results( "crdelete           ", csl_common_test_timer_v0(crdelete),"" );
  return 0;
}

/* EOF */
