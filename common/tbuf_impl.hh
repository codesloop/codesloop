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

#ifndef _csl_common_tbuf_impl_hh_included_
#define _csl_common_tbuf_impl_hh_included_

/**
   @file tbuf_impl.hh
   @brief tbuf implementation should only be used in tbuf.hh
 */

#ifdef __cplusplus

namespace csl
{
  namespace common
  {
    template <uint64_t SZ>
    uint8_t * tbuf<SZ>::allocate(uint64_t sz)
    {
      // XXX solve circular deps here
      // ENTER_FUNCTION();
      // CSL_DEBUGF(L"allocate(sz:%lld)",sz);
      uint8_t * ret = data_;

      if( !sz )
      {
        // CSL_DEBUGF(L"resetting buffer, because of 0 size");
        reset();
      }
      else if( sz <= size_ )
      {
        // CSL_DEBUGF(L"not (re)allocating memory as there is enough already");
        size_ = sz;
      }
      else if( sz <= sizeof(preallocated_) && data_ == preallocated_ )
      {
        // CSL_DEBUGF(L"size fits into the preallocated block");
        size_ = sz;
      }
      else
      {
        // CSL_DEBUGF(L"need to allocate %lld bytes",sz);
        uint8_t * tmp = reinterpret_cast<uint8_t *>(::malloc( static_cast<size_t>(sz) ));
        // CSL_DEBUG_ASSERT( tmp != 0 );

        if( !tmp )
        {
          // CSL_DEBUGF(L"malloc failed");
          ret = 0;
        }
        else
        {
          if( size_ > 0 )
          {
            // CSL_DEBUGF(L"there was data in the previous buffer, must copy it");
            ::memcpy( tmp, data_, static_cast<size_t>(size_) );

            if( data_ != preallocated_ )
            {
              // CSL_DEBUGF(L"copied from dynamic buffer. must free that.");
              ::free( data_ );
            }
          }

          data_ = tmp;
          size_ = sz;
          ret = data_;
        }
      }
      // RETURN_FUNCTION( ret );
      return ( ret );
    }

    template <uint64_t SZ>
    void tbuf<SZ>::reset()
    {
      // XXX solve circular deps here
      // ENTER_FUNCTION();
      if( data_ && data_ != preallocated_ )
      {
        // CSL_DEBUGF(L"there is dynamic data allocated. need to free that.");
        ::free( data_ );
        data_ = preallocated_;
      }
      size_ = 0;
      // LEAVE_FUNCTION();
    }
  }
}

#endif /* __cplusplus */


#endif /* _csl_common_tbuf_impl_hh_included_ */
