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

#ifndef _csl_common_tbuf_hh_included_
#define _csl_common_tbuf_hh_included_

/**
   @file tbuf.hh
   @brief Preallocated template buffer
   @todo document me
 */

#include "pbuf.hh"
#include <stdlib.h>
#include <string.h>
#ifdef __cplusplus

namespace csl
{
  namespace common
  {
    /** @todo document me */
    template <int SZ>
    class tbuf
    {
      public:
        enum { preallocated_size = SZ };

        inline tbuf() : data_(preallocated_), size_(0) { }
        inline ~tbuf() { reset(); }

        inline tbuf(const tbuf & other) : data_(preallocated_), size_(0) // TODO test copy constructor!!!
        {
          *this = other;
        }

        inline bool operator==(const tbuf & other) const
        {
          if( other.size_ != size_ ) return false;
          if( size_ == 0 )           return true;
          if( data_ == 0 )           return false;
          if( other.data_ == 0 )     return false;
          return (::memcmp(other.data_,data_,size_) == 0);
        }

        inline tbuf & operator=(const pbuf & other)
        {
          unsigned long sz = other.size();

          /* quick return if empty */
          if( !sz ) { reset(); return *this; }

          unsigned char * tmp = allocate(sz);

          if( tmp ) other.copy_to(tmp);

          return *this;
        }

        inline tbuf & operator=(const tbuf & other) // TODO test copy operator!!!
        {
          /* return immediately if they are the same */
          if( &other == this || other.data_ == data_ )
          {
            return *this;
          }

          /* quick return if empty */
          if( other.is_empty() ) { reset(); return *this; }

          unsigned char * tmp = allocate_nocopy( other.size_ );

          if( tmp )
          {
            ::memcpy( tmp, other.data_, other.size_ );
          }
          return *this;
        }

        inline void reset()
        {
          if( data_ && data_ != preallocated_ )
          {
              ::free( data_ );
              data_ = preallocated_;
          }
          size_ = 0;
        }

        inline bool get(unsigned char * dta)
        {
          if( !dta || !size_ || !data_ ) { return false; }
            ::memcpy( dta,data_,size_ );
            return true;
        }

        inline bool set(const unsigned char * dta,unsigned int sz)
        {
          /* if no data on the other side we are done */
          if( !sz )  { reset(); return true; }

          /* if sz is not zero than dta must not be null */
          if( !dta ) { return false; }

          if( allocate(sz) )
          {
            /* copy in the data */
            ::memcpy(data_,dta,sz);
            return true;
          }
          else
          {
            return false;
          }
        }

        /** @todo test: allocate with existing data: should copy the old data!!! */
        inline unsigned char * allocate(unsigned int sz)
        {
          if( !sz ) { reset(); return data_; }

          if( sz <= size_ )
          {
            /* the requested data is smaller than the allocated */
            size_ = sz;
            return data_;
          }
          else if( sz <= sizeof(preallocated_) && data_ == preallocated_ )
          {
            /* data fits into preallocated size */
            size_ = sz;
            return data_;
          }
          else
          {
            /* cannot use the preallocated space */
            unsigned char * tmp = (unsigned char *)::malloc(sz);
            if( !tmp ) return 0;

            /* already have data ? */
            if( size_ > 0 )
            {
                ::memcpy(tmp,data_,size_);
                if( data_ != preallocated_ ) ::free(data_);
            }

            data_ = tmp;
            size_ = sz;

            return data_;
          }
        }

        inline unsigned char * allocate_nocopy(unsigned int sz)
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
            if( size_ > 0 && data_ != preallocated_ ) ::free(data_);

            data_ = preallocated_;
            size_ = sz;
            return data_;
          }
          else
          {
            /* cannot use the preallocated space */
            unsigned char * tmp = (unsigned char *)::malloc(sz);
            if( !tmp ) return 0;

            /* already have data ? */
            if( size_ > 0 && data_ != preallocated_ ) ::free(data_);

            data_ = tmp;
            size_ = sz;
            return data_;
          }
        }

        /** @todo return value */
        inline void append(unsigned char c)
        {
          set_at(size_,c);
        }

        /** @todo test  */
        inline bool append(const unsigned char * dta,unsigned int sz)
        {
          /* if no data on the other side we are done */
          if( !sz )  { return true; }

          /* if sz is not zero than dta must not be null */
          if( !dta ) { return false; }

          if( allocate(size_+sz) )
          {
            /* copy in the data */
            ::memcpy(data_+size_-sz,dta,sz);
            return true;
          }
          else
          {
            return false;
          }
        }

        inline bool append(const tbuf & other)
        {
          return append( other.data(), other.size() );
        }

        /** @todo return value */
        inline void set_at(unsigned int pos,unsigned char c)
        {
          unsigned char * t = allocate(pos+1);
          t[pos] = c;
        }

        /* inline functions */
        inline bool is_empty() const        { return (size_ == 0); }             ///<checks if empty
        inline bool has_data() const        { return (size_ > 0); }              ///<checks if has_data
        inline bool is_static() const       { return (data_ == preallocated_); } ///<checks if statically allocated

        /** @brief returns the size of the allocated data */
        inline unsigned int size() const    { return size_; }

        /** @brief return the allocated data */
        inline const unsigned char * data() const { return data_; }

        /** @brief return the internal data pointer */
        inline unsigned char * private_data() const { return data_; }

      private:
        unsigned char   preallocated_[SZ];   ///<the preallocated buffer
        unsigned char * data_;               ///<the data
        unsigned int    size_;               ///<the allocated size
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_common_tbuf_hh_included_ */
