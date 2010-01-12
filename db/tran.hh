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

#ifndef _csl_db_tran_hh_included_
#define _csl_db_tran_hh_included_

#include "codesloop/db/driver.hh"
#include "codesloop/db/conn.hh"
#include "codesloop/common/ustr.hh"

#ifdef __cplusplus

namespace csl
{
  namespace db
  {
    using common::ustr;

    class tran
    {
      public:
        tran(conn & c) :
            conn_(&c), parent_(0),
            commit_on_destruct_(true),
            rollback_on_destruct_(false)
        {
          conn_->get_driver().begin(id_);
        }

        tran(tran & t) :
            conn_(t.conn_), parent_(&t),
            commit_on_destruct_(true),
            rollback_on_destruct_(false)
        {
          conn_->get_driver().savepoint(id_, t.id_);
        }

        void commit()
        {
          if( parent_ == 0 ) conn_->get_driver().commit(id_);
          else               conn_->get_driver().release_savepoint(id_, parent_->id_);
          commit_on_destruct_    = false;
          rollback_on_destruct_  = false;
        }

        void rollback()
        {
          if( parent_ == 0 ) conn_->get_driver().rollback(id_);
          else               conn_->get_driver().rollback_savepoint(id_, parent_->id_);
          commit_on_destruct_    = false;
          rollback_on_destruct_  = false;
        }

        void commit_on_destruct(bool yesno=true)    { commit_on_destruct_   = yesno; }
        void rollback_on_destruct(bool yesno=true)  { rollback_on_destruct_ = yesno; }

        ~tran()
        {
          if( commit_on_destruct_ )         commit();
          else if( rollback_on_destruct_ )  rollback();
        }

      private:
        /* no default construction */
        tran() :
            conn_(0), parent_(0),
            commit_on_destruct_(false),
            rollback_on_destruct_(false) { }

        /* no copy */
        tran & operator=(const tran & other) { return *this; }

        conn * conn_;
        tran * parent_;
        ustr   id_;
        bool   commit_on_destruct_;
        bool   rollback_on_destruct_;
    };
  }; // end of ns:csl::db
}; // end of ns:csl

#endif // __cplusplus
#endif // _csl_db_tran_hh_included_
