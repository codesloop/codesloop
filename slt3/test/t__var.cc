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
   @file t__var.cc
   @brief Tests to check and measure various slt3::var features
 */

#include "test_timer.h"
#include "var.hh"
#include "obj.hh"
#include "sql.hh"
#include "reg.hh"
#include "conn.hh"
#include "exc.hh"
#include "common.h"
#include "mpool.hh"
#include <assert.h>
#include <stdlib.h>

using namespace csl::slt3;

/** @brief contains tests related to slt3::var */
namespace test_var {

  /** @todo document me */
  class TestBase : public obj
  {
    public:
      /* helper to lookup the database */
      virtual conn & db() { return reg_.db(); }
      static reg::helper  reg_;
  };

  reg::helper TestBase::reg_("test_var","test_var.db");

  /** @todo document me */
  class SingleInt : public TestBase
  {
    public:
      virtual sql::helper & sql_helper() const { return sql_helper_; }
      static sql::helper  sql_helper_;

      SingleInt() : id_("id",*this,"PRIMARY KEY ASC AUTOINCREMENT"), 
                    value_("intval",*this) {}

      intvar id_;
      intvar value_;
  };

  sql::helper SingleInt::sql_helper_("single_int");

  /** @todo document me */
  class SingleString : public TestBase
  {
    public:
      virtual sql::helper & sql_helper() const { return sql_helper_; }
      static sql::helper  sql_helper_;

      SingleString() : id_("id",*this,"PRIMARY KEY ASC AUTOINCREMENT"),
                       value_("strval",*this) {}

      intvar id_;
      strvar value_;
  };

  sql::helper SingleString::sql_helper_("single_string");

  /** @todo document me */
  class SingleDouble : public TestBase
  {
    public:
      virtual sql::helper & sql_helper() const { return sql_helper_; }
      static sql::helper    sql_helper_;

      SingleDouble() : id_("id",*this,"PRIMARY KEY ASC AUTOINCREMENT"),
                       value_("dblval",*this) {}

      intvar     id_;
      doublevar  value_;
  };

  sql::helper SingleDouble::sql_helper_("single_dbl");

  /** @todo document me */
  class SingleBlob : public TestBase
  {
    public:
      virtual sql::helper & sql_helper() const { return sql_helper_; }
      static sql::helper    sql_helper_;

      SingleBlob() : id_("id",*this,"PRIMARY KEY ASC AUTOINCREMENT"),
                     value_("blobval",*this) {}

      intvar     id_;
      blobvar  value_;
  };

  sql::helper SingleBlob::sql_helper_("single_blob");

  /** @todo document me */
  void baseline()
  {
  }

  /** @todo document me */
  void single_int0()
  {
    SingleInt i0;
  }

  /** @todo document me */
  void single_int1()
  {
    SingleInt i0;
    assert( i0.init() == true );
  }

  /** @todo document me */
  void single_int2()
  {
    SingleInt i0;
    tran t(i0.db());
    // assume single_int1() already initialized it...
    // assert( i0.init(t) == true );
    assert( i0.create(t) == true );
    assert( i0.remove(t) == true );
  }

  /** @todo document me */
  void single_int3()
  {
    SingleInt i0;
    {
      tran t(i0.db());
      // assume single_int1() already initialized it...
      // assert( i0.init(t) == true );
      i0.value_ = 123987;
      assert( i0.create(t) == true );
      assert( i0.find_by_id(t) == true );
    }

    SingleInt i1;
    {
      tran t(i0.db());
      i1.value_ = i0.value_;
      assert( i1.find_by(t,1) == true );
      assert( i0.id_.get() == i1.id_.get() );
      assert( i0.remove(t) == true );
    }
  }

  /** @todo document me */
  void single_str0()
  {
    SingleString s0;
  }

  /** @todo document me */
  void single_str1()
  {
    SingleString s0;
    assert( s0.init() == true );
  }

  /** @todo document me */
  void single_str2()
  {
    SingleString s0;
    tran t(s0.db());
    assert( s0.create(t) == true );
    assert( s0.remove(t) == true );
  }

  /** @todo document me */
  void single_str3()
  {
    SingleString s0;
    {
      tran t(s0.db());
      s0.value_ = "Hello World";
      assert( s0.create(t) == true );
      assert( s0.find_by_id(t) == true );
    }

    SingleString s1;
    {
      tran t(s0.db());
      s1.value_ = s0.value_;
      assert( s1.find_by(t,1) == true );
      assert( s0.id_.get() == s1.id_.get() );
      assert( s0.remove(t) == true );
    }
  }

  /** @todo document me */
  void single_dbl0()
  {
    SingleDouble v0;
  }

  /** @todo document me */
  void single_dbl1()
  {
    SingleDouble v0;
    assert( v0.init() == true );
  }

  /** @todo document me */
  void single_dbl2()
  {
    SingleDouble v0;
    tran t(v0.db());
    assert( v0.create(t) == true );
    assert( v0.remove(t) == true );
  }

  /** @todo document me */
  void single_dbl3()
  {
    SingleDouble v0;
    {
      tran t(v0.db());
      v0.value_ = 3.14;
      assert( v0.create(t) == true );
      assert( v0.find_by_id(t) == true );
    }

    SingleDouble v1;
    {
      tran t(v0.db());
      v1.value_ = v0.value_;
      assert( v1.find_by(t,1) == true );
      assert( v0.id_.get() == v1.id_.get() );
      assert( v0.remove(t) == true );
    }
  }

  /** @todo document me */
  void single_blb0()
  {
    SingleBlob v0;
  }

  /** @todo document me */
  void single_blb1()
  {
    SingleBlob v0;
    assert( v0.init() == true );
  }

  /** @todo document me */
  void single_blb2()
  {
    SingleBlob v0;
    tran t(v0.db());
    assert( v0.create(t) == true );
    assert( v0.remove(t) == true );
  }

  /** @todo document me */
  void single_blb3()
  {
    std::vector<unsigned char> ve;
    for(int i=0;i<256;++i ) ve.push_back(i);

    SingleBlob v0;
    {
      tran t(v0.db());
      v0.value_ = ve;
      assert( v0.create(t) == true );
      assert( v0.find_by_id(t) == true );
    }

    SingleBlob v1;
    {
      tran t(v0.db());
      v1.value_ = v0.value_;
      assert( v1.find_by(t,1) == true );
      assert( v0.id_.get() == v1.id_.get() );
      assert( v0.remove(t) == true );
    }
  }
} // end of test_var

using namespace test_var;

int main()
{
  csl_common_print_results( "baseline           ", csl_common_test_timer_v0(baseline),"" );
  csl_common_print_results( "single_int0        ", csl_common_test_timer_v0(single_int0),"" );
  csl_common_print_results( "single_int1        ", csl_common_test_timer_v0(single_int1),"" );
  csl_common_print_results( "single_int2        ", csl_common_test_timer_v0(single_int2),"" );
  csl_common_print_results( "single_int3        ", csl_common_test_timer_v0(single_int3),"" );

  csl_common_print_results( "single_str0        ", csl_common_test_timer_v0(single_str0),"" );
  csl_common_print_results( "single_str1        ", csl_common_test_timer_v0(single_str1),"" );
  csl_common_print_results( "single_str2        ", csl_common_test_timer_v0(single_str2),"" );
  csl_common_print_results( "single_str3        ", csl_common_test_timer_v0(single_str3),"" );

  csl_common_print_results( "single_dbl0        ", csl_common_test_timer_v0(single_dbl0),"" );
  csl_common_print_results( "single_dbl1        ", csl_common_test_timer_v0(single_dbl1),"" );
  csl_common_print_results( "single_dbl2        ", csl_common_test_timer_v0(single_dbl2),"" );
  csl_common_print_results( "single_dbl3        ", csl_common_test_timer_v0(single_dbl3),"" );

  csl_common_print_results( "single_blb0        ", csl_common_test_timer_v0(single_blb0),"" );
  csl_common_print_results( "single_blb1        ", csl_common_test_timer_v0(single_blb1),"" );
  csl_common_print_results( "single_blb2        ", csl_common_test_timer_v0(single_blb2),"" );
  csl_common_print_results( "single_blb3        ", csl_common_test_timer_v0(single_blb3),"" );

  return 0;
}

/* EOF */
