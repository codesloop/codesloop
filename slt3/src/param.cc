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
#include "str.hh"
#include "ustr.hh"
#include "_shared_impl.hh"

/**
  @file param.cc
  @brief implementation of slt3::param
 */

using csl::common::str;
using csl::common::ustr;

namespace csl
{
  namespace slt3
  {
    void param::debug() { impl_->debug(); }
    int param::get_type() const { return impl_->get_type(); }
    unsigned int param::get_size() const { return impl_->get_size(); }
    void * param::get_ptr() const { return impl_->get_ptr(); }
    long long param::get_long() const { return impl_->get_long(); }
    double param::get_double() const { return impl_->get_double(); }
    const char * param::get_string() const { return impl_->get_string(); }
    bool param::is_empty() { return impl_->is_empty(); }

    bool param::get(long long & val) const { return impl_->get(val); }
    bool param::get(double & val) const { return impl_->get(val); }
    bool param::get(common::str & val) const { return impl_->get(val); }
    bool param::get(common::ustr & val) const { return impl_->get(val); }
    bool param::get(blob_t & val) const { return impl_->get(val); }

    void param::set(long long val) { impl_->set(val); }
    void param::set(double val) { impl_->set(val); }
    void param::set(const common::str & val) { impl_->set(val); }
    void param::set(const common::ustr & val) { impl_->set(val); }
    void param::set(const char * val) { impl_->set(val); }
    void param::set(const wchar_t * val) { impl_->set(val); }
    void param::set(const blob_t & val) { impl_->set(val); }
    void param::set(const unsigned char * ptr,unsigned int size) { impl_->set(ptr,size); }

    void param::use_exc(bool yesno) { impl_->use_exc(yesno); }
    bool param::use_exc() { return impl_->use_exc(); }

    /* public interface */
    param::param(synqry::impl & sq) : impl_(new impl(sq))    { }
    param::~param() {}

    /* private functions, copying not allowed */
    param::param(const param & other) : impl_((impl *)0) { }
    param & param::operator=(const param & other) { return *this; }

    /* no default construction */
    param::param() : impl_((impl *)0) {}
  };
};

/* EOF */
