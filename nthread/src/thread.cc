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

#include "thread.hh"
#include "common.h"
#include <string>

#ifdef WIN32
# include "thread_impl_windows.cc"
#else /* NOT WIN32 */
# include "thread_impl_pthread.cc"
#endif /* WIN32 */

/**
  @file thread.cc
  @brief implementation of thread
*/

namespace csl
{
  namespace nthread
  {
    thread::thread() : impl_(new impl) {}
    thread::~thread() {}

    void thread::set_entry(callback & entry)
    {
      impl_->set_entry(entry);
    }

    void thread::set_stack_size(unsigned long sz)
    {
      impl_->set_stack_size(sz);
    }

    unsigned long thread::get_stack_size()
    {
      return impl_->get_stack_size();
    }

    bool thread::start()
    {
      return impl_->start();
    }

    bool thread::stop()
    {
      return impl_->stop();
    }

    void thread::cancel_here()
    {
#ifndef WIN32
      pthread_testcancel();
#endif
    }

    pevent & thread::start_event()
    {
      return impl_->start_event();
    }

    pevent & thread::exit_event()
    {
      return impl_->exit_event();
    }

    bool thread::is_started()
    {
      return impl_->is_started();
    }

    bool thread::is_running()
    {
      return impl_->is_running();
    }

    bool thread::is_exited()
    {
      return impl_->is_exited();
    }

    // no-copy
    thread::thread(const thread & other) 
      : impl_((impl *)0) {throw std::string("should never be called"); }

    thread & thread::operator=(const thread & other) { return *this; }
  }
}

/* EOF */
