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

#if 0
#ifndef DEBUG
#define DEBUG
#define DEBUG_ENABLE_INDENT
#endif /* DEBUG */
#endif

#include "codesloop/common/logger.hh"
#include "codesloop/common/str.hh"
#include "codesloop/common/common.h"
#include "codesloop/db/mysql/driver.hh"
#include "codesloop/db/exc.hh"

using csl::common::str;

namespace csl
{
  namespace db
  {
    namespace mysql
    {
      namespace syntax
      {
        // ==============================================================
        // = others =====================================================
        // ==============================================================
        bool generator::GO()
        {
          ENTER_FUNCTION();
          if(statement_ptr().get() == 0)
          {
            // calculate statement
            if( insert_column_.items().n_items() > 0 )
            {
              common::ustr query("INSERT INTO "); query << insert_column_.table_name() << " ( ";
              insert_column::items_t::iterator it = insert_column_.items().begin();
              insert_column::item * i = *it;
              query << i->column_;
              common::ustr bound("?");

              while( (i=it.next_used()) != 0 )
              {
                query << ", " << i->column_;
                bound << ", ?";
              }

              query << " ) VALUES ( " << bound << " )";

              // prepare statement
              statement_ptr().reset(get_driver().prepare(query));
            }
          }

          // bind variables
          if( insert_column_.items().n_items() > 0 )
          {
            insert_column::items_t::iterator it = insert_column_.items().begin();
            insert_column::item * i = *it;

            statement_ptr()->const_bind( 1ULL, i->column_, *(i->arg_) );

            while( (i=it.next_used()) != 0 )
            {
              statement_ptr()->const_bind( it.get_pos()+1, i->column_, *(i->arg_) );
            }
          }

          // execute statement
          RETURN_FUNCTION(statement_ptr()->execute());
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

      // ==============================================================
      // statement thingies

      statement::statement(csl::db::driver & d, const ustr & q, MYSQL * c) :
        csl::db::statement(d,q), stmt_(0), conn_(c), bound_variables_(0),
        param_count_(0), use_exc_(true)
      {
        ENTER_FUNCTION();

        if( !conn_ )         { THRRNORET(csl::db::exc::rs_nullparam,L"conn_ is null"); }
        if( q.size() == 0 )  { THRRNORET(csl::db::exc::rs_empty_query,L"query is empty"); }

        stmt_ = mysql_stmt_init(conn_);

        if( stmt_ == NULL ) { THRRNORET(csl::db::exc::rs_mysql_outofmem,L"mysql out of mem"); }
        else                { CSL_DEBUGF(L"mysql statement initialized:%p",stmt_); }

        if( mysql_stmt_prepare(stmt_, q.c_str(), static_cast<unsigned long>(q.size())) )
        {
          str err(mysql_stmt_error(stmt_));
          CSL_DEBUGF(L"cannot prepare query:[%s] ERROR:[%ls]",
                      q.c_str(),
                      err.c_str() );

          mysql_stmt_close(stmt_);
          stmt_ = 0;
          THRRNORET(csl::db::exc::rs_cannot_prepare,err.c_str());
        }

        param_count_= mysql_stmt_param_count(stmt_);
        CSL_DEBUGF(L"query has %lld parameters",param_count_);

        bound_variables_ =  new MYSQL_BIND[param_count_];
        ::memset( bound_variables_,0,sizeof(MYSQL_BIND)*static_cast<size_t>(param_count_) );
        LEAVE_FUNCTION();
      }

      statement::~statement()
      {
        ENTER_FUNCTION();
        if( stmt_ )
        {
          CSL_DEBUGF(L"free mysql statement:%p",stmt_);
          if( mysql_stmt_close(stmt_) )
          {
            CSL_DEBUGF(L"failed to free statement:%p [%s]",
                        stmt_,
                        mysql_stmt_error(stmt_));
          }
          stmt_ = 0;
        }
        if( bound_variables_ )
        {
          delete [] bound_variables_;
          bound_variables_ = 0;
        }
        LEAVE_FUNCTION();
      }

      statement::statement() :
        csl::db::statement(*(new csl::db::mysql::driver()),*(new ustr()))
      {
        ENTER_FUNCTION();
        throw "should never be called";
        LEAVE_FUNCTION();
      }

      bool statement::const_bind(uint64_t which, const ustr & column, const var & value)
      {
        ENTER_FUNCTION();
#ifdef DEBUG
        ustr tmp; tmp << value;
        CSL_DEBUGF(L"const_bind(which:%lld,column:%s,value:'%s')",which,column.c_str(),tmp.c_str());
#endif
        if( which < 1 || which > param_count_ )
        {
          CSL_DEBUGF(L"trying to bind an invalid index:%lld [bound_variables_:%lld]",
                     which, param_count_ );
          THRR(csl::db::exc::rs_invalid_param,L"invalid parameter index",false);
        }

        item * i = params_.construct(which-1);

        if( !i ) { THRR(db::exc::rs_internal,L"inpvec cannot construct",false); }

        unsigned char * bf = 0;
        i->arg_ = &value;

        if( value.var_size() > 0 )
        {
          size_t sz = static_cast<size_t>(value.var_size());

          // TODO : check str type here....

          // remove trailing zero
          if( value.var_type() == CSL_TYPE_USTR )     { --sz; }
          else if( value.var_type() == CSL_TYPE_STR ) { sz -= sizeof(wchar_t); }

          bf = i->buffer_.allocate( sz );
          ::memcpy( bf,value.ucharp_data(),sz );
          i->is_null_ = 0;
          i->length_  = static_cast<unsigned long>(sz);
        }
        else
        {
          i->is_null_ = 1;
        }

        bound_variables_[which-1].buffer         = reinterpret_cast<char *>(bf);
        bound_variables_[which-1].is_null        = &(i->is_null_);
        bound_variables_[which-1].length         = &(i->length_);
        bound_variables_[which-1].buffer_length  = i->length_;

        switch( value.var_type() )
        {
          case CSL_TYPE_INT64:  bound_variables_[which-1].buffer_type = MYSQL_TYPE_LONGLONG; break;
          case CSL_TYPE_DOUBLE: bound_variables_[which-1].buffer_type = MYSQL_TYPE_DOUBLE;   break;
          case CSL_TYPE_USTR:
          case CSL_TYPE_STR:    bound_variables_[which-1].buffer_type = MYSQL_TYPE_STRING;   break;
          case CSL_TYPE_BIN:    bound_variables_[which-1].buffer_type = MYSQL_TYPE_BLOB;     break;
          default:
            bound_variables_[which-1].buffer_type = MYSQL_TYPE_NULL;
            bound_variables_[which-1].buffer      = 0;
            i->is_null_                           = 1;
            break;
        };

        RETURN_FUNCTION(true);
      }

      bool statement::bind(uint64_t which, ustr & column, var & value)
      {
        ENTER_FUNCTION();
#ifdef DEBUG
        ustr tmp; tmp << value;
        CSL_DEBUGF(L"bind(which:%lld,column:%s,value:'%s')",which,column.c_str(),tmp.c_str());
#endif
        // TODO : XXX - bind variable here using
        // NOTE : this function should be used when fetching into user supplied
        // parameter values
        RETURN_FUNCTION(true);
      }

      bool statement::execute()
      {
        ENTER_FUNCTION();

        if( stmt_ == 0 ) { THRR(db::exc::rs_nullstmnt,L"stmt_ variable is null",false); }

        if( param_count_ > 0 && bound_variables_ > 0 )
        {
          CSL_DEBUG_ASSERT( param_count_ == params_.n_items() );

          if( mysql_stmt_bind_param(stmt_, bound_variables_) )
          {
            str err(mysql_stmt_error(stmt_));
            CSL_DEBUGF(L"mysql_bind_param() => ERROR[%ls]", err.c_str());
            THRR(db::exc::rs_mysql_stmt_bind,err.c_str(),false);
          }
        }

        if( mysql_stmt_execute(stmt_) )
        {
          str err(mysql_stmt_error(stmt_));
          CSL_DEBUGF(L"mysql_stmt_execute() => ERROR[%ls]",err.c_str());
          THRR(db::exc::rs_mysql_stmt_execute,err.c_str(),false);
        }

        my_ulonglong affected_rows = mysql_stmt_affected_rows(stmt_);

        CSL_DEBUGF(L"affected rows:%lld",affected_rows);

        RETURN_FUNCTION(true);
      }

      // ==============================================================
      // driver thingies

      csl::db::statement * driver::prepare(const ustr & q)
      {
        ENTER_FUNCTION();
        CSL_DEBUGF(L"prepare(%s)",q.c_str());
        RETURN_FUNCTION( (new csl::db::mysql::statement(*this,q,conn_)) );
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

        bool ret = false;

        if( !conn_ )
        {
          // init
          conn_ = mysql_init(NULL);
          if( !conn_ ) { THRR(db::exc::rs_mysql_init,L"mysql_init() failed",false); }
          else         { CSL_DEBUGF(L"mysql connection: %p",conn_); }
        }

        CSL_DEBUGF(L"open(info[host:%s port:%lld db_name:%s user:%s password:%s])",
                    info.host_.c_str(),
                    info.port_.value(),
                    info.db_name_.c_str(),
                    info.user_.c_str(),
                    info.password_.c_str() );

        if( mysql_real_connect( conn_,
                                info.host_.c_str(),
                                info.user_.c_str(),
                                info.password_.c_str(),
                                info.db_name_.c_str(),
                                static_cast<unsigned int>(info.port_.value()),
                                NULL, /* unix socket */
                                0 /* client flags */ ) == NULL )
        {
          str err(mysql_error(conn_));
          CSL_DEBUGF(L"mysql error [%ls] during mysql_real_connect(%p,...)",
                       err.c_str(),
                       conn_ );
          THRR(db::exc::rs_mysql_real_connect,err.c_str(),ret);
        }
        ret = true;

        RETURN_FUNCTION(ret);
      }

      bool driver::open(const ustr & connect_string)
      {
        ENTER_FUNCTION();
        CSL_DEBUGF(L"open(%s)",connect_string.c_str());
        THR(db::exc::rs_not_implemented,false);
        RETURN_FUNCTION(false);
      }

      bool driver::close()
      {
        ENTER_FUNCTION();
        bool ret = false;
        if( conn_ )
        {
          ret = true;
          CSL_DEBUGF(L"closing mysql connection:%p",conn_);
          mysql_close(conn_);
          conn_ = 0;
        }
        CSL_DEBUGF(L"close() => %s",(ret==true?"TRUE":"FALSE"));
        RETURN_FUNCTION(ret);
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

      driver::driver() : conn_(0), use_exc_(true)
      {
        ENTER_FUNCTION();
        LEAVE_FUNCTION();
      }

      driver::~driver()
      {
        ENTER_FUNCTION();
        if( conn_ )
        {
          CSL_DEBUGF(L"closing mysql connection:%p",conn_);
          mysql_close(conn_);
          conn_ = 0;
        }
        LEAVE_FUNCTION();
      }
    } // end of ns:csl::db::mysql
  } // end of ns:csl::db
} // end of ns:csl

/* EOF */
