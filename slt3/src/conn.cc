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

#include "conn.hh"
#include "_shared_impl.hh"

/**
  @file conn.cc
  @brief implementation of sqlite3 conn
 */

namespace csl
{
  namespace slt3
  {
    long long conn::last_insert_id() { return impl_->last_insert_id(); }
    long long conn::change_count() { return impl_->change_count(); }

    bool conn::open(const char * db) { return impl_->open(db); }
    bool conn::close() { return impl_->close(); }

    void conn::use_exc(bool yesno) { impl_->use_exc(yesno); }
    bool conn::use_exc() { return impl_->use_exc(); }
    const std::string & conn::name() const { return impl_->name(); }

    /* public interface */
    conn::conn() : impl_(new impl) { }
    conn::~conn() {}

    /* private functions, copying not allowed */
    conn::conn(const conn & other) : impl_((impl *)0) { }
    conn & conn::operator=(const conn & other) { return *this; }
  };
};

/* EOF */
