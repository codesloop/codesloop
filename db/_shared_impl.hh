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

#include "codesloop/common/pvlist.hh"
#include "codesloop/common/mpool.hh"
#include "codesloop/db/conn.hh"
#include "codesloop/db/tran.hh"
#include "codesloop/db/query.hh"
#include "codesloop/db/exc.hh"
#include "codesloop/db/sqlite3.h"
#include "codesloop/common/mpool.hh"
#include "codesloop/common/str.hh"
#include "codesloop/common/ustr.hh"
#include "codesloop/common/common.h"
#include "codesloop/common/csl_common.hh"

/**
  @file _shared_impl.hh
  @brief private implementation of slt3 classes
 */

namespace csl
{
  namespace db
  {
    namespace slt3
    {
      struct conn::impl
      {
        /* variables */
        sqlite3 *              db_;
        unsigned long long   tran_id_;
        bool                   use_exc_;
        common::ustr           name_;

        /* initialization */
        impl();
        ~impl();

        /* internal */
        unsigned long long new_tran_id();
        static exc create_exc(int rc, const wchar_t * component, const char * str);
        static exc create_exc(int rc, const wchar_t * component, const wchar_t * str);
        static exc create_exc(int rc, const wchar_t * component, const common::str & str);
        bool exec_noret(const char * sql);
        bool exec(const char * sql,common::ustr & res);
        bool valid_db_ptr();
        long long last_insert_id();
        long long change_count();

        /* interface */
        bool open(const char * db);
        bool close();

        /* inline functions */
        inline void use_exc(bool yesno)          { use_exc_ = yesno; }
        inline bool use_exc() const              { return use_exc_;  }
        inline const common::ustr & name() const { return name_; }

        CSL_OBJ(csl::db::slt3,conn::impl);
      };

      struct tran::impl
      {
        /* variables */
        conn::impl *           cn_;
        tran::impl *           tr_;
        unsigned long long   tran_id_;
        bool                   do_rollback_;
        bool                   do_commit_;
        bool                   use_exc_;
        bool                   started_;

        /* initialization */
        impl(conn::impl_t & c);
        impl(tran::impl_t & t);
        ~impl();

        /* internal */
        /* interface */
        void commit_on_destruct(bool yesno=true);
        void rollback_on_destruct(bool yesno=true);

        void start();
        void commit();
        void rollback();

        /* inline functions */
        inline void use_exc(bool yesno) { use_exc_ = yesno; }
        inline bool use_exc() const     { return use_exc_;  }

        CSL_OBJ(csl::db::slt3,tran::impl);
      };

      struct query::impl
      {
        /* variables */
        typedef common::pvlist< 32,common::var,common::delete_destructor<common::var> > parampool_t;
        typedef common::pvlist< 32,query::colhead,common::delete_destructor<colhead> > columnpool_t;
        typedef common::pvlist< 32,common::var,common::delete_destructor<common::var> > fieldpool_t;

        tran::impl *      tran_;
        columnpool_t      column_pool_;
        fieldpool_t       field_pool_;
        parampool_t      params_;
        common::mpool<>  coldata_pool_;
        fieldpool_t      param_pool_;
        common::mpool<>  data_pool_;
        sqlite3_stmt *   stmt_;
        const char *     tail_;
        bool             use_exc_;
        bool             autoreset_data_;
        long long        last_insert_id_;
        long long        change_count_;

        /* initialization */
        impl(tran::impl_t & t);
        ~impl();

        /* internal */
        bool fill_columns();
        void finalize();

        template <typename T>
        T & get_at(unsigned int pos)
        {
          if( pos > 2000 )
          {
            THR(exc::rs_toobig,
                *( reinterpret_cast<T *>(params_.push_back(new T()))));
          }
          unsigned int sz = params_.n_items();

          /* ensure we have enough space */
          if( sz <= pos )
            for( unsigned int i=sz;i<=pos;++i )
              params_.push_back(0);

          common::var * q = 0;

          if( (q=params_.get_at(pos))!=NULL )
          {
            if( q->var_type() == T::var_type_v )
            {
              return *( reinterpret_cast<T*>(q) );
            }
            else
            {
              delete q;
            }
          }

          T * t = new T();
          params_.set_at( pos, t );
          return *t;
        }

        /* interface */
        common::int64 & int64_param(unsigned int pos) { return get_at<common::int64>(pos); }
        common::dbl   & dbl_param(unsigned int pos)   { return get_at<common::dbl>(pos); }
        common::ustr  & ustr_param(unsigned int pos)  { return get_at<common::ustr>(pos); }
        common::binry & binry_param(unsigned int pos) { return get_at<common::binry>(pos); }

        common::var & set_param(unsigned int pos,const common::var & p)
        {
          switch( p.var_type() )
          {
            case query::colhead::t_integer:
            {
              common::int64 & ret(int64_param(pos));
              ret.from_var(p);
              return ret;
            }

            case query::colhead::t_string:
            {
              common::ustr & ret(ustr_param(pos));
              ret.from_var(p);
              return ret;
            }

            case query::colhead::t_double:
            {
              common::dbl & ret(dbl_param(pos));
              ret.from_var(p);
              return ret;
            }

            case query::colhead::t_blob:
            default:
            {
              common::binry & ret(binry_param(pos));
              ret.from_var(p);
              return ret;
            }
          };
          /* should never happen */
          common::binry & ret(binry_param(pos));
          ret.from_var(p);
          return ret;
        }

        unsigned int n_params();
        void clear_params();
        void copy_columns(columns_t & cols);

        long long last_insert_id();
        long long change_count();

        // stepwise query
        bool prepare(const char * sql);
        bool reset();
        void reset_data();
        bool next(columns_t & cols, fields_t & fields);
        bool next();

        // oneshot query
        bool execute(const char * sql);
        bool execute(const char * sql, common::ustr & result);

        void debug();

        /* inline functions */
        inline void use_exc(bool yesno) { use_exc_ = yesno; }
        inline bool use_exc() const     { return use_exc_;  }

        inline void autoreset_data(bool yesno) { autoreset_data_ = yesno; }
        inline bool autoreset_data() const     { return autoreset_data_; }

        CSL_OBJ(csl::db::slt3,query::impl);
      };
    } /* end of ns: csl::db::slt3 */
  } /* end of ns: csl::db */
} /* end of ns: csl */

/* EOF */
