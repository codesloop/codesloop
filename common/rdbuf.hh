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

#ifndef _csl_common_rdbuf_hh_included_
#define _csl_common_rdbuf_hh_included_

#include "codesloop/common/read_res.hh"
#include "codesloop/common/exc.hh"
#include "codesloop/common/tbuf.hh"
#include "codesloop/common/common.h"
#include "codesloop/common/logger.hh"
#include "codesloop/common/obj.hh"

#ifdef __cplusplus

namespace csl
{
  namespace common
  {
    template <uint64_t Preallocated=1024,uint64_t MaxSize=256*1024>
    class rdbuf
    {
#ifndef RDBUF_DEBUG_STATE
#define RDBUF_DEBUG_STATE(WHICH) \
  CSL_DEBUGF(L"%s : [PreAll:%lld Max:%lld start_:%lld len_:%lld buf_.size():%lld]", \
             WHICH, \
             Preallocated, \
             MaxSize, \
             start_, \
             len_, \
             buf_.size() );
#endif /*RDBUF_DEBUG_STATE*/

#ifndef RDBUF_DEBUG_STATE_RR
#define RDBUF_DEBUG_STATE_RR(WHICH,RR) \
  CSL_DEBUGF(L"%s : [PreAll:%lld Max:%lld start_:%lld len_:%lld buf_.size():%lld " \
              "|RR data:%p bytes:%lld timed_out:%s failed:%s]", \
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

#ifndef RDBUF_DEBUG_ASSERT
# ifndef RDBUF_ASSERT_TESTING
#  define RDBUF_DEBUG_ASSERT( COND, RETVAL ) CSL_DEBUG_ASSERT( COND )
# else /* */
#  define RDBUF_DEBUG_ASSERT( COND, RETVAL ) \
     if( !(COND) ) \
     { \
       THRR( csl::common::exc::rs_assert,L"assert failed: "#COND,RETVAL ); \
     }
# endif
#endif

      public:
        static const uint64_t preallocated_size_ = Preallocated;
        static const uint64_t max_size_          = MaxSize;

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
            rr.data( buf_.private_data() + start_ );
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
            CSL_DEBUGF(L"cannot allocate %lld bytes",sz);
            if( start_ + len_ < MaxSize )
            {
              CSL_DEBUGF(L"allocate additional %lld bytes instead of the "
                         "requested %lld bytes [max:%lld-start_:%lld-len_:%lld]",
                         n_free(),
                         sz,
                         start_,
                         len_);
              uint8_t * ptr = buf_.allocate( MaxSize );
              rr.data( ptr + start_ + len_ );
              rr.bytes( n_free() );
              len_ = MaxSize - start_;
            }
            else
            {
              // we are at maximum capacity already: this is an error
              CSL_DEBUGF(L"cannot allocate more data");
              rr.failed( true );
            }
          }
          else if( sz == 0 )
          {
            CSL_DEBUGF(L"not allocating");
            rr.reset(); // this is to enforce errors
          }
          else
          {
            CSL_DEBUGF(L"allocating %lld bytes",sz);
            uint8_t * ptr = buf_.allocate( len_+sz );
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

          uint64_t start_offset = 0;
          uint64_t adjust_len   = 0;

          start_offset = rr.data() - buf_.private_data();

          // sanity checks
          RDBUF_DEBUG_ASSERT( (rr.bytes() >= n_succeed), rr );
          RDBUF_DEBUG_ASSERT( (rr.data() != NULL), rr );
          RDBUF_DEBUG_ASSERT( (start_offset == (start_+len_-rr.bytes())), rr );
          RDBUF_DEBUG_ASSERT( (len_ >= rr.bytes()), rr );
          RDBUF_DEBUG_ASSERT( (rr.failed() == false), rr );
          RDBUF_DEBUG_ASSERT( (rr.timed_out() == false), rr );

          if( rr.bytes() < n_succeed ||
              rr.data() == NULL      ||
              rr.failed() == true    ||
              len_ < rr.bytes()      ||
              start_offset !=  (start_+len_-rr.bytes()) )
          {
            CSL_DEBUGF(L"invalid param received");
            goto bail;
          }

          {
            // set rr
            adjust_len = rr.bytes() - n_succeed;
            rr.bytes( n_succeed );
            if( !n_succeed ) rr.data( NULL );
          }

          {
            // set internal data
            len_ -= adjust_len;
            if( len_ == 0 )           { start_ = 0; buf_.allocate(0); }
            else if( adjust_len > 0 )
            {
              uint8_t * p = buf_.allocate( len_ + start_ );
              if( n_succeed > 0 ) { rr.data( p + start_offset ); }
            }
          }

          CSL_DEBUGF(L"length decreased by: %lld bytes",adjust_len);
          RDBUF_DEBUG_STATE_RR("new state",rr);
        bail:
          RETURN_FUNCTION( rr );
        }

        uint64_t start()  const { return start_;      }
        uint64_t len()    const { return len_;        }
        uint64_t buflen() const { return buf_.size(); }
        uint64_t n_free() const { return (MaxSize-len_-start_); }

        rdbuf() : start_(0), len_(0), use_exc_(true) {}

      private:
        common::tbuf<Preallocated> buf_;
        uint64_t start_;
        uint64_t len_;

        CSL_OBJ(csl::common,rdbuf);
        USE_EXC();
    };
  } /* end of ns:csl:common */
} /* end of ns:csl */

#endif /* __cplusplus */
#endif /* _csl_common_rdbuf_hh_included_ */
