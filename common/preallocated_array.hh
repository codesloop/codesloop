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

#ifndef _csl_common_preallocated_array_hh_included_
#define _csl_common_preallocated_array_hh_included_

/**
   @file preallocated_array.hh
   @brief Preallocated template buffer
 */

#include "codesloop/common/pbuf.hh"
#include "codesloop/common/hlprs.hh"
#include "codesloop/common/obj.hh"
#include "codesloop/common/common.h"
#ifdef __cplusplus

namespace csl
{
  namespace common
  {
    template <uint64_t SZ> class preallocated_array
    {
      public:
        enum { preallocated_size = SZ };

        inline ~preallocated_array() { reset(); }
        inline preallocated_array() : data_(preallocated_), size_(0) { }

        inline explicit preallocated_array(unsigned char c) : data_(preallocated_), size_(1)
        {
          preallocated_[0] = c;
        }

        inline explicit preallocated_array(wchar_t c) : data_(preallocated_), size_(sizeof(wchar_t))
        {
          const uint8_t * p = (reinterpret_cast<const uint8_t *>(&c));
          copy_n_uchars<sizeof(wchar_t)>(preallocated_,p);
        }

        inline preallocated_array(const preallocated_array & other) : data_(preallocated_), size_(0)
        {
          *this = other;
        }

        inline explicit preallocated_array(const char * other) : data_(preallocated_), size_(0)
        {
          *this = other;
        }

        inline bool operator==(const preallocated_array & other) const
        {
          if( other.size_ != size_ ) return false;
          if( size_ == 0 )           return true;
          if( data_ == 0 )           return false;
          if( other.data_ == 0 )     return false;
          return (::memcmp( other.data_, data_, static_cast<size_t>(size_) ) == 0);
        }

        inline preallocated_array & operator=(const char * other)
        {
          if( other ) set( reinterpret_cast<const uint8_t *>(other), (::strlen(other)+1) );
          return *this;
        }

        inline preallocated_array & operator=(const pbuf & other)
        {
          uint64_t sz = other.size();

          /* quick return if empty */
          if( !sz ) { reset(); return *this; }

          uint8_t * tmp = allocate(sz);

          if( tmp ) other.copy_to(tmp);

          return *this;
        }

        inline preallocated_array & operator=(const preallocated_array & other)
        {
          /* return immediately if they are the same */
          if( &other == this || other.data_ == data_ )
          {
            return *this;
          }

          /* quick return if empty */
          if( other.is_empty() ) { reset(); return *this; }

          uint8_t * tmp = allocate_nocopy( other.size_ );

          if( tmp )
          {
            ::memcpy( tmp, other.data_, static_cast<size_t>(other.size_) );
          }
          return *this;
        }

        void reset();

        inline bool get(uint8_t * dta)
        {
          if( !dta || !size_ || !data_ ) { return false; }
          ::memcpy( dta,data_,static_cast<size_t>(size_) );
          return true;
        }

        inline bool set(const uint8_t * dta, uint64_t sz)
        {
          /* if no data on the other side we are done */
          if( !sz )  { reset(); return true; }

          /* if sz is not zero than dta must not be null */
          if( !dta ) { return false; }

          if( allocate(sz) )
          {
            /* copy in the data */
            ::memcpy( data_, dta, static_cast<size_t>(sz) );
            return true;
          }
          else
          {
            return false;
          }
        }

        uint8_t * allocate(uint64_t sz);

        inline uint8_t * allocate_nocopy(uint64_t sz)
        {
          if( !sz ) { reset(); return data_; }

          if( sz <= size_ )
          {
            /* the requested data is smaller than the allocated */
            size_ = sz;
            return data_;
          }
          else if( sz <= sizeof(preallocated_) )
          {
            /* data fits into preallocated size */
            if( size_ > 0 && data_ != preallocated_ ) ::free( data_ );

            data_ = preallocated_;
            size_ = sz;
            return data_;
          }
          else
          {
            /* cannot use the preallocated space */
            uint8_t * tmp =
              reinterpret_cast<uint8_t *>(::malloc( static_cast<size_t>(sz) ));

            if( !tmp ) return 0;

            /* already have data ? */
            if( size_ > 0 && data_ != preallocated_ ) ::free( data_ );

            data_ = tmp;
            size_ = sz;
            return data_;
          }
        }

        inline void append(unsigned char c)
        {
          set_at(size_,c);
        }

        inline bool append(const uint8_t * dta, uint64_t sz)
        {
          /* if no data on the other side we are done */
          if( !sz )  { return true; }

          /* if sz is not zero than dta must not be null */
          if( !dta ) { return false; }

          if( allocate(size_+sz) )
          {
            /* copy in the data */
            ::memcpy( data_+size_-sz, dta, static_cast<size_t>(sz) );
            return true;
          }
          else
          {
            return false;
          }
        }

        inline bool append(const preallocated_array & other)
        {
          return append( other.data(), other.size() );
        }

        inline void set_at(uint64_t pos,unsigned char c)
        {
          uint8_t * t = data_;
          if( pos >= size_ ) t = allocate( pos+1 );
          t[pos] = c;
        }

        /* inline functions */
        inline bool is_empty() const        { return (size_ == 0); }
        inline bool has_data() const        { return (size_ > 0); }
        inline bool is_static() const       { return (data_ == preallocated_); }

        inline uint64_t size() const    { return size_; }
        inline const uint8_t * data() const { return data_; }
        inline uint8_t * private_data() const { return data_; }

      private:
        uint8_t            preallocated_[SZ];
        uint8_t *          data_;
        uint64_t           size_;
    };
  }
}

#endif /* __cplusplus */
#include "codesloop/common/preallocated_array_impl.hh"
#endif /* _csl_common_preallocated_array_hh_included_ */
