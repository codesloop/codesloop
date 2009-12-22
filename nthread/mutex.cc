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

#include "codesloop/nthread/mutex.hh"
#include "codesloop/nthread/exc.hh"
#include "codesloop/common/str.hh"

#ifdef WIN32
# include "mutex_impl_windows.cc"
#else /* NOT WIN32 */
# include "mutex_impl_pthread.cc"
#endif /* WIN32 */

/**
  @file mutex.cc
  @brief implementation of mutex
*/

namespace csl
{
  namespace nthread
  {
    mutex::mutex() : impl_(new impl), use_exc_(true) {}
    mutex::~mutex() {}

    // mutex operations
    bool mutex::lock(unsigned long timeout_ms)
    {
      return impl_->lock(timeout_ms);
    }

    bool mutex::try_lock()
    {
      return impl_->try_lock();
    }

    bool mutex::unlock()
    {
      return impl_->unlock();
    }

    bool mutex::recursive_unlock()
    {
      return impl_->recursive_unlock();
    }

    bool mutex::is_locked()
    {
      return impl_->is_locked();
    }

    unsigned int mutex::waiting_count()
    {
      return impl_->waiting_count();
    }

    unsigned int mutex::locked_count()
    {
      return impl_->locked_count();
    }

    // no-copy
    mutex::mutex(const mutex & other) : impl_( reinterpret_cast<impl *>(0) )
    {
      THRNORET(exc::rs_not_implemented);
    }

    mutex & mutex::operator=(const mutex & other)
    {
      THR(exc::rs_not_implemented, *this);
      return *this;
    }
  }
}

/* EOF */
