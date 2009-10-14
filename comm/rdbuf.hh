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

#ifndef _csl_comm_rdbuf_hh_included_
#define _csl_comm_rdbuf_hh_included_

#include "codesloop/comm/read_res.hh"
#include "codesloop/common/tbuf.hh"
#include "codesloop/common/common.h"
#include "codesloop/common/logger.hh"
#include "codesloop/common/obj.hh"

#ifdef __cplusplus

namespace csl
{
  namespace comm
  {
    template <uint64_t Preallocated=1024,uint64_t MaxSize=256*1024>
    class rdbuf
    {
#ifndef RDBUF_DEBUG_STATE
#define RDBUF_DEBUG_STATE(WHICH) \
  CSL_DEBUGF(L"%s : [PreAll:%d Max:%d start_:%lld len_:%lld buf_.size():%lld]", \
             WHICH, \
             Preallocated, \
             MaxSize, \
             start_, \
             len_, \
             buf_.size() );
#endif /*RDBUF_DEBUG_STATE*/

#ifndef RDBUF_DEBUG_STATE_RR
#define RDBUF_DEBUG_STATE_RR(WHICH,RR) \
  CSL_DEBUGF(L"%s : [PreAll:%d Max:%d start_:%lld len_:%lld buf_.size():%lld |RR data:%p bytes:%lld timed_out:%s failed:%s]", \
             WHICH, \
             Preallocated, \
             MaxSize, \
             start_, \
             len_, \
             buf_.size(), \
             (RR).data(), \
             (RR).bytes(), \
             ((RR).timed_out() == true ? "TRUE":"FALSE"), \
             ((RR).failed() == true ? "TRUE":"FALSE") \
             );
#endif /*RDBUF_DEBUG_STATE_RR*/


      public:
        read_res & get( uint64_t sz, read_res & rr )
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"get(sz:%lld,rr)",sz);
          RDBUF_DEBUG_STATE("old state");
          rr.reset();

          if( sz == 0 )
          {
            CSL_DEBUGF(L"invalid size [sz:0]");
            rr.failed( true );
          }
          else if( len_ > 0 )
          {
            uint64_t ret_size = (len_ > sz ? sz : len_);
            rr.data( buf_.get_private_data() + start_ );
            rr.bytes( ret_size );
            start_ += ret_size;
            len_   -= ret_size;
            if( len_ == 0 ) start_ = 0;
          }
          else
          {
            // this is not an error: empty buffer
            CSL_DEBUGF(L"no data to be returned");
          }
          RDBUF_DEBUG_STATE_RR("new state",rr);
          RETURN_FUNCTION( rr );
        }

        // there is an important design decision here: data is always allocated at
        // the end of the buffer. even if space is available at the beginning of
        // the buffer, it can only be used if all data is returned by get().
        // this behaviour forces the application to care about the buffer, thus
        // not not enforcing unneccessary and time consuming memory copies.
        read_res & reserve( uint64_t sz, read_res & rr )
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"resrerve(sz:%lld,rr)",sz);
          RDBUF_DEBUG_STATE("old state");
          rr.reset();

          uint64_t new_len = start_ + len_ + sz;
          if( new_len > MaxSize )
          {
            CSL_DEBUGF("cannot allocate %lld bytes",sz);
            if( start_ + len_ < MaxSize )
            {
              CSL_DEBUGF("allocate additional %lld bytes instead of the requested %lld bytes [max:%lld-start_:%lld-len_:%lld]",
                         MaxSize-start_-len_,
                         sz,
                         start_,
                         len_);
              uint8_t * ptr = buf_.allocate( MaxSize );
              rr.data( ptr + start_ + len_ );
              rr.bytes( MaxSize - start_ - len_ );
              len_ = MaxSize - start_;
            }
            else
            {
              // we are at maximum capacity already: this is an error
              CSL_DEBUGF("cannot allocate more data");
              rr.failed( true );
            }
          }
          else
          {
            CSL_DEBUGF("allocating %lld bytes",sz);
            uint8_t * ptr = buf_.allocate( MaxSize );
            rr.data( ptr + start_ + len_ );
            rr.bytes( sz );
            len_ += sz;
          }
          RDBUF_DEBUG_STATE_RR("new state",rr);
          RETURN_FUNCTION( rr );
        }

        read_res & adjust( read_res & rr, uint64_t n_succeed )
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"adjust(rr,n_succeed:%lld)",n_succeed);
          RDBUF_DEBUG_STATE_RR("old state",rr);
          RDBUF_DEBUG_STATE_RR("new state",rr);
          RETURN_FUNCTION( rr );
        }

      private:
        common::tbuf<Preallocated> buf_;
        uint64_t start_;
        uint64_t len_;

        CSL_OBJ(csl::comm,rdbuf);
        USE_EXC();
    };
  } /* end of ns:csl:comm */
} /* end of ns:csl */

#endif /* __cplusplus */
#endif /* _csl_comm_rdbuf_hh_included_ */
