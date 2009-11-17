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

#if 0
#ifndef DEBUG
#define DEBUG
#define DEBUG_ENABLE_INDENT
//#define DEBUG_VERBOSE
#endif /* DEBUG */
#endif /* 0 */

#include "codesloop/db/_shared_impl.hh"
#include "codesloop/common/common.h"
#include "codesloop/common/int64.hh"
#include "codesloop/common/dbl.hh"
#include "codesloop/common/binry.hh"
#include "codesloop/common/str.hh"
#include "codesloop/common/ustr.hh"
#include <map>

using csl::common::str;
using csl::common::ustr;
using csl::common::int64;
using csl::common::dbl;
using csl::common::binry;

/**
  @file _shared_impl.cc
  @brief private implementation of slt3 classes
 */

#define THRE(FUN,RC,MSG,RET) \
    do { \
      if( this->use_exc_ ) \
      { \
        exc e = FUN(RC,this->get_class_name(),MSG);\
        e.file_ = L""__FILE__; \
        e.line_ = __LINE__; \
        throw e; \
        return RET; \
      } \
      else \
      { \
        FPRINTF(stderr,L"Exception(%ls:%d): [%ls] : %ls\n", \
            L""__FILE__,__LINE__, \
            exc::reason_string(RC), \
            MSG); \
        return RET; \
      } \
    } while(false);

namespace csl
{
  namespace db
  {
    namespace slt3
    {
      namespace
      {
        struct auto_free
        {
          void * p_;
          auto_free(void * p) : p_(p) { }
          ~auto_free()
          {
            if(p_) sqlite3_free(p_);
            p_ = 0;
          }
        };
      };

      /* conn */
      conn::impl::impl()
        : db_(0), tran_id_(0), use_exc_(true) { }

      unsigned long long conn::impl::new_tran_id()
      {
        ++tran_id_;
        return tran_id_;
      }

      long long conn::impl::last_insert_id()
      {
        long long ret = -1;
        if( !db_ ) THR(exc::rs_notopened,-1);
        {
          ret = sqlite3_last_insert_rowid(db_);
          if( !ret ) ret = -1;
        }
        return ret;
      }

      long long conn::impl::change_count()
      {
        long long ret = -1;
        if( !db_ ) THR(exc::rs_notopened,-1);
        {
          ret = sqlite3_changes(db_);
          if( !ret ) ret = -1;
        }
        return ret;
      }

      bool conn::impl::open(const char * db)
      {
        sqlite3 * td = 0;
        int rc = 0;
        if( !db ) { return false; }
        else if( (rc=sqlite3_open( db, &td )) == SQLITE_OK )
        {
          name_ = db;
          db_ = td;
          sqlite3_busy_timeout(db_,10000);
          return true;
        }
        else
        {
          THRE(create_exc,rc,L"Failed to open DB",false);
        }
      }

      bool conn::impl::valid_db_ptr()
      {
        bool ret = false;
        {
          ret = (db_ == 0 ? false : true);
        }
        return ret;
      }

      bool conn::impl::close()
      {
        if( !db_ ) { return false; }
        else       { sqlite3_busy_timeout(db_,1000); }
        //
        int rc = 0;
        if( (rc=sqlite3_close(db_)) != SQLITE_OK )
        {
          db_ = 0;
          name_.clear();
          THRE(create_exc,rc,sqlite3_errmsg(db_),false);
        }
        else
        {
          db_ = 0;
          name_.clear();
          return true;
        }
      }

      exc conn::impl::create_exc(int rc, const wchar_t * component, const char * s)
      {
        return create_exc(rc,component,str(s));
      }

      exc conn::impl::create_exc(int rc, const wchar_t * component, const wchar_t * s)
      {
        return create_exc(rc,component,str(s));
      }

      exc conn::impl::create_exc(int rc, const wchar_t * component, const str & s)
      {
        exc e(component);
        if( s.size() > 0 ) e.text_ = s;
        switch( rc )
        {
          case SQLITE_INTERNAL:   e.reason_ = exc::rs_internal;   break;
          case SQLITE_PERM:       e.reason_ = exc::rs_permission; break;
          case SQLITE_ABORT:      e.reason_ = exc::rs_abort;      break;
          case SQLITE_BUSY:       e.reason_ = exc::rs_busy;       break;
          case SQLITE_LOCKED:     e.reason_ = exc::rs_locked;     break;
          case SQLITE_NOMEM:      e.reason_ = exc::rs_nomem;      break;
          case SQLITE_READONLY:   e.reason_ = exc::rs_readonly;   break;
          case SQLITE_IOERR:      e.reason_ = exc::rs_ioerr;      break;
          case SQLITE_CORRUPT:    e.reason_ = exc::rs_corrupt;    break;
          case SQLITE_NOTFOUND:   e.reason_ = exc::rs_notfound;   break;
          case SQLITE_FULL:       e.reason_ = exc::rs_full;       break;
          case SQLITE_CANTOPEN:   e.reason_ = exc::rs_cantopen;   break;
          case SQLITE_PROTOCOL:   e.reason_ = exc::rs_protocol;   break;
          case SQLITE_EMPTY:      e.reason_ = exc::rs_empty;      break;
          case SQLITE_SCHEMA:     e.reason_ = exc::rs_schema;     break;
          case SQLITE_TOOBIG:     e.reason_ = exc::rs_toobig;     break;
          case SQLITE_CONSTRAINT: e.reason_ = exc::rs_constraint; break;
          case SQLITE_MISMATCH:   e.reason_ = exc::rs_mismatch;   break;
          case SQLITE_MISUSE:     e.reason_ = exc::rs_misuse;     break;
          case SQLITE_AUTH:       e.reason_ = exc::rs_auth;       break;
          case SQLITE_FORMAT:     e.reason_ = exc::rs_format;     break;
          case SQLITE_RANGE:      e.reason_ = exc::rs_range;      break;
          case SQLITE_NOTADB:     e.reason_ = exc::rs_notadb;     break;
          default:                e.reason_ = exc::rs_unknown;    break;
        };
        return e;
      }

      bool conn::impl::exec_noret(const char * sql)
      {
        char * zErr = 0;
        if( !db_ ) THR(exc::rs_notopened,false);
        if( !sql ) THR(exc::rs_nullparam,false);

        int rc = sqlite3_exec( db_, sql, NULL, NULL, &zErr );

        if( rc == SQLITE_OK || rc == SQLITE_ROW || rc == SQLITE_DONE )
        {
          ; // OK
        }
        else
        {
          str s;
          if( zErr ) { s = zErr; sqlite3_free( zErr ); }
          THRE(create_exc,rc,s.c_str(),false);
        }
        return true;
      }

      bool conn::impl::exec(const char * sql,common::ustr & res)
      {
        char * zErr = 0;
        if( !db_ ) THR(exc::rs_notopened,false);
        if( !sql ) THR(exc::rs_nullparam,false);

        char    **   rset = NULL;
        int          nrow = 0;
        int          ncol = 0;

        int rc = sqlite3_get_table( db_,
                                    sql,
                                    &rset,
                                    &nrow,
                                    &ncol,
                                    &zErr );


        if( rc == SQLITE_OK || rc == SQLITE_ROW || rc == SQLITE_DONE )
        {
          if( ncol > 0 && nrow > 0 && rset[ncol] )
          {
            res = rset[ncol];
          }
        }
        else
        {
          if( rset ) sqlite3_free_table( rset );
          rset = 0;
          str s;
          if( zErr ) { s = zErr; sqlite3_free( zErr ); }
          THRE(create_exc,rc,s.c_str(),false);
        }

        if( rset ) sqlite3_free_table( rset );
        return true;
      }

      conn::impl::~impl()
      {
        close();
      }

      /* tran */
      tran::impl::impl(conn::impl_t & c)
        : cn_(&(*c)), tr_(0), tran_id_(cn_->new_tran_id()),
                 do_rollback_(false), do_commit_(true), use_exc_(c->use_exc_),
                              started_(false) { }

      tran::impl::impl(tran::impl_t & t)
        : cn_(t->cn_), tr_(&(*t)), tran_id_(t->cn_->new_tran_id()),
              do_rollback_(false), do_commit_(true), use_exc_(t->use_exc_),
                           started_(false) { }

      void tran::impl::start()
      {
        ENTER_FUNCTION( );
        if( !started_ )
        {
          if( tr_ )
          {
            char * sql = sqlite3_mprintf("SAVEPOINT SP%lld;",tran_id_);
            auto_free fr(sql);
            cn_->exec_noret(sql);
          }
          else
          {
            cn_->exec_noret("BEGIN DEFERRED TRANSACTION;");
          }
          started_ = true;
        }
        LEAVE_FUNCTION( );
      }

      tran::impl::~impl()
      {
        try
        {
          if( !started_ ) { }
          else if( do_rollback_ )
          {
            rollback();
          }
          else if( do_commit_ )
          {
            commit();
          }
        }
        catch(db::exc e)
        {
          // catch all
        }
      }

      void tran::impl::commit_on_destruct(bool yesno)
      {
        ENTER_FUNCTION( );
        do_commit_ = yesno;
        LEAVE_FUNCTION( );
      }

      void tran::impl::rollback_on_destruct(bool yesno)
      {
        ENTER_FUNCTION( );
        do_rollback_ = yesno;
        LEAVE_FUNCTION( );
      }

      void tran::impl::commit()
      {
        ENTER_FUNCTION( );
        if( !started_ ) { }
        else if( cn_->valid_db_ptr() == false ) { THRNORET(exc::rs_notopened); }
        else if( tr_ )
        {
          char * sql = sqlite3_mprintf("RELEASE SAVEPOINT SP%lld;",tran_id_);
          auto_free fr(sql);
          cn_->exec_noret(sql);
          started_ = false;
        }
        else
        {
          cn_->exec_noret("COMMIT TRANSACTION;");
          started_ = false;
        }
        LEAVE_FUNCTION( );
      }

      void tran::impl::rollback()
      {
        ENTER_FUNCTION( );
        if( !started_ ) { }
        else if( cn_->valid_db_ptr() == false ) { THRNORET(exc::rs_notopened); }
        else if( tr_ )
        {
          char * sql = sqlite3_mprintf("ROLLBACK TRANSACTION TO SAVEPOINT SP%lld;",tran_id_);
          auto_free fr(sql);
          cn_->exec_noret(sql);
          started_ = false;
        }
        else
        {
          cn_->exec_noret("ROLLBACK TRANSACTION;");
          started_ = false;
        }
        LEAVE_FUNCTION( );
      }

      /* query */
      query::impl::impl(tran::impl_t & t)
        : tran_(&(*t)), stmt_(0), tail_(0),
                   use_exc_(t->use_exc_), autoreset_data_(true),
                            last_insert_id_(-1), change_count_(-1)
      {
      }

      query::impl::~impl()
      {
        finalize();
      }

      void query::impl::finalize()
      {
        if( stmt_ )
        {
          sqlite3_finalize(stmt_);
          stmt_ = 0;
        }
      }

      void query::impl::debug()
      {
        printf("== query::impl::debug ==\n");
        printf("== ////// column_pool_ ==\n");
        column_pool_.debug();
        printf("== /////// field_pool_ ==\n");
        field_pool_.debug();
        printf("== /////////// params_ ==\n");
        params_.debug();

        parampool_t::iterator it(params_.begin());
        parampool_t::iterator end(params_.end());

        for( ;it!=end;++it )
        {
          printf("--");
          // TODO var * p = (*it);
          // TODO if( p ) { p->debug(); }
          // TODO else    { printf("Param is null\n"); }
        }
      }

      void query::impl::clear_params()
      {
        ENTER_FUNCTION_X( );
        params_.free_all();
        param_pool_.free_all();
        LEAVE_FUNCTION_X( );
      }

      // stepwise query
      bool query::impl::prepare(const char * sql)
      {
        ENTER_FUNCTION( );

        if( !sql )               THR(exc::rs_nullparam,false);
        if( !tran_ )             THR(exc::rs_nulltran,false);
        if( !(tran_->cn_) )      THR(exc::rs_nullconn,false);
        if( !(tran_->cn_->db_) ) THR(exc::rs_nulldb,false);

        CSL_DEBUGF(L"prepare(sql:'%s')",sql);

        finalize();
        tran_->start();

        if( autoreset_data_ )
        {
          column_pool_.free_all();
          field_pool_.free_all();
          data_pool_.free_all();
          coldata_pool_.free_all();
        }

        int rc = 0;

        rc = sqlite3_prepare(tran_->cn_->db_, sql, strlen(sql), &stmt_, &tail_);

        if(rc != SQLITE_OK)
        {
          str s(sqlite3_errmsg(tran_->cn_->db_));
          THRE(conn::impl::create_exc,rc,s.c_str(),false);
        }
        //
        RETURN_FUNCTION( true );
      }

      bool query::impl::reset()
      {
        ENTER_FUNCTION( );

        if( !stmt_ )        THR(exc::rs_nullstmnt,false);
        if( !tran_ )        THR(exc::rs_nulltran,false);
        if( !(tran_->cn_) ) THR(exc::rs_nullconn,false);

        int rc = 0;

        rc = sqlite3_reset(stmt_);

        if( rc != SQLITE_OK )
        {
          str s(sqlite3_errmsg(tran_->cn_->db_));
          THRE(conn::impl::create_exc,rc,s.c_str(),false);
        }

        RETURN_FUNCTION( true );
      }

      void query::impl::reset_data()
      {
        ENTER_FUNCTION( );
        field_pool_.free_all();
        data_pool_.free_all();
        LEAVE_FUNCTION( );
      }

      bool query::impl::next(columns_t & cols, fields_t & fields)
      {
        ENTER_FUNCTION( );
        CSL_DEBUGF(L"next(cols,fields) [assuming rows to be returned]");

        if( !stmt_ )        THR(exc::rs_nullstmnt,false);
        if( !tran_ )        THR(exc::rs_nulltran,false);
        if( !(tran_->cn_) ) THR(exc::rs_nullconn,false);

        parampool_t::iterator it(params_.begin());
        parampool_t::iterator end(params_.end());
        unsigned int which = 0;

        if( autoreset_data_ ) reset_data();

        // bind params
        {
          for( ;it!=end;++it )
          {
            common::var * p = *it;
            if( p && which > 0 )
            {
              int t = p->var_type();
              switch( t )
              {
                case query::colhead::t_integer:
                  CSL_DEBUGF(L"binding param #%d [int64] = %lld",which,p->get_long());
                  sqlite3_bind_int64( stmt_, which, p->get_long() );
                  break;

                case query::colhead::t_double:
                  CSL_DEBUGF(L"binding param #%d [double] = %lf",which,p->get_double());
                  sqlite3_bind_double( stmt_, which, p->get_double() );
                  break;

                case query::colhead::t_string:
                  /* assume string always has a trailing zero */
                  CSL_DEBUGF(L"binding param #%d [string] = '%s'",which,p->charp_data());
                  sqlite3_bind_text( stmt_, which, p->charp_data(), (p->var_size()-1), SQLITE_TRANSIENT );
                  break;

                case query::colhead::t_blob:
                  CSL_DEBUGF(L"binding param #%d [blob] = %d bytes",which,p->var_size());
                  sqlite3_bind_blob( stmt_, which, (p->charp_data()), (p->var_size()), SQLITE_TRANSIENT );
                  break;

                case query::colhead::t_null:
                default:
                  break;
              };
            }
            ++which;
          }
        }

        // step query
        int rc = 0;

        rc = sqlite3_step( stmt_ );

        if( rc == SQLITE_ROW )
        {
          CSL_DEBUGF(L"sqlite3_step() returned %d = SQLITE_ROW",rc);

          fill_columns();
          if( cols.n_items() == 0 ) copy_columns( cols );

          columnpool_t::iterator i( column_pool_.begin() );
          columnpool_t::iterator e( column_pool_.end() );

          fields.free_all();

          unsigned int ac=0;
          for( ;i!=e;++i )
          {
            query::field * f = 0;
            switch( (*i)->type_ )
            {
              case query::colhead::t_integer:
                f = new common::int64(sqlite3_column_int64(stmt_,ac));
                CSL_DEBUGF(L"column%d [int64] = %lld",ac,f->get_long());
                break;

              case query::colhead::t_double:
                f = new common::dbl(sqlite3_column_double(stmt_,ac));
                CSL_DEBUGF(L"column%d [double] = %lf",ac,f->get_double());
                break;

              case query::colhead::t_blob:
                f = new common::binry();
                f->from_binary( sqlite3_column_blob(stmt_,ac), sqlite3_column_bytes(stmt_,ac) );
                CSL_DEBUGF(L"column%d [blob] = %lld bytes",ac,f->var_size());
                break;

              case query::colhead::t_string:
                f = new common::ustr( reinterpret_cast<const char *>(sqlite3_column_text(stmt_,ac)) );
                CSL_DEBUGF(L"column%d [string] = '%s'",ac,f->charp_data());
                break;

              case query::colhead::t_null:
              default:
                //f = new common::ustr();
                f = 0;
                break;
            };
            field_pool_.push_back(f);
            fields.push_back(f);
            ++ac;
          }
        }
        else if( rc == SQLITE_OK )
        {
          CSL_DEBUGF(L"sqlite3_step() returned %d = SQLITE_OK",rc);
          last_insert_id_ = tran_->cn_->last_insert_id();
          change_count_ = tran_->cn_->change_count();
          CSL_DEBUGF(L"last_insert_id:%lld change_count:%lld",last_insert_id_,change_count_);
          RETURN_FUNCTION( true );
        }
        else if( rc == SQLITE_DONE )
        {
          CSL_DEBUGF(L"sqlite3_step() returned %d = SQLITE_DONE",rc);
          last_insert_id_ = tran_->cn_->last_insert_id();
          change_count_ = tran_->cn_->change_count();
          CSL_DEBUGF(L"last_insert_id:%lld change_count:%lld",last_insert_id_,change_count_);
          RETURN_FUNCTION( false );
        }
        else
        {
          CSL_DEBUGF(L"sqlite3_step() returned %d = ??? = ERROR",rc);
          str s(sqlite3_errmsg(tran_->cn_->db_));
          THRE(conn::impl::create_exc,rc,s.c_str(),false);
        }
        RETURN_FUNCTION( true );
      }

      bool query::impl::next()
      {
        ENTER_FUNCTION( );
        CSL_DEBUGF(L"next()");

        if( !stmt_ )         THR(exc::rs_nullstmnt,false);
        if( !tran_ )         THR(exc::rs_nulltran,false);
        if( !(tran_->cn_) )  THR(exc::rs_nullconn,false);

        parampool_t::iterator it(params_.begin());
        parampool_t::iterator end(params_.end());
        unsigned int which = 0;

        if( autoreset_data_ )
        {
          column_pool_.free_all();
          field_pool_.free_all();
          data_pool_.free_all();
          coldata_pool_.free_all();
        }

        // bind params
        {
          for( ;it!=end;++it )
          {
            common::var * p = *it;
            if( p && which > 0 )
            {
              int t = p->var_type();
              switch( t )
              {
                case query::colhead::t_integer:
                  CSL_DEBUGF(L"binding param #%d [int64] = %lld",which,p->get_long());
                  sqlite3_bind_int64( stmt_, which, p->get_long() );
                  break;

                case query::colhead::t_double:
                  CSL_DEBUGF(L"binding param #%d [double] = %lf",which,p->get_double());
                  sqlite3_bind_double( stmt_, which, p->get_double() );
                  break;

                case query::colhead::t_string:
                  /* assume string always has a trailing zero */
                  CSL_DEBUGF(L"binding param #%d [string] = '%s'",which,p->charp_data());
                  sqlite3_bind_text( stmt_, which, (p->charp_data()), (p->var_size()-1), SQLITE_TRANSIENT );
                  break;

                case query::colhead::t_blob:
                  CSL_DEBUGF(L"binding param #%d [blob] = %lld bytes",which,p->var_size());
                  sqlite3_bind_blob( stmt_, which, (p->charp_data()), (p->var_size()), SQLITE_TRANSIENT );
                  break;

                case query::colhead::t_null:
                default:
                  break;
              };
            }
            ++which;
          }
        }

        // step query
        int rc = 0;

        rc = sqlite3_step( stmt_ );

        if( rc == SQLITE_ROW )
        {
          CSL_DEBUGF(L"sqlite3_step() returned %d = SQLITE_ROW",rc);
          RETURN_FUNCTION( true );
        }
        else if( rc == SQLITE_OK  )
        {
          CSL_DEBUGF(L"sqlite3_step() returned %d = SQLITE_OK",rc);
          last_insert_id_ = tran_->cn_->last_insert_id();
          change_count_ = tran_->cn_->change_count();
          CSL_DEBUGF(L"last_insert_id:%lld change_count:%lld",last_insert_id_,change_count_);
          RETURN_FUNCTION( true );
        }
        else if( rc == SQLITE_DONE )
        {
          CSL_DEBUGF(L"sqlite3_step() returned %d = SQLITE_DONE",rc);
          last_insert_id_ = tran_->cn_->last_insert_id();
          change_count_ = tran_->cn_->change_count();
          CSL_DEBUGF(L"last_insert_id:%lld change_count:%lld",last_insert_id_,change_count_);
          RETURN_FUNCTION( false );
        }
        else
        {
          CSL_DEBUGF(L"sqlite3_step() returned %d = ??? = ERROR",rc);
          str s(sqlite3_errmsg(tran_->cn_->db_));
          THRE(conn::impl::create_exc,rc,s.c_str(),false);
        }

        RETURN_FUNCTION( true );
      }

      long long query::impl::last_insert_id()
      {
        ENTER_FUNCTION_X( );
        CSL_DEBUGF_X( L"last_insert_id() => %lld",last_insert_id_ );
        RETURN_FUNCTION_X( last_insert_id_ );
      }

      long long query::impl::change_count()
      {
        ENTER_FUNCTION_X( );
        CSL_DEBUGF_X( L"change_count() => %lld",change_count_ );
        RETURN_FUNCTION_X( change_count_ );
      }

      // oneshot query
      bool query::impl::execute(const char * sql)
      {
        ENTER_FUNCTION( );

        if( !sql )          THR(exc::rs_nullparam,false);
        if( !tran_ )        THR(exc::rs_nulltran,false);
        if( !(tran_->cn_) ) THR(exc::rs_nullconn,false);

        finalize();
        tran_->start();

        if( autoreset_data_ )
        {
          column_pool_.free_all();
          field_pool_.free_all();
          data_pool_.free_all();
          coldata_pool_.free_all();
        }

        bool ret = tran_->cn_->exec_noret(sql);
        if( ret )
        {
          last_insert_id_ = tran_->cn_->last_insert_id();
          change_count_ = tran_->cn_->change_count();
          CSL_DEBUGF(L"last_insert_id:%lld change_count:%lld",last_insert_id_,change_count_);
        }
        RETURN_FUNCTION( ret );
      }

      bool query::impl::execute(const char * sql, ustr & result)
      {
        ENTER_FUNCTION( );

        if( !sql )          THR(exc::rs_nullparam,false);
        if( !tran_ )        THR(exc::rs_nulltran,false);
        if( !(tran_->cn_) ) THR(exc::rs_nullconn,false);

        finalize();
        tran_->start();

        if( autoreset_data_ )
        {
          column_pool_.free_all();
          field_pool_.free_all();
          data_pool_.free_all();
          coldata_pool_.free_all();
        }

        bool ret = tran_->cn_->exec(sql,result);
        if( ret )
        {
          last_insert_id_ = tran_->cn_->last_insert_id();
          change_count_ = tran_->cn_->change_count();
          CSL_DEBUGF(L"last_insert_id:%lld change_count:%lld",last_insert_id_,change_count_);
        }

        RETURN_FUNCTION( ret );
      }

      void query::impl::copy_columns(columns_t & cols)
      {
        ENTER_FUNCTION( );
        if( column_pool_.n_items() > 0 && cols.n_items() == 0 )
        {
          columnpool_t::iterator it(column_pool_.begin());
          columnpool_t::iterator end(column_pool_.end());
          for( ;it!=end;++it )
          {
            cols.push_back( *it );
          }
        }
        LEAVE_FUNCTION( );
      }

      bool query::impl::fill_columns()
      {
        ENTER_FUNCTION( );
        if( !stmt_ ) { RETURN_FUNCTION( false ); }

        if( column_pool_.n_items() == 0 )
        {
          int ncols = sqlite3_column_count(stmt_);
          if( ncols > 0 )
          {
            for(int i=0;i<ncols;++i )
            {
              query::colhead * h = new query::colhead();
              //
              switch( sqlite3_column_type(stmt_,i) )
              {
                case SQLITE_INTEGER:
                  h->type_ = query::colhead::t_integer;
                  break;

                case SQLITE_FLOAT:
                  h->type_ = query::colhead::t_double;
                  break;

                case SQLITE_NULL:
                  h->type_ = query::colhead::t_null;
                  break;

                case SQLITE_TEXT:
                  h->type_ = query::colhead::t_string;
                  break;

                case SQLITE_BLOB:
                default:
                  h->type_ = query::colhead::t_blob;
                  break;
              };
              //
              h->name_   = coldata_pool_.strdup( reinterpret_cast<const char *>(sqlite3_column_name(stmt_,i)) );
              h->table_  = coldata_pool_.strdup( reinterpret_cast<const char *>(sqlite3_column_table_name(stmt_,i)) );
              h->db_     = coldata_pool_.strdup( reinterpret_cast<const char *>(sqlite3_column_database_name(stmt_,i)) );
              h->origin_ = coldata_pool_.strdup( reinterpret_cast<const char *>(sqlite3_column_origin_name(stmt_,i)) );
              column_pool_.push_back(h);
            }
          }
        }
        bool ret = (column_pool_.n_items() > 0 ? true : false);
        RETURN_FUNCTION( ret );
      }

    } /* end of slt3 namespace */
  } /* end of db namespace */
} /* end of cls namespace */

/* EOF */
