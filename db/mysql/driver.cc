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

// #if 0
#ifndef DEBUG
#define DEBUG
#define DEBUG_ENABLE_INDENT
#endif /* DEBUG */
// #endif

#include "codesloop/common/logger.hh"
#include "codesloop/common/common.h"
#include "codesloop/db/mysql/driver.hh"

namespace csl
{
  namespace db
  {
    namespace mysql
    {
      namespace syntax
      {
        // ==============================================================
        // == interface =================================================
        // ==============================================================
        void insert_column::table_name(const char * table)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"table_name(%s)",table);
          table_name_ = table;
          LEAVE_FUNCTION();
        }

        const char * insert_column::table_name()
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"table_name() => %s",table_name_.c_str());
          RETURN_FUNCTION(table_name_.c_str());
        }
        // ==============================================================
        // == insert query ==============================================
        // ==============================================================
        csl::db::syntax::insert_column & generator::INSERT_INTO(const char * table)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"INSERT_INTO(%s)",table);
          insert_column_.table_name(table);
          RETURN_FUNCTION(insert_column_);
        }

        csl::db::syntax::insert_column & insert_column::VAL(const char * column_name,
                                                            const var & value)
        {
          ENTER_FUNCTION();
          ustr tmp; tmp << value;
          CSL_DEBUGF(L"VAL(%s,'%s')",column_name,tmp.c_str());
          RETURN_FUNCTION((*this));
        }

        // ==============================================================
        // = others =====================================================
        // ==============================================================
        bool generator::GO()
        {
          ENTER_FUNCTION();
          RETURN_FUNCTION(true);
        }

        bool insert_column::GO()
        {
          ENTER_FUNCTION();
          RETURN_FUNCTION(generator_->GO());
        }

        // ==============================================================

        /* internals */
        insert_column::insert_column() : csl::db::syntax::insert_column()
        {
          ENTER_FUNCTION();
          LEAVE_FUNCTION();
        }

        insert_column::insert_column(csl::db::mysql::syntax::generator & g) :
            csl::db::syntax::insert_column(),
            generator_(&g)
        {
          ENTER_FUNCTION();
          LEAVE_FUNCTION();
        }

        generator::generator() :
            csl::db::syntax::generator(*(new csl::db::mysql::driver())),
            insert_column_(*this)
        {
          ENTER_FUNCTION();
          throw "should never be called";
          LEAVE_FUNCTION();
        }

        generator::generator(csl::db::driver & d) :
            csl::db::syntax::generator(d),
            insert_column_(*this)
        {
          ENTER_FUNCTION();
          LEAVE_FUNCTION();
        }
      } /* end of syntax ns */

      statement::statement(csl::db::driver & d, const ustr & q) :
        csl::db::statement(d,q)
      {
        ENTER_FUNCTION();
        LEAVE_FUNCTION();
      }

      statement::statement() :
        csl::db::statement(*(new csl::db::mysql::driver()),*(new ustr()))
      {
        ENTER_FUNCTION();
        throw "should never be called";
        LEAVE_FUNCTION();
      }

      bool statement::bind(uint64_t which, const ustr & column, const var & value)
      {
        ENTER_FUNCTION();
        ustr tmp; tmp << value;
        CSL_DEBUGF(L"bind(which:%lld,column:%s,value:'%s')",which,column.c_str(),tmp.c_str());
        RETURN_FUNCTION(true);
      }

      bool statement::execute()
      {
        ENTER_FUNCTION();
        RETURN_FUNCTION(true);
      }

      csl::db::statement * driver::prepare(const ustr & q)
      {
        ENTER_FUNCTION();
        CSL_DEBUGF(L"prepare(%s)",q.c_str());
        RETURN_FUNCTION( (new csl::db::mysql::statement(*this,q)) );
      }

      /* static */ driver * driver::instance()
      {
        ENTER_FUNCTION();
        RETURN_FUNCTION((new driver()));
      }

      csl::db::syntax::generator * driver::generator(csl::db::driver & d)
      {
        ENTER_FUNCTION();
        RETURN_FUNCTION((new csl::db::mysql::syntax::generator(d)));
      }

      // connection related
      bool driver::open(const csl::db::driver::connect_desc & info)
      {
        ENTER_FUNCTION();
        CSL_DEBUGF(L"open(info[host:%s port:%lld db_name:%s user:%s password:%s])",
                    info.host_.c_str(),
                    info.port_.value(),
                    info.db_name_.c_str(),
                    info.user_.c_str(),
                    info.password_.c_str() );
        RETURN_FUNCTION(false);
      }

      bool driver::open(const ustr & connect_string)
      {
        ENTER_FUNCTION();
        CSL_DEBUGF(L"open(%s)",connect_string.c_str());
        RETURN_FUNCTION(false);
      }

      bool driver::close()
      {
        ENTER_FUNCTION();
        RETURN_FUNCTION(false);
      }

      // transactions
      bool driver::begin(ustr & id)
      {
        ENTER_FUNCTION();
        id="mysql-transaction";
        RETURN_FUNCTION(false);
      }

      bool driver::commit(const ustr & id)
      {
        ENTER_FUNCTION();
        CSL_DEBUGF(L"commit(%s)",id.c_str());
        RETURN_FUNCTION(false);
      }

      bool driver::rollback(const ustr & id)
      {
        ENTER_FUNCTION();
        CSL_DEBUGF(L"rollback(%s)",id.c_str());
        RETURN_FUNCTION(false);
      }

      // subtransactions
      bool driver::savepoint(ustr & id, const ustr & parent_id)
      {
        ENTER_FUNCTION();
        id="mysql-savepoint";
        CSL_DEBUGF(L"savepoint(%s,%s)",id.c_str(),parent_id.c_str());
        RETURN_FUNCTION(false);
      }

      bool driver::release_savepoint(const ustr & id, const ustr & parent_id)
      {
        ENTER_FUNCTION();
        CSL_DEBUGF(L"release_savepoint(%s,%s)",id.c_str(),parent_id.c_str());
        RETURN_FUNCTION(false);
      }

      bool driver::rollback_savepoint(const ustr & id, const ustr & parent_id)
      {
        ENTER_FUNCTION();
        CSL_DEBUGF(L"rollback_savepoint(%s,%s)",id.c_str(),parent_id.c_str());
        RETURN_FUNCTION(false);
      }

      // infos
      uint64_t driver::last_insert_id()
      {
        ENTER_FUNCTION();
        uint64_t id=0;
        CSL_DEBUGF(L"last_insert_id() => %lld",id);
        RETURN_FUNCTION(0);
      }

      uint64_t driver::change_count()
      {
        ENTER_FUNCTION();
        uint64_t cn=0;
        CSL_DEBUGF(L"change_count() => %lld",cn);
        RETURN_FUNCTION(0);
      }

      void driver::reset_change_count()
      {
        ENTER_FUNCTION();
        LEAVE_FUNCTION();
      }

      driver::driver()
      {
        ENTER_FUNCTION();
        LEAVE_FUNCTION();
      }

      driver::~driver()
      {
        ENTER_FUNCTION();
        LEAVE_FUNCTION();
      }
    } // end of ns:csl::db::mysql
  } // end of ns:csl::db
} // end of ns:csl

/* EOF */
