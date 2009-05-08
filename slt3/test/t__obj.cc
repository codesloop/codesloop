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
   @file t__obj.cc
   @brief Tests to check and measure various slt3::obj features
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

/** @brief contains tests related to slt3::obj */
namespace test_obj {

  /** @todo document me */
  void baseline()
  {
  }

  class DbUser : public obj
  {
    public:
      virtual conn & db() { return reg_.db(); }
      virtual sql::helper & sql_helper() const { return sql_helper_; }

      virtual ~DbUser() {}

      DbUser()
      : id_("id",*this,"PRIMARY KEY ASC AUTOINCREMENT"),
        name_("name",*this),
        height_("height",*this),
        pk_("pk",*this) {}

    private:
      static reg::helper  reg_;
      static sql::helper  sql_helper_;

    public:
      intvar      id_;
      strvar      name_;
      doublevar   height_;
      blobvar     pk_;
  };

  reg::helper DbUser::reg_("test_obj","test_obj.db");
  sql::helper DbUser::sql_helper_("user");

  void usage1()
  {
    DbUser u;
  }

} // end of test_obj

using namespace test_obj;

int main()
{
  usage1();
  csl_common_print_results( "baseline           ", csl_common_test_timer_v0(baseline),"" );
  return 0;
}

/* EOF */
