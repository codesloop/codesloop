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

#include "tran.hh"
#include "_shared_impl.hh"

/**
  @file tran.cc
  @brief implementation of slt3::tran
 */

namespace csl
{
  namespace slt3
  {
    void tran::commit_on_destruct(bool yesno)   { impl_->commit_on_destruct(yesno);   }
    void tran::rollback_on_destruct(bool yesno) { impl_->rollback_on_destruct(yesno); }

    void tran::commit()   { impl_->commit();   }
    void tran::rollback() { impl_->rollback(); }

    void tran::use_exc(bool yesno) { impl_->use_exc(yesno); }
    bool tran::use_exc() { return impl_->use_exc(); }

    /* public interface */
    tran::tran(conn & c) : impl_(new impl(c.impl_)) {}
    tran::tran(tran & t) : impl_(new impl(t.impl_)) {}

    tran::~tran() {}

    /* private functions, copying not allowed */
    tran & tran::operator=(const tran & other) { return *this; }

    /* transactions should only be created in conn, or tran context */
    tran::tran() : impl_((impl *)0) { }
  };
};

/* EOF */
