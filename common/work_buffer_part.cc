/*
Copyright (c) 2008,2009,2010, CodeSLoop Team

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

#if 0
#ifndef DEBUG
#define DEBUG
#define DEBUG_ENABLE_INDENT
//#define DEBUG_VERBOSE
#endif /* DEBUG */
#endif /* 0 */

#include "codesloop/common/work_buffer_part.hh"
#include "codesloop/common/logger.hh"

namespace csl
{
  namespace common
  {
    work_buffer_part::work_buffer_part()
        : data_(0), bytes_(0), timed_out_(false), failed_(false) { }

    void work_buffer_part::reset()
    {
      ENTER_FUNCTION();
      data_       = 0;
      bytes_      = 0;
      timed_out_  = false;
      failed_     = false;
      LEAVE_FUNCTION();
    }

    uint8_t * work_buffer_part::data() const
    {
      ENTER_FUNCTION();
      RETURN_FUNCTION( data_ );
    }

    void work_buffer_part::data(uint8_t * d)
    {
      ENTER_FUNCTION();
      CSL_DEBUGF(L"data(%p)",d );
      data_ = d;
      LEAVE_FUNCTION();
    }

    uint64_t work_buffer_part::bytes() const
    {
      ENTER_FUNCTION();
      RETURN_FUNCTION( bytes_ );
    }

    void work_buffer_part::bytes(uint64_t b)
    {
      ENTER_FUNCTION();
      CSL_DEBUGF(L"bytes(%lld)",b );
      bytes_ = b;
      LEAVE_FUNCTION();
    }

    bool work_buffer_part::timed_out() const
    {
      ENTER_FUNCTION();
      RETURN_FUNCTION( timed_out_ );
    }

    void work_buffer_part::timed_out(bool to)
    {
      ENTER_FUNCTION();
      CSL_DEBUGF(L"timed_out(%s)",(to==true?"TRUE":"FALSE") );
      timed_out_ = to;
      LEAVE_FUNCTION();
    }

    bool work_buffer_part::failed() const
    {
      ENTER_FUNCTION();
      RETURN_FUNCTION( failed_ );
    }

    void work_buffer_part::failed(bool fd)
    {
      ENTER_FUNCTION();
      CSL_DEBUGF(L"failed(%s)",(fd==true?"TRUE":"FALSE") );
      failed_ = fd;
      LEAVE_FUNCTION();
    }
  } /* end of ns: csl::common */
} /* end of ns: csl */

/* EOF */
