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

#include "_shared_impl.hh"
#include "common.h"
#include "str.hh"
#include <map>

/**
  @file _shared_impl.cc
  @brief private implementation of slt3 classes
 */

#define THRE(FUN,RC,COMPONENT,MSG,RET) \
    do { \
      if( this->use_exc_ ) \
      { \
        exc e = FUN(RC,COMPONENT,MSG);\
        e.file_ = __FILE__; \
        e.line_ = __LINE__; \
        throw e; \
        return RET; \
      } \
      else \
      { \
        fprintf(stderr,"Exception(%s:%d): [%s] [%s] : %s\n", \
            __FILE__,__LINE__, \
            exc::component_string(COMPONENT), \
            exc::reason_string(RC), \
            MSG); \
        return RET; \
      } \
    } while(false);

namespace csl
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
      scoped_mutex m(mtx_);
      ++tran_id_;
      return tran_id_;
    }

    long long conn::impl::last_insert_id()
    {
      long long ret = -1;
      if( !db_ ) THR(exc::rs_notopened,exc::cm_conn,-1);
      {
        scoped_mutex m(mtx_);
        ret = sqlite3_last_insert_rowid(db_);
        if( !ret ) ret = -1;
      }
      return ret;
    }

    long long conn::impl::change_count()
    {
      long long ret = -1;
      if( !db_ ) THR(exc::rs_notopened,exc::cm_conn,-1);
      {
        scoped_mutex m(mtx_);
        ret = sqlite3_changes(db_);
        if( !ret ) ret = -1;
      }
      return ret;
    }

    bool conn::impl::open(const wchar_t * db)
    {
      scoped_mutex m(mtx_);
      sqlite3 * td = 0;
      int rc = 0;
      if( !db ) { return false; }
      else if( (rc=sqlite3_open16( db, &td )) == SQLITE_OK )
      {
        name_ = db;
        db_ = td;
        sqlite3_busy_timeout(db_,10000);
        return true;
      }
      else
      {
        THRE(create_exc,rc,exc::cm_conn,L"Failed to open DB",false);
      }
    }

    bool conn::impl::valid_db_ptr()
    {
      bool ret = false;
      {
        scoped_mutex m(mtx_);
        ret = (db_ == 0 ? false : true);
      }
      return ret;
    }

    bool conn::impl::close()
    {
      scoped_mutex m(mtx_);
      if( !db_ ) { return false; }
      else       { sqlite3_busy_timeout(db_,1000); }
      //
      int rc = 0;
      if( (rc=sqlite3_close(db_)) != SQLITE_OK )
      {
        db_ = 0;
        name_.clear();
        THRE(create_exc,rc,exc::cm_conn,sqlite3_errmsg(db_),false);
      }
      else
      {
        db_ = 0;
        name_.clear();
        return true;
      }
    }

    exc conn::impl::create_exc(int rc,int component, const common::str & str)
    {
      exc e(exc::cm_conn);
      if( str.size() > 0 ) e.text_ = str;
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
      scoped_mutex m(mtx_);
      char * zErr = 0;
      if( !db_ ) THR(exc::rs_notopened,exc::cm_conn,false);
      if( !sql ) THR(exc::rs_nullparam,exc::cm_conn,false);

      int rc = sqlite3_exec( db_, sql, NULL, NULL, &zErr );

      if( rc == SQLITE_OK || rc == SQLITE_ROW || rc == SQLITE_DONE )
      {
        ; // OK
      }
      else
      {
        common::str s;
        if( zErr ) { s = zErr; sqlite3_free( zErr ); }
        THRE(create_exc,rc,exc::cm_conn,s.c_str(),false);
      }
      return true;
    }

    bool conn::impl::exec(const char * sql,common::str & res)
    {
      scoped_mutex m(mtx_);
      char * zErr = 0;
      if( !db_ ) THR(exc::rs_notopened,exc::cm_conn,false);
      if( !sql ) THR(exc::rs_nullparam,exc::cm_conn,false);

      char **      rset = NULL;
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
        common::str s;
        if( zErr ) { s = zErr; sqlite3_free( zErr ); }
        THRE(create_exc,rc,exc::cm_conn,s.c_str(),false);
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
      scoped_mutex m(mtx_);
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
    }

    tran::impl::~impl()
    {
      try
      {
        scoped_mutex m(mtx_);
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
      catch(slt3::exc e)
      {
        // catch all
      }
    }

    void tran::impl::commit_on_destruct(bool yesno)
    {
      scoped_mutex m(mtx_);
      do_commit_ = yesno;
    }

    void tran::impl::rollback_on_destruct(bool yesno)
    {
      scoped_mutex m(mtx_);
      do_rollback_ = yesno;
    }

    void tran::impl::commit()
    {
      scoped_mutex m(mtx_);
      if( !started_ ) { }
      else if( cn_->valid_db_ptr() == false ) { THRNORET(exc::rs_notopened,exc::cm_tran); return; }
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
    }

    void tran::impl::rollback()
    {
      scoped_mutex m(mtx_);
      if( !started_ ) { }
      else if( cn_->valid_db_ptr() == false ) { THRNORET(exc::rs_notopened,exc::cm_tran); return; }
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
    }

    /* synqry */
    synqry::impl::impl(tran::impl_t & t)
      : tran_(&(*t)), stmt_(0), tail_(0), 
                 use_exc_(t->use_exc_), autoreset_data_(true),
                          last_insert_id_(-1), change_count_(-1)
    {
    }

    synqry::impl::~impl()
    {
      finalize();
    }

    void synqry::impl::finalize()
    {
      scoped_mutex m(mtx_);
      if( stmt_ )
      {
        scoped_mutex m2(tran_->cn_->mtx_);
        sqlite3_finalize(stmt_);
        stmt_ = 0;
      }
    }

    void synqry::impl::debug()
    {
      printf("== synqry::impl::debug ==\n");
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
        param * p = (*it);
        if( p ) { p->debug(); }
        else    { printf("Param is null\n"); }
      }
    }

    param & synqry::impl::get_param(unsigned int pos)
    {
      scoped_mutex m(mtx_);

      unsigned int sz = params_.n_items();
      param * ret = 0;
      param * q = 0;
      //
      if( pos > 2000 ) { THRNORET(exc::rs_toobig,exc::cm_synqry); goto bail; }
      // ensure items to be there
      if( sz <= pos )
        for( unsigned int i=sz;i<=pos;++i )
          params_.push_back(0);
    bail:
      //
      if( (q=params_.get_at(pos))!=NULL )
      {
        ret = q;
      }
      else
      {
        ret = new param(*this);
        params_.set_at(pos,ret);
      }
      return *ret;
    }

    void synqry::impl::clear_params()
    {
      scoped_mutex m(mtx_);

      params_.free_all();
      param_pool_.free_all();
    }

    // stepwise query
    bool synqry::impl::prepare(const char * sql)
    {
      scoped_mutex m(mtx_);

      if( !sql )               THR(exc::rs_nullparam,exc::cm_synqry,false);
      if( !tran_ )             THR(exc::rs_nulltran,exc::cm_synqry,false);
      if( !(tran_->cn_) )      THR(exc::rs_nullconn,exc::cm_synqry,false);
      if( !(tran_->cn_->db_) ) THR(exc::rs_nulldb,exc::cm_synqry,false);

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

      {
        scoped_mutex m2(tran_->cn_->mtx_);
        rc = sqlite3_prepare(tran_->cn_->db_, sql, strlen(sql), &stmt_, &tail_);
      }

      if(rc != SQLITE_OK)
      {
        scoped_mutex m2(tran_->cn_->mtx_);
        common::str s(sqlite3_errmsg(tran_->cn_->db_));
        THRE(conn::impl::create_exc,rc,exc::cm_synqry,s.c_str(),false);
      }
      //
      return true;
    }

    bool synqry::impl::reset()
    {
      scoped_mutex m(mtx_);

      if( !stmt_ )        THR(exc::rs_nullstmnt,exc::cm_synqry,false);
      if( !tran_ )        THR(exc::rs_nulltran,exc::cm_synqry,false);
      if( !(tran_->cn_) ) THR(exc::rs_nullconn,exc::cm_synqry,false);

      int rc = 0;

      {
        scoped_mutex m2(tran_->cn_->mtx_);
        rc = sqlite3_reset(stmt_);
      }

      if( rc != SQLITE_OK )
      {
        scoped_mutex m2(tran_->cn_->mtx_);
        common::str s(sqlite3_errmsg(tran_->cn_->db_));
        THRE(conn::impl::create_exc,rc,exc::cm_synqry,s.c_str(),false);
      }

      return true;
    }

    void synqry::impl::reset_data()
    {
      scoped_mutex m(mtx_);

      field_pool_.free_all();
      data_pool_.free_all();
    }

    bool synqry::impl::next(columns_t & cols, fields_t & fields)
    {
      scoped_mutex m(mtx_);

      if( !stmt_ )        THR(exc::rs_nullstmnt,exc::cm_synqry,false);
      if( !tran_ )        THR(exc::rs_nulltran,exc::cm_synqry,false);
      if( !(tran_->cn_) ) THR(exc::rs_nullconn,exc::cm_synqry,false);

      parampool_t::iterator it(params_.begin());
      parampool_t::iterator end(params_.end());
      unsigned int which = 0;

      if( autoreset_data_ ) reset_data();

      // bind params
      {
        scoped_mutex m2(tran_->cn_->mtx_);
        for( ;it!=end;++it )
        {
          param * p = *it;
          if( p && p->impl_->changed_ && which > 0 )
          {
            int t = p->get_type();
            switch( t )
            {
              case synqry::colhead::t_integer:
                sqlite3_bind_int64( stmt_, which, p->get_long() );
                break;

              case synqry::colhead::t_double:
                sqlite3_bind_double( stmt_, which, p->get_double() );
                break;

              case synqry::colhead::t_string:
                sqlite3_bind_text( stmt_, which, p->get_string(), p->get_size(), SQLITE_TRANSIENT );
                break;

              case synqry::colhead::t_blob:
                sqlite3_bind_blob( stmt_, which, p->get_ptr(), p->get_size(), SQLITE_TRANSIENT );
                break;

              case synqry::colhead::t_null:
              default:
                break;
            };
          }
          ++which;
        }
      }

      // step query
      int rc = 0;

      {
        scoped_mutex m2(tran_->cn_->mtx_);
        rc = sqlite3_step( stmt_ );
      }

      if( rc == SQLITE_ROW )
      {
        scoped_mutex m2(tran_->cn_->mtx_);

        fill_columns();
        if( cols.n_items() == 0 ) copy_columns( cols );

        columnpool_t::iterator it(column_pool_.begin());
        columnpool_t::iterator end(column_pool_.end());

        fields.free_all();

        unsigned int ac=0;
        for( ;it!=end;++it )
        {
          synqry::field * f = new synqry::field();
          switch( (*it)->type_ )
          {
            case synqry::colhead::t_integer:
              f->intval_   = sqlite3_column_int64(stmt_,ac);
              f->size_     = sizeof(long long);
              break;

            case synqry::colhead::t_double:
              f->doubleval_  = sqlite3_column_double(stmt_,ac);
              f->size_       = sizeof(double);
              break;

            case synqry::colhead::t_blob:
              f->blobval_ =
                  (unsigned char *)data_pool_.memdup( sqlite3_column_blob(stmt_,ac),
                                                      sqlite3_column_bytes(stmt_,ac) );
              f->size_ = sqlite3_column_bytes(stmt_,ac);
              break;

            case synqry::colhead::t_string:
              f->stringval_  = data_pool_.strdup((const char *)sqlite3_column_text(stmt_,ac));
              f->size_       = sqlite3_column_bytes(stmt_,ac);
              break;

            case synqry::colhead::t_null:
            default:
              f->size_ = 0;
              break;
          };
          field_pool_.push_back(f);
          fields.push_back(f);
          ++ac;
        }
      }
      else if( rc == SQLITE_OK )
      {
        last_insert_id_ = tran_->cn_->last_insert_id();
        change_count_ = tran_->cn_->change_count();
        return true;
      }
      else if( rc == SQLITE_DONE )
      {
        last_insert_id_ = tran_->cn_->last_insert_id();
        change_count_ = tran_->cn_->change_count();
        return false;
      }
      else
      {
        scoped_mutex m2(tran_->cn_->mtx_);
        common::str s(sqlite3_errmsg(tran_->cn_->db_));
        THRE(conn::impl::create_exc,rc,exc::cm_synqry,s.c_str(),false);
      }

      return true;
    }

    bool synqry::impl::next()
    {
      scoped_mutex m(mtx_);

      if( !stmt_ )         THR(exc::rs_nullstmnt,exc::cm_synqry,false);
      if( !tran_ )         THR(exc::rs_nulltran,exc::cm_synqry,false);
      if( !(tran_->cn_) )  THR(exc::rs_nullconn,exc::cm_synqry,false);

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
        scoped_mutex m2(tran_->cn_->mtx_);
        for( ;it!=end;++it )
        {
          param * p = *it;
          if( p && p->impl_->changed_ && which > 0 )
          {
            int t = p->get_type();
            switch( t )
            {
              case synqry::colhead::t_integer:
                sqlite3_bind_int64( stmt_, which, p->get_long() );
                break;

              case synqry::colhead::t_double:
                sqlite3_bind_double( stmt_, which, p->get_double() );
                break;

              case synqry::colhead::t_string:
                sqlite3_bind_text( stmt_, which, p->get_string(), p->get_size(), SQLITE_TRANSIENT );
                break;

              case synqry::colhead::t_blob:
                sqlite3_bind_blob( stmt_, which, p->get_ptr(), p->get_size(), SQLITE_TRANSIENT );
                break;

              case synqry::colhead::t_null:
              default:
                break;
            };
          }
          ++which;
        }
      }

      // step query
      int rc = 0;

      {
        scoped_mutex m2(tran_->cn_->mtx_);
        rc = sqlite3_step( stmt_ );
      }

      if( rc == SQLITE_ROW )
      {
        return true;
      }
      else if( rc == SQLITE_OK  )
      {
        last_insert_id_ = tran_->cn_->last_insert_id();
        change_count_ = tran_->cn_->change_count();
        return true;
      }
      else if( rc == SQLITE_DONE )
      {
        last_insert_id_ = tran_->cn_->last_insert_id();
        change_count_ = tran_->cn_->change_count();
        return false;
      }
      else
      {
        scoped_mutex m2(tran_->cn_->mtx_);
        common::str s(sqlite3_errmsg(tran_->cn_->db_));
        THRE(conn::impl::create_exc,rc,exc::cm_synqry,s.c_str(),false);
      }

      return true;
    }

    long long synqry::impl::last_insert_id()
    {
      long long ret = -1;
      {
        scoped_mutex m(mtx_);
        ret = last_insert_id_;
      }
      return ret;
    }

    long long synqry::impl::change_count()
    {
      long long ret = -1;
      {
        scoped_mutex m(mtx_);
        ret = change_count_;
      }
      return ret;
    }

    // oneshot query
    bool synqry::impl::execute(const char * sql)
    {
      scoped_mutex m(mtx_);

      if( !sql )          THR(exc::rs_nullparam,exc::cm_synqry,false);
      if( !tran_ )        THR(exc::rs_nulltran,exc::cm_synqry,false);
      if( !(tran_->cn_) ) THR(exc::rs_nullconn,exc::cm_synqry,false);

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
      }
      return ret;
    }

    bool synqry::impl::execute(const char * sql, common::str & result)
    {
      scoped_mutex m(mtx_);

      if( !sql )          THR(exc::rs_nullparam,exc::cm_synqry,false);
      if( !tran_ )        THR(exc::rs_nulltran,exc::cm_synqry,false);
      if( !(tran_->cn_) ) THR(exc::rs_nullconn,exc::cm_synqry,false);

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
      }

      return ret;
    }

    void synqry::impl::copy_columns(columns_t & cols)
    {
      scoped_mutex m(mtx_);

      if( column_pool_.n_items() > 0 && cols.n_items() == 0 )
      {
        columnpool_t::iterator it(column_pool_.begin());
        columnpool_t::iterator end(column_pool_.end());
        for( ;it!=end;++it )
        {
          cols.push_back( *it );
        }
      }
    }

    bool synqry::impl::fill_columns()
    {
      scoped_mutex m(mtx_);

      if( !stmt_ ) return false;

      if( column_pool_.n_items() == 0 )
      {
        scoped_mutex m2(tran_->cn_->mtx_);

        int ncols = sqlite3_column_count(stmt_);
        if( ncols > 0 )
        {
          for(int i=0;i<ncols;++i )
          {
            synqry::colhead * h = new synqry::colhead();
            //
            switch( sqlite3_column_type(stmt_,i) )
            {
              case SQLITE_INTEGER:
                h->type_ = synqry::colhead::t_integer;
                break;

              case SQLITE_FLOAT:
                h->type_ = synqry::colhead::t_double;
                break;

              case SQLITE_NULL:
                h->type_ = synqry::colhead::t_null;
                break;

              case SQLITE_TEXT:
                h->type_ = synqry::colhead::t_string;
                break;

              case SQLITE_BLOB:
              default:
                h->type_ = synqry::colhead::t_blob;
                break;
            };
            //
            h->name_   = coldata_pool_.strdup(sqlite3_column_name(stmt_,i));
            h->table_  = coldata_pool_.strdup(sqlite3_column_table_name(stmt_,i));
            h->db_     = coldata_pool_.strdup(sqlite3_column_database_name(stmt_,i));
            h->origin_ = coldata_pool_.strdup(sqlite3_column_origin_name(stmt_,i));
            column_pool_.push_back(h);
          }
        }
      }
      return (column_pool_.n_items() > 0 ? true : false);
    }

    /* param */
    param::impl::impl(synqry::impl & q)
      : q_(&q), changed_(false), type_(synqry::colhead::t_null), size_(0), ptr_(0), use_exc_(q.use_exc_)
      { }

    void param::impl::debug()
    {
      std::map<int,common::str> m;
      m[synqry::colhead::t_integer] = "integer";
      m[synqry::colhead::t_string]  = "string";
      m[synqry::colhead::t_double]  = "double";
      m[synqry::colhead::t_blob]    = "blob";
      m[synqry::colhead::t_null]    = "null";
      if( ptr_ ) printf("Param[%s]: %s\n",m[type_].c_str(),get_string());
      else       printf("Param[%s]: null\n",m[type_].c_str());
    }

    param::impl::~impl() {}

    int param::impl::get_type() const
    {
      return type_;
    }
// 
    unsigned int param::impl::get_size() const
    {
      return size_;
    }

    void * param::impl::get_ptr() const
    {
      return ptr_;
    }

    long long param::impl::get_long() const
    {
      if( !ptr_ ) THR(exc::rs_nullparam,exc::cm_param,false);
      //
      switch( type_ )
      {
        case synqry::colhead::t_integer:  return *((long long *)ptr_);
        case synqry::colhead::t_string:   return ATOLL((char *)ptr_);
        case synqry::colhead::t_double:   return (long long)(*((double *)ptr_));
        case synqry::colhead::t_blob:     return (*((long long *)ptr_));
        case synqry::colhead::t_null:
        default:
          break;
      };
      return 0;
    }

    double param::impl::get_double() const
    {
      if( !ptr_ ) THR(exc::rs_nullparam,exc::cm_param,0.0);
      //
      switch( type_ )
      {
        case synqry::colhead::t_integer:  return (double)(*((long long *)ptr_));
        case synqry::colhead::t_string:   return atof((char *)ptr_);
        case synqry::colhead::t_double:   return (*((double *)ptr_));
        case synqry::colhead::t_blob:     return (*((double *)ptr_));
        case synqry::colhead::t_null:
        default:
          break;
      };
      return 0.0;
    }

    const char * param::impl::get_string() const
    {
      if( !ptr_ ) THR(exc::rs_nullparam,exc::cm_param,"");
      //
      char tmp[200];
      switch( type_ )
      {
        case synqry::colhead::t_integer:
          SNPRINTF( tmp, sizeof(tmp)-1,"%lld",(*((long long *)ptr_)) );
          return q_->param_pool_.strdup(tmp);

        case synqry::colhead::t_double:
          SNPRINTF( tmp, sizeof(tmp)-1,"%.10f", (*((double *)ptr_)) );
          return q_->param_pool_.strdup(tmp);

        case synqry::colhead::t_string:
          return (char*)ptr_;

        case synqry::colhead::t_blob:
          return (char*)ptr_;

        case synqry::colhead::t_null:
        default:
          break;
      };
      return "";
    }

    bool param::impl::get(long long & val) const
    {
      if( !ptr_ ) THR(exc::rs_nullparam,exc::cm_param,false);
      val = get_long();
      return true;
    }

    bool param::impl::get(double & val) const
    {
      if( !ptr_ ) THR(exc::rs_nullparam,exc::cm_param,false);
      val = get_double();
      return true;
    }

    bool param::impl::get(common::str & val) const
    {
      if( !ptr_ ) THR(exc::rs_nullparam,exc::cm_param,false);
      val = get_string();
      return true;
    }

    bool param::impl::get(blob_t & val) const
    {
      if( !ptr_ ) THR(exc::rs_nullparam,exc::cm_param,false);
      val.assign( (unsigned char *)ptr_, ((unsigned char *)ptr_)+size_ );
      return true;
    }

    void param::impl::set(long long val)
    {
      ptr_      = q_->param_pool_.memdup(&val,sizeof(val));
      size_     = sizeof(val);
      type_     = synqry::colhead::t_integer;
      changed_  = true;
    }

    void param::impl::set(double val)
    {
      ptr_      = q_->param_pool_.memdup(&val,sizeof(val));
      size_     = sizeof(val);
      type_     = synqry::colhead::t_double;
      changed_  = true;
    }

    void param::impl::set(const common::str & val)
    {
      if( val.size() > 0 )
      {
        ptr_  = q_->param_pool_.strdup(val.c_str());
        size_ = val.size();
      }
      else
      {
        ptr_  = q_->param_pool_.memdup("\0",1);
        size_ = 0;
      }
      type_    = synqry::colhead::t_string;
      changed_ = true;
    }

    void param::impl::set(const char * val)
    {
      if( val )
      {
        ptr_  = q_->param_pool_.strdup(val);
        size_ = ::strlen(val);
      }
      else
      {
        ptr_  = q_->param_pool_.memdup("\0",1);
        size_ = 0;
      }
      type_    = synqry::colhead::t_string;
      changed_ = true;
    }

    void param::impl::set(const blob_t & val)
    {
      if( val.size() > 0 )
      {
        ptr_ = q_->param_pool_.memdup(&(val[0]),val.size());
      }
      size_    = val.size();
      type_    = synqry::colhead::t_blob;
      changed_ = true;
    }

    void param::impl::set(const unsigned char * ptr,unsigned int size)
    {
      if( ptr > 0 && size > 0 )
      {
        ptr_  = q_->param_pool_.memdup(ptr,size);
        size_ = size;
      }
      else
      {
        ptr_  = 0;
        size_ = 0;
      }
      type_    = synqry::colhead::t_blob;
      changed_ = true;
    }
  }
}

/* EOF */
