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
#include "codesloop/common/int64.hh"
#include "codesloop/common/common.h"
#include <assert.h>

using namespace csl::common;

namespace test_syntax {

  struct X
  {
    X() : n_update_columns_(0), n_where_columns_(0) {}

    struct where_conditions
    {
      where_conditions(X * x) : x_(x) {}
      X * x_;

      template <typename COL, typename ARG>
      where_conditions & gen(const COL & column, const char * op, const ARG & arg)
      {
        ustr & out(x_->result_);
        out << column << " " << op << "'" << arg << "'";
        ++(x_->n_where_columns_);
        return *this;
      }

      template <typename COL, typename ARG>
      where_conditions & condition(const COL & column, const char * op, const ARG & arg)
      {
        ustr & out(x_->result_);
        out << " WHERE ";
        return gen( column, op, arg );
      }

      template <typename COL, typename ARG>
      where_conditions & AND(const COL & column, const char * op, const ARG & arg)
      {
        ustr & out(x_->result_);
        out << " AND ";
        return gen( column, op, arg );
      }

      template <typename COL, typename ARG>
      where_conditions & OR(const COL & column, const char * op, const ARG & arg)
      {
        ustr & out(x_->result_);
        out << " OR ";
        return gen( column, op, arg );
      }
    };

    struct update_columns
    {
      update_columns(X * x) : x_(x) {}
      X * x_;

      template <typename COL, typename ARG>
      update_columns & SET(const COL & column, const ARG & arg)
      {
        ustr & out(x_->result_);

        if( x_->n_update_columns_ > 0 ) out << ",";
        else out << " SET";

        out << " " << column << " = '" << arg << "'";
        ++(x_->n_update_columns_);
        return *this;
      }

      template <typename COL, typename ARG>
      where_conditions & WHERE(const COL & column, const char * op, const ARG & arg)
      {
        where_conditions w(x_);
        return w.condition(column,op,arg);
      }
    };

    update_columns UPDATE(const char * table)
    {
      result_ << "UPDATE " << table;
      return update_columns(this);
    }

    ustr result_;
    uint32_t n_update_columns_;
    uint32_t n_where_columns_;
  };

  void test1()
  {
    X x;
    ustr els("oo");
    int64 i1(1);
    x.UPDATE("hello").SET("elso",els).SET("what",i1).WHERE("what","=",i1).AND("Q","<",els);

    fprintf(stderr,"%s\n",x.result_.c_str());
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

