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

#include "pbuf.hh"
#include "arch.hh"

/**
   @file pbuf.cc
   @brief Paged buffer management
 */

namespace csl
{
  namespace common
  {
    bool pbuf::append(const unsigned char * dta, unsigned int sz)
    {
      if( !sz )  return true;
      if( !dta ) return false;

      buf * p = 0;
      while( (p=allocate(sz)) != 0 )
      {
        unsigned int fsp = p->free_space();
        if( fsp > sz ) { fsp = sz; }

        ::memcpy(p->data_here(),dta,fsp);
        p->size_ += fsp;
        dta      += fsp;
        sz       -= fsp;
        size_    += fsp;
      }

      if( sz > 0 ) return false;
      else         return true;
    }

    bool pbuf::copy_to(unsigned char * ptr, unsigned int max_size) const
    {
      if( !ptr ) return false;
      const_iterator it(begin());
      const_iterator ee(end());

      if( !max_size ) max_size = size();

      for( ;it!=ee;++it )
      {
        const buf * bp = *it;
        if( bp->size_ && bp->data_ )
        {
          if( max_size >= bp->size_ )
          {
            ::memcpy( ptr,bp->data_,bp->size_ );
            ptr += bp->size_;
            max_size -= bp->size_;
          }
          else if( max_size == 0 ) { break; }
          else
          {
            ::memcpy( ptr,bp->data_,max_size );
            break;
          }
        }
      }
      return true;
    }

    pbuf::buf * pbuf::allocate(unsigned int sz)
    {
      if( !sz ) { return 0; }

      bufpool_t::iterator it = bufpool_.last();

      if( *it == 0 )
      {
        /* no page has allocated so far */
        buf * ret  = new buf();
        ret->data_ = preallocated_;
        ret->size_ = 0;

        bufpool_.push_back(ret);
        return ret;
      }
      else if( (*it)->free_space() > 0 )
      {
        /* have space in the last buffer */
        return *it;
      }
      else
      {
        buf * ret  = new buf();
        ret->data_ = reinterpret_cast<unsigned char *>(pool_.allocate(buf_size));
        ret->size_ = 0;

        bufpool_.push_back(ret);
        return ret;
      }
    }

    pbuf::pbuf() : size_(0) {}

    pbuf::pbuf(const pbuf & other)
    {
      *this = other;
    }

    pbuf & pbuf::operator=(const pbuf & other)
    {
      this->free_all();
      pbuf::const_iterator it(other.begin());
      pbuf::const_iterator e(other.end());

      for( ;it!=e;++it )
      {
        this->append( (*it)->data_, (*it)->size_ );
      }
      return *this;
    }

    bool pbuf::operator==(const pbuf & other) const
    {
      if( size_ != other.size_ ) return false;

      pbuf::const_iterator it(other.begin());
      pbuf::const_iterator e(other.end());

      pbuf::const_iterator thit(this->begin());
      pbuf::const_iterator thend(this->end());

      for( ;it!=e;++it )
      {
        const buf * lhbuf = (*thit);
        const buf * rhbuf = (*it);
        if( !lhbuf || !rhbuf ) return false;
        if( lhbuf->size_ != rhbuf->size_ ) return false;
        if( ::memcmp(lhbuf->data_,rhbuf->data_,lhbuf->size_) != 0 ) return false;
        ++thit;
      }
      if( thit != thend ) return false;
      return true;
    }

    void pbuf::serialize(arch & ar)
    {
      ar.serialize( *this );
    }
  }
}

/* EOF */
