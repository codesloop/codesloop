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

#include "common.h"
#include "pvlist.hh"
#include "mpool.hh"
#include "mutex.hh"
#include "thread.hh"
#include "conn.hh"
#include "tran.hh"
#include "synqry.hh"
#include "param.hh"
#include "exc.hh"
#include "sqlite3.h"
#include "mpool.hh"

/**
  @file _shared_impl.hh
  @brief private implementation of slt3 classes
 */

using namespace csl::nthread;

namespace csl
{
  namespace slt3
  {
    struct conn::impl
    {
      /* variables */
      sqlite3 *            db_;
      mutex                mtx_;
      unsigned long long   tran_id_;
      bool                 use_exc_;
      std::string          name_;

      /* initialization */
      impl();
      ~impl();

      /* internal */
      unsigned long long new_tran_id();
      static exc create_exc(int rc,int component, const std::string & str);
      bool exec_noret(const char * sql);
      bool exec(const char * sql,std::string & res);
      bool valid_db_ptr();
      long long last_insert_id();
      long long change_count();

      /* interface */
      bool open(const char * db);
      bool close();

      /* inline functions */
      inline void use_exc(bool yesno)         { use_exc_ = yesno; }
      inline bool use_exc() const             { return use_exc_;  }
      inline const std::string & name() const { return name_; }
    };

    struct tran::impl
    {
      /* variables */
      conn::impl *         cn_;
      tran::impl *         tr_;
      unsigned long long   tran_id_;
      bool                 do_rollback_;
      bool                 do_commit_;
      bool                 use_exc_;
      bool                 started_;
      mutex                mtx_;

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
    };

    struct synqry::impl
    {
      /* variables */
      typedef common::pvlist< 32,param,common::delete_destructor<param> > parampool_t;
      typedef common::pvlist< 32,synqry::colhead,common::delete_destructor<colhead> > columnpool_t;
      typedef common::pvlist< 32,synqry::field,common::delete_destructor<field> > fieldpool_t;

      tran::impl *     tran_;
      columnpool_t     column_pool_;
      fieldpool_t      field_pool_;
      parampool_t      params_;
      common::mpool<>  coldata_pool_;
      common::mpool<>  param_pool_;
      common::mpool<>  data_pool_;
      sqlite3_stmt *   stmt_;
      const char *     tail_;
      bool             use_exc_;
      bool             autoreset_data_;
      long long        last_insert_id_;
      long long        change_count_;
      mutex            mtx_;

      /* initialization */
      impl(tran::impl_t & t);
      ~impl();

      /* internal */
      bool fill_columns();
      void finalize();

      /* interface */
      param & get_param(unsigned int pos);
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
      bool execute(const char * sql, std::string & result);

      void debug();

      /* inline functions */
      inline void use_exc(bool yesno) { use_exc_ = yesno; }
      inline bool use_exc() const     { return use_exc_;  }

      inline void autoreset_data(bool yesno) { autoreset_data_ = yesno; }
      inline bool autoreset_data() const     { return autoreset_data_; }
    };

    struct param::impl
    {
      /* variables */
      synqry::impl * q_;
      bool           changed_;
      unsigned int   type_;
      unsigned int   size_;
      void *         ptr_;
      bool           use_exc_;

      /* initialization */
      impl(synqry::impl & q);
      ~impl();

      /* internal */
      /* interface */
      int get_type() const;
      unsigned int get_size() const;
      void * get_ptr() const;
      long long get_long() const;
      double get_double() const;
      const char * get_string() const;

      bool get(long long & val) const;
      bool get(double & val) const;
      bool get(std::string & val) const;
      bool get(blob_t & val) const;

      void set(long long val);
      void set(double val);
      void set(const std::string & val);
      void set(const char * val);
      void set(const blob_t & val);
      void set(const unsigned char * ptr,unsigned int size);

      void debug();

      /* inline functions */
      inline void use_exc(bool yesno) { use_exc_ = yesno; }
      inline bool use_exc() const     { return use_exc_;  }

      inline bool is_empty() const { return ((!ptr_ || !size_) ? true : false); }
    };
  }
}

/* EOF */
