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
#include "codesloop/common/test_timer.h"
#include "codesloop/common/ustr.hh"
#include "codesloop/common/common.h"
#include <assert.h>

using namespace csl::common;

namespace test_syntax {

  struct X
  {
    X() {}

    struct update_columns
    {
      update_columns(X * x) : x_(x) {}
      X * x_;

      update_columns & set(const char * column, csl::common::ustr & v)
      {
        x_->result_ += v;
        return *this;
      }
    };

    update_columns update(const char * table)
    {
      result_ += "uppdate ";
      result_ += table;
      return update_columns(this);
    }

    ustr result_;
  };

  void test1()
  {
    X x;
    ustr els("oo");
    x.update("hello").set("elso",els);

    fprintf(stderr,"%s",x.result_.c_str());
  }

} // end of test_syntax

using namespace test_syntax;

int main()
{
  test1();
  //csl_common_print_results( "baseline      ", csl_common_test_timer_v0(baseline),"" );
  return 0;
}

// EOF

