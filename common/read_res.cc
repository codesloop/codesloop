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

#include "codesloop/common/read_res.hh"
#include "codesloop/common/logger.hh"

namespace csl
{
  namespace common
  {
#ifndef READ_RES_DEBUG_STATE
#define READ_RES_DEBUG_STATE(WHICH) \
  CSL_DEBUGF(L"%s : [data_:%p bytes_:%lld timed_out_:%s failed_:%s]", \
             WHICH, \
             data_, \
             bytes_, \
            (timed_out_==true?"TRUE":"FALSE"), \
            (failed_==true?"TRUE":"FALSE") );
#endif /*READ_RES_DEBUG_STATE */


    read_res::read_res() : data_(0), bytes_(0), timed_out_(false), failed_(false) { }

    void read_res::reset()
    {
      ENTER_FUNCTION();
      READ_RES_DEBUG_STATE("reset() : old");
      data_       = 0;
      bytes_      = 0;
      timed_out_  = false;
      failed_     = false;
      READ_RES_DEBUG_STATE("new");
      LEAVE_FUNCTION();
    }

    uint8_t * read_res::data() const
    {
      ENTER_FUNCTION();
      READ_RES_DEBUG_STATE("data() : values");
      RETURN_FUNCTION( data_ );
    }

    void read_res::data(uint8_t * d)
    {
      ENTER_FUNCTION();
      CSL_DEBUGF(L"data(%p)",d );
      READ_RES_DEBUG_STATE("old");
      data_ = d;
      READ_RES_DEBUG_STATE("new");
      LEAVE_FUNCTION();
    }

    uint64_t read_res::bytes() const
    {
      ENTER_FUNCTION();
      READ_RES_DEBUG_STATE("bytes() : values");
      RETURN_FUNCTION( bytes_ );
    }

    void read_res::bytes(uint64_t b)
    {
      ENTER_FUNCTION();
      CSL_DEBUGF(L"bytes(%lld)",b );
      READ_RES_DEBUG_STATE("old");
      bytes_ = b;
      READ_RES_DEBUG_STATE("new");
      LEAVE_FUNCTION();
    }

    bool read_res::timed_out() const
    {
      ENTER_FUNCTION();
      READ_RES_DEBUG_STATE("timed_out() : values");
      RETURN_FUNCTION( timed_out_ );
    }

    void read_res::timed_out(bool to)
    {
      ENTER_FUNCTION();
      CSL_DEBUGF(L"timed_out(%s)",(to==true?"TRUE":"FALSE") );
      READ_RES_DEBUG_STATE("old");
      timed_out_ = to;
      READ_RES_DEBUG_STATE("new");
      LEAVE_FUNCTION();
    }

    bool read_res::failed() const
    {
      ENTER_FUNCTION();
      READ_RES_DEBUG_STATE("failed() : values");
      RETURN_FUNCTION( failed_ );
    }

    void read_res::failed(bool fd)
    {
      ENTER_FUNCTION();
      CSL_DEBUGF(L"failed(%s)",(fd==true?"TRUE":"FALSE") );
      READ_RES_DEBUG_STATE("old");
      failed_ = fd;
      READ_RES_DEBUG_STATE("new");
      LEAVE_FUNCTION();
    }
  } /* end of ns: csl::common */
} /* end of ns: csl */

/* EOF */
