/*
Copyright (c) 2008,2009, David Beck, Tamas Foldi

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

#include "exc.hh"
#include "pevent.hh"
#include "str.hh"
#include "common.h"

#ifdef WIN32
# include "pevent_impl_windows.cc"
#else /* NOT WIN32 */
# include "pevent_impl_pthread.cc"
#endif /* WIN32 */

/**
  @file pevent.cc
  @brief implementation of pevent
*/

namespace csl
{
  namespace nthread
  {
    pevent::pevent() : impl_(new impl) {}
    pevent::~pevent() {}

    bool pevent::notify(unsigned int n)
    {
      return impl_->notify(n);
    }

    bool pevent::notify_all()
    {
      return impl_->notify_all();
    }

    bool pevent::wait(unsigned long timeout_ms)
    {
      return impl_->wait(timeout_ms);
    }

    bool pevent::wait_nb()
    {
      return impl_->wait_nb();
    }

    unsigned int pevent::waiting_count()
    {
      return impl_->waiting_count();
    }

    unsigned int pevent::available_count()
    {
      return impl_->available_count();
    }

    void pevent::set_permanent()
    {
      impl_->set_permanent();
    }

    void pevent::clear_permanent()
    {
      impl_->clear_permanent();
    }

    bool pevent::is_permanent()
    {
      return impl_->is_permanent();
    }

    void pevent::clear_available()
    {
      impl_->clear_available();
    }

    // no-copy
    pevent::pevent(const pevent & other) 
      : impl_( reinterpret_cast<impl *>(0) ) { throw nthread::exc(exc::rs_not_implemented,L"csl::nthread::event"); }

    pevent & pevent::operator=(const pevent & other) { return *this; }
  }
}

/* EOF */
