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

// #if 0
#ifndef DEBUG
#define DEBUG
#define DEBUG_ENABLE_INDENT
#endif /* DEBUG */
// #endif

#include "codesloop/db/driver.hh"
#include "codesloop/db/dummy/driver.hh"
#include "codesloop/db/slt3/driver.hh"
#include "codesloop/db/mysql/driver.hh"
#include "codesloop/common/logger.hh"

namespace csl
{
  namespace db
  {
    namespace syntax
    {
      void insert_column::table_name(const char * table)
      {
        ENTER_FUNCTION();
        CSL_DEBUGF(L"table_name('%s')",table);
        table_name_ = table;
        LEAVE_FUNCTION();
      }

      const char * insert_column::table_name()
      {
        ENTER_FUNCTION();
        const char * ret = table_name_.c_str();
        CSL_DEBUGF(L"table_name() => '%s'",ret);
        RETURN_FUNCTION(ret);
      }

      csl::db::syntax::insert_column & insert_column::VAL(const char * column_name,
                                                          const var & value)
      {
        ENTER_FUNCTION();
#ifdef DEBUG
        ustr tmp; tmp << value;
        CSL_DEBUGF(L"VAL(%s,'%s') [%lld]",column_name,tmp.c_str(),items_.n_items()+1);
#endif /*DEBUG*/
        {
          item i;
          i.column_ = column_name;
          i.arg_    = &value;
          items().push_back(i);
        }
        RETURN_FUNCTION((*this));
      }

      csl::db::syntax::insert_column & generator::INSERT_INTO(const char * table)
      {
        ENTER_FUNCTION();
        CSL_DEBUGF(L"INSERT_INTO(%s)",table);
        insert_column_ref().table_name(table);
        statement_.reset(0);
        RETURN_FUNCTION(insert_column_ref());
      }

      csl::db::syntax::select_query & generator::SELECT()
      {
        ENTER_FUNCTION();
        RETURN_FUNCTION(select_query_ref());
      }
    }

    TABLE::TABLE(const char * name) : name_(name)
    {
      ENTER_FUNCTION();
      CSL_DEBUGF(L"TABLE(name:'%s')",name);
      LEAVE_FUNCTION();
    }

    TABLE & TABLE::FIELD(const char * name, var & v)
    {
      ENTER_FUNCTION();
#ifdef DEBUG
      ustr s; s.from_var(v);
      CSL_DEBUGF(L"FIELD(name:'%s', var:'%s')",name,s.c_str());
#endif /*DEBUG*/
      field f;
      f.name_ = name;
      f.var_  = &v;
      fields_.push_back(f);
      RETURN_FUNCTION( *this );
    }

    driver * driver::instance(int driver_type)
    {
      switch( driver_type )
      {
        case d_dummy:
          return csl::db::dummy::driver::instance();

        case d_sqlite3:
          return csl::db::slt3::driver::instance();

        case d_mysql:
          return csl::db::mysql::driver::instance();
      };
      return 0;
    }

  }; // end of ns:csl::db
}; // end of ns:csl

/* EOF */
