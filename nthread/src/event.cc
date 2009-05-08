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

#include "event.hh"
#include "common.h"
#include <string>

#ifdef WIN32
# include "event_impl_windows.cc"
#else /* NOT WIN32 */
# include "event_impl_pthread.cc"
#endif /* WIN32 */

/**
  @file event.cc
  @brief implementation of event
*/

namespace csl
{
  namespace nthread
  {
    event::event() : impl_(new impl) {}
    event::~event() {}

    bool event::notify(unsigned int n)
    {
      return impl_->notify(n);
    }

    bool event::notify_all()
    {
      return impl_->notify_all();
    }

    bool event::wait(unsigned long timeout_ms)
    {
      return impl_->wait(timeout_ms);
    }

    bool event::wait_nb()
    {
      return impl_->wait_nb();
    }

    unsigned int event::waiting_count()
    {
      return impl_->waiting_count();
    }

    unsigned int event::available_count()
    {
      return impl_->available_count();
    }

    void event::clear_available()
    {
      impl_->clear_available();
    }

    // no-copy
    event::event(const event & other) 
      : impl_((impl *)0) {throw std::string("should never be called"); }

    event & event::operator=(const event & other) { return *this; }
  }
}

/* EOF */
