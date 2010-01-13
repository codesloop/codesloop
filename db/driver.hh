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

#ifndef _csl_db_driver_hh_included_
#define _csl_db_driver_hh_included_

#include "codesloop/common/ustr.hh"
#include "codesloop/common/obj.hh"
#include "codesloop/common/inpvec.hh"
#include "codesloop/common/common.h"
#ifdef __cplusplus

namespace csl
{
  using common::ustr;
  using common::inpvec;
  using common::var;

  namespace db
  {
    class driver;

    namespace syntax
    {
      class insert_column
      {
        public:
          // interface
          virtual void table_name(const char * table) = 0;
          virtual const char * table_name() = 0;
          virtual insert_column & VAL(const char * column_name, const var & value) = 0;
          virtual void DO() = 0;

          // internals
          insert_column() {}
          virtual ~insert_column() {}
      };

      class update_column
      {
        public:
      };

      class where_condition
      {
        public:
      };

      class generator
      {
        public:
          // interface
          virtual insert_column & INSERT_INTO(const char * table) = 0;
          virtual void DO() = 0;

          // internals
          generator(driver & d) : driver_(&d) {}
          virtual ~generator() {}
          driver & get_driver() { return *driver_; }

        private:
          // no default construction
          generator() {}
          driver * driver_;
      };
    }

    class statement
    {
      public:
        // interface
        virtual bool bind(uint32_t which, const ustr & column, const var & value) = 0;
        virtual bool execute() = 0;

        // internals
        statement(driver & d, const char * q) : driver_(&d), query_(q) { }
        virtual ~statement() {}
        driver & get_driver() { return *driver_; }
        ustr & get_query()    { return query_;   }

      private:
        // no default construction
        statement() {}
        driver * driver_;
        ustr     query_;
    };

    class driver
    {
      public:
        // allocate driver instance
        enum {
          d_unknown,  // unknown DB driver
          d_dummy,    // for debugging purposes
          d_sqlite3,  // SQLite3 driver
          d_mysql,    // MySQL driver
        };

        static driver * instance(int driver_type);

        virtual syntax::generator * generator(driver & d) = 0;

        enum {
          t_null    = CSL_TYPE_NULL,      ///<Null column
          t_integer = CSL_TYPE_INT64,     ///<64 bit integer column
          t_double  = CSL_TYPE_DOUBLE,    ///<standard 8 byte double precision column
          t_string  = CSL_TYPE_USTR,      ///<string column
          t_blob    = CSL_TYPE_BIN        ///<blob column
        };

        // connection related
        virtual bool open(const ustr & connect_string) = 0;
        virtual bool close() = 0;

        // transactions
        virtual bool begin(ustr & id) = 0;
        virtual bool commit(const ustr & id) = 0;
        virtual bool rollback(const ustr & id) = 0;

        // subtransactions
        virtual bool savepoint(ustr & id, const ustr & parent_id) = 0;
        virtual bool release_savepoint(const ustr & id, const ustr & parent_id) = 0;
        virtual bool rollback_savepoint(const ustr & id, const ustr & parent_id) = 0;

        // infos
        virtual uint64_t last_insert_id() = 0;
        virtual uint64_t change_count() = 0;
        virtual void reset_change_count() = 0;

        // prepare statement
        virtual statement * prepare(const char * q) = 0;

        // internals
        virtual ~driver() {}
    };
  }; // end of ns:csl::db
}; // end of ns:csl

#endif // __cplusplus
#endif // _csl_db_driver_hh_included_
