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

#ifndef _csl_db_dummy_driver_hh_included_
#define _csl_db_dummy_driver_hh_included_

#include "codesloop/db/driver.hh"
#include "codesloop/common/obj.hh"
#include "codesloop/common/ustr.hh"
#include "codesloop/common/inpvec.hh"

#ifdef __cplusplus

namespace csl
{
  namespace db
  {
    namespace dummy
    {
      namespace syntax
      {
        class generator;

        class insert_column : public csl::db::syntax::insert_column
        {
          public:
            /* interface */
            void table_name(const char * table);
            csl::db::syntax::insert_column & VAL(const char * column_name,
                                                const var & value);
            void DO();

            /* internals */
            insert_column(csl::db::dummy::syntax::generator & g);
            virtual ~insert_column() {}

          private:
            /* no default construction */
            insert_column();

            common::ustr table_name_;
            csl::db::dummy::syntax::generator * generator_;
        };

        class update_column : public csl::db::syntax::update_column
        {
          public:
        };

        class where_condition : public csl::db::syntax::where_condition
        {
          public:
        };

        class generator : public csl::db::syntax::generator
        {
          public:
            /* interface */
            csl::db::syntax::insert_column & INSERT_INTO(const char * table);
            void DO();

            /* internals */
            generator(csl::db::driver & d);
            virtual ~generator() {}

          private:
            /* no default construction */
            generator();

            insert_column insert_column_;
        };
      }

      class driver : public csl::db::driver
      {
        public:
          //
          static driver * instance();
          csl::db::syntax::generator * generator(csl::db::driver & d);

          // connection related
          bool open(const ustr & connect_string);
          bool close();

          // transactions
          bool begin(ustr & id);
          bool commit(const ustr & id);
          bool rollback(const ustr & id);

          // subtransactions
          bool savepoint(ustr & id, const ustr & parent_id);
          bool release_savepoint(const ustr & id, const ustr & parent_id);
          bool rollback_savepoint(const ustr & id, const ustr & parent_id);

          // infos
          uint64_t last_insert_id();
          uint64_t change_count();
          void reset_change_count();

          // construction / destruction
          virtual ~driver();
          driver();

          CSL_OBJ(csl::db::dummy,driver);
      };
    } // end of ns:csl::db::dummy
  } // end of ns:csl::db
} // end of ns:csl

#endif // __cplusplus
#endif // _csl_db_dummy_driver_hh_included_
