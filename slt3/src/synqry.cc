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

#include "param.hh"
#include "synqry.hh"
#include "_shared_impl.hh"
#include "str.hh"

/**
  @file synqry.cc
  @brief implementation of slt3::synqry
 */

namespace csl
{
  namespace slt3
  {
    void synqry::colhead::debug()
    {
      PRINTF(L"Colhead: Name='%sl' Table='%sl' DB='%sl' Origin='%sl' Type=%d\n",
             (name_ ? name_ : L"null"),
             (table_ ? table_ : L"null"),
             (db_ ? db_ : L"null"),
             (origin_ ? origin_ : L"null"),
             type_ );
    }

    long long synqry::last_insert_id() { return impl_->last_insert_id(); }
    long long synqry::change_count() { return impl_->change_count(); }
    void synqry::debug() { impl_->debug(); }
    param & synqry::get_param(unsigned int pos) { return impl_->get_param(pos); }
    void synqry::clear_params() { impl_->clear_params(); }
    bool synqry::prepare(const wchar_t * sql) { return impl_->prepare(sql); }
    bool synqry::reset()  { return impl_->reset(); }
    void synqry::reset_data()  { impl_->reset_data(); }
    void synqry::autoreset_data(bool yesno) { impl_->autoreset_data(yesno); }
    bool synqry::autoreset_data() { return impl_->autoreset_data(); }
    bool synqry::next(columns_t & cols, fields_t & fields) { return impl_->next(cols,fields); }
    bool synqry::next() { return impl_->next(); }
    bool synqry::execute(const wchar_t * sql) { return impl_->execute(sql); }
    bool synqry::execute(const wchar_t * sql, common::str & result)  { return impl_->execute(sql, result); }

    void synqry::use_exc(bool yesno) { impl_->use_exc(yesno); }
    bool synqry::use_exc() { return impl_->use_exc(); }

    /* public interface */
    synqry::synqry(tran & t) : impl_(new impl(t.impl_)) {}
    synqry::~synqry() {}

    /* private functions, copying not allowed */
    synqry::synqry(const synqry & other) : impl_((impl *)0) { }
    synqry & synqry::operator=(const synqry & other) { return *this; }

    /* no deafault constructor */
    synqry::synqry() : impl_((impl *)0) {}
  };
};

/* EOF */
