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
   @file t__syntax_cxx_sql.cc
   @brief @todo
 */

#include "codesloop/db/exc.hh"
#include "codesloop/common/test_timer.h"
#include "codesloop/common/inpvec.hh"
#include "codesloop/common/ustr.hh"
#include "codesloop/common/int64.hh"
#include "codesloop/common/common.h"
#include <assert.h>

using namespace csl::common;

namespace test_syntax_cxx_sql {

  // this is a test place to try CPlusPlus SQL syntax ideas
  // the result of these experiments will most likely go to the
  // query class

  // to be supported:
  // ----------------
  // SELECT a, b, c, d FROM x WHERE ( a=5 OR b=? AND q='r' ) OR z=?
  //   .. join .. in TABLES ..
  //   .. functions .. in WHERE, CONVERSIONS ..
  //   .. step, next .. in RESULTSETS ..
  // DELETE FROM x WHERE ( a=5 OR b=? AND q='r' ) OR z=?
  // UPDATE x SET a=?, b=5, c='w' WHERE ( a=5 OR b=? AND q='r' ) OR z=?
  // INSERT INTO x (col1, col2, col3,...) VALUES (x,y,?,?)

  struct X
  {
    X() {}

    struct where_conditions
    {
      where_conditions(X * x) : x_(x) {}
      X *           x_;

      struct item
      {
        ustr          prefix_;
        ustr          column_;
        ustr          op_;
        const var *   arg_;
      };

      template <typename COL, typename ARG>
      where_conditions gen(const char * prefix, const COL & column, const char * op, const ARG & arg)
      {
        item i;
        i.prefix_ = prefix;
        i.column_ = column;
        i.op_     = op;
        i.arg_    = &arg;
        x_->where_conditions_.push_back( i );
        return *this;
      }

      template <typename COL, typename ARG>
      where_conditions condition(const COL & column, const char * op, const ARG & arg)
      {
        return gen( " WHERE ", column, op, arg );
      }

      template <typename COL, typename ARG>
      where_conditions AND(const COL & column, const char * op, const ARG & arg)
      {
        return gen( " AND ", column, op, arg );
      }

      template <typename COL, typename ARG>
      where_conditions OR(const COL & column, const char * op, const ARG & arg)
      {
        return gen( " OR ", column, op, arg );
      }
    };

    struct update_columns
    {
      update_columns(X * x) : x_(x) {}
      X * x_;

      struct item
      {
        ustr          column_;
        const var *   arg_;
      };

      template <typename COL, typename ARG>
      update_columns SET(const COL & column, const ARG & arg)
      {
        item i;
        i.column_ = column;
        i.arg_    = &arg;
        x_->update_columns_.push_back( i );
        return *this;
      }

      template <typename COL, typename ARG>
      where_conditions WHERE(const COL & column, const char * op, const ARG & arg)
      {
        where_conditions w(x_);
        return w.condition(column,op,arg);
      }
    };

    update_columns UPDATE(const char * table)
    {
      reset();
      result_ << "UPDATE " << table;
      return update_columns(this);
    }

    struct insert_columns
    {
      insert_columns(X * x) : x_(x) {}
      X * x_;

      struct item
      {
        ustr          column_;
        const var *   arg_;
      };

      template <typename ARG>
      insert_columns VAL(const char * column, const ARG & arg)
      {
        item i;
        i.column_ = column;
        i.arg_    = &arg;
        x_->insert_columns_.push_back( i );
        return *this;
      }
    };

    insert_columns INSERT_INTO(const char * table)
    {
      reset();
      result_ << "INSERT INTO " << table;
      return insert_columns(this);
    }

    ustr & result()
    {
      if( update_columns_.n_items() > 0 )
      {
        // update query
        update_columns_t::iterator it = update_columns_.begin();
        update_columns::item * i = 0;

        assert( it.is_empty() == false );
        i = *it;

        result_ << " SET " << i->column_ << "='" << *(i->arg_) << "' ";

        while( (i=it.next_used()) != 0 )
        {
          result_ << ", " << i->column_ << "='" << *(i->arg_) << "' ";
        }

        // add where conditions if there is any
        if( where_conditions_.n_items() > 0 )
        {
          where_conditions_t::iterator itw = where_conditions_.begin();
          where_conditions::item * iw = 0;

          assert( itw.is_empty() == false );
          iw = *itw;

          result_ << iw->prefix_ << iw->column_ << iw->op_ << "'" << *(iw->arg_) << "' ";

          while( (iw=itw.next_used()) != 0 )
          {
            result_ << iw->prefix_ << iw->column_ << iw->op_ << "'" << *(iw->arg_) << "' ";
          }
        }

        result_ << ";";
      }
      else if( insert_columns_.n_items() > 0 )
      {
        // insert query
        insert_columns_t::iterator it = insert_columns_.begin();
        insert_columns::item * i = 0;

        assert( it.is_empty() == false );
        i = *it;

        result_ << " ( " << i->column_;

        while( (i=it.next_used()) != 0 )
        {
          result_ << ", " << i->column_;
        }

        result_ << " ) VALUES ( ";

        it = insert_columns_.begin();
        i = *it;

        result_ << "'" << *(i->arg_) << "'";

        while( (i=it.next_used()) != 0 )
        {
          result_ << ", '" << *(i->arg_) << "'";
        }

        result_ << " );";
      }
      return result_;
    }

    ustr result_;

    typedef inpvec<where_conditions::item>   where_conditions_t;
    typedef inpvec<update_columns::item>     update_columns_t;
    typedef inpvec<insert_columns::item>     insert_columns_t;

    where_conditions_t   where_conditions_;
    update_columns_t     update_columns_;
    insert_columns_t     insert_columns_;

    void reset()
    {
      result_.reset();
      where_conditions_.reset();
      update_columns_.reset();
      insert_columns_.reset();
    }
  };

  void test1()
  {
    X x;
    ustr els("oo");
    int64 i1(1);

    x.UPDATE("hello").SET("elso",els).SET("what",i1).WHERE("what","=",i1).AND("Q","<",els);
    // fprintf(stderr,"%s\n",x.result().c_str());

    x.INSERT_INTO("hello").VAL("elso",els).VAL("masodik",i1);
    // fprintf(stderr,"%s\n",x.result().c_str());
  }

} // end of test_syntax_cxx_sql

using namespace test_syntax_cxx_sql;

int main()
{
  test1();
  csl_common_print_results( "test1         ", csl_common_test_timer_v0(test1),"" );
  //csl_common_print_results( "baseline      ", csl_common_test_timer_v0(baseline),"" );
  return 0;
}

// EOF

