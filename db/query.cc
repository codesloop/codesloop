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

#include "codesloop/db/query.hh"
#include "codesloop/db/_shared_impl.hh"
#include "codesloop/common/str.hh"
#include "codesloop/common/ustr.hh"

/**
  @file query.cc
  @brief implementation of slt3::query
 */

using csl::common::str;
using csl::common::ustr;

namespace csl
{
  namespace slt3
  {
    void query::colhead::debug()
    {
      PRINTF(L"Colhead: Name='%s' Table='%s' DB='%s' Origin='%s' Type=%d\n",
             (name_ ? name_ : "null"),
             (table_ ? table_ : "null"),
             (db_ ? db_ : "null"),
             (origin_ ? origin_ : "null"),
             type_ );
    }

    long long query::last_insert_id() { return impl_->last_insert_id(); }
    long long query::change_count() { return impl_->change_count(); }
    void query::debug() { impl_->debug(); }    
    common::int64 & query::int64_param(unsigned int pos) { return impl_->int64_param(pos); }
    common::dbl & query::dbl_param(unsigned int pos) { return impl_->dbl_param(pos); }
    common::ustr & query::ustr_param(unsigned int pos) { return impl_->ustr_param(pos); }
    common::binry & query::binry_param(unsigned int pos) { return impl_->binry_param(pos); }
    common::var & query::set_param(unsigned int pos,const common::var & p) { return impl_->set_param(pos,p); }
    void query::clear_params() { impl_->clear_params(); }
    bool query::prepare(const char * sql) { return impl_->prepare(sql); }
    bool query::reset()  { return impl_->reset(); }
    void query::reset_data()  { impl_->reset_data(); }
    void query::autoreset_data(bool yesno) { impl_->autoreset_data(yesno); }
    bool query::autoreset_data() { return impl_->autoreset_data(); }
    bool query::next(columns_t & cols, fields_t & fields) { return impl_->next(cols,fields); }
    bool query::next() { return impl_->next(); }
    bool query::execute(const char * sql) { return impl_->execute(sql); }
    bool query::execute(const char * sql, common::ustr & result)  { return impl_->execute(sql, result); }

    void query::use_exc(bool yesno) { impl_->use_exc(yesno); }
    bool query::use_exc() { return impl_->use_exc(); }

    /* public interface */
    query::query(tran & t) : impl_(new impl(t.impl_)) {}
    query::~query() {}

    /* private functions, copying not allowed */
    query::query(const query & other) : impl_( reinterpret_cast<impl *>(0) ) { }
    query & query::operator=(const query & other) { return *this; }

    /* no deafault constructor */
    query::query() : impl_( reinterpret_cast<impl *>(0) ) {}
  };
};

/* EOF */
