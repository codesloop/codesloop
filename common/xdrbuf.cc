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

#include "codesloop/common/xdrbuf.hh"
#include "codesloop/common/logger.hh"
#include "codesloop/common/common.h"
#include "codesloop/common/pbuf.hh"
#include "codesloop/common/exc.hh"
#include "codesloop/common/str.hh"
#include "codesloop/common/var.hh"
#include "codesloop/common/ustr.hh"
#include <memory>

/**
   @file xdrbuf.cc
   @brief XDR helper to reduce XDR dependecies
*/

namespace csl
{
  using csl::common::exc;

  namespace
  {
    void round_to_4(uint64_t sz,uint64_t & new_sz,uint64_t & pad)
    {
      new_sz = ((((sz) + 3) & (~3)));
      pad    = new_sz - sz;
    }

    void size_and_buf_to_pbuf(common::pbuf * b, const void * p, uint64_t sz)
    {
      if( !b || !p || !sz )
      {
        throw common::exc(exc::rs_cannot_append,L"",L"",L""__FILE__,__LINE__);
        return;
      }

      unsigned char pad[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
      uint64_t elen = htonll( sz ); 

      uint64_t new_len, pad_size;
      round_to_4( sz, new_len, pad_size );

      if( !b->append( reinterpret_cast<unsigned char *>(&elen),sizeof(elen)) )
      {
        throw common::exc(exc::rs_cannot_append,L"",L"",L""__FILE__,__LINE__);
        return;
      }

      if( !b->append( reinterpret_cast<const unsigned char *>(p), sz ) )
      {
        throw common::exc(exc::rs_cannot_append,L"",L"",L""__FILE__,__LINE__);
        return;
      }

      if( pad_size )
      {
        if( !b->append(pad,pad_size) )
        {
          throw common::exc(exc::rs_cannot_append,L"",L"",L""__FILE__,__LINE__);
          return;
        }
      }
    }
  };

  namespace common
  {
    xdrbuf & xdrbuf::operator<<(int32_t val)
    {
      int32_t v = htonl(val);

      if( !b_->append( reinterpret_cast<unsigned char *>(&v),sizeof(int32_t)) )
      {
        THR(exc::rs_cannot_append,*this);
      }

      return *this;
    }

    xdrbuf & xdrbuf::operator<<(uint32_t val)
    {
      uint32_t v = htonl(val);

      if( !b_->append( reinterpret_cast<unsigned char *>(&v),sizeof(uint32_t)) )
      {
        THR(exc::rs_cannot_append,*this);
      }

      return *this;
    }

    xdrbuf & xdrbuf::operator<<(int64_t val)
    {
      int64_t v = htonll(val);

      if( !b_->append( reinterpret_cast<unsigned char *>(&v),sizeof(int64_t)) )
      {
        THR(exc::rs_cannot_append,*this);
      }

      return *this;
    }

    xdrbuf & xdrbuf::operator<<(uint64_t val)
    {
      uint64_t v = htonll(val);

      if( !b_->append( reinterpret_cast<unsigned char *>(&v),sizeof(uint64_t)) )
      {
        THR(exc::rs_cannot_append,*this);
      }

      return *this;
    }

    xdrbuf & xdrbuf::operator<<(const char * val)
    {
      uint32_t sz = 0;
      if( !val )
      {
        sz = 0;
        (*this) << sz;
      }
      else
      {
        sz = ::strlen(val);
        if( !sz )
        {
          (*this) << sz;
        }
        else
        {
          try
          {
            size_and_buf_to_pbuf( b_, val, ::strlen(val) );
          }
          catch(common::exc e)
          {
            THREX(e,*this);
          }
        }
      }
      return *this;
    }

    xdrbuf & xdrbuf::operator<<(const common::var & val)
    {
      val.to_xdr( *this );
      return *this;
    }

    xdrbuf & xdrbuf::operator<<(const common::str & val)
    {
      uint64_t sz = val.nbytes(); 
      if( sz )
      {
        try
        {
          sz = val.size() * sizeof(wchar_t); 

          size_and_buf_to_pbuf( b_, val.data(), sz );
        }
        catch(common::exc e)
        {
          if( use_exc() ) throw e;
        }
      }
      else
      {
        (*this) << sz;
      }
      return *this;
    }

    xdrbuf & xdrbuf::operator<<(const common::ustr & val)
    {
      uint64_t sz = val.nbytes(); 
      if( sz )
      {
        try
        {
          size_and_buf_to_pbuf( b_, val.data(), sz );
        }
        catch(common::exc e)
        {
          if( use_exc() ) throw e;
        }
      }
      else
      {
        (*this) << sz;
      }
      return *this;
    }

    xdrbuf & xdrbuf::operator<<(const xdrbuf::bindata_t & val)
    {
      uint32_t sz = val.second; // TODO : check for 64bit truncation
      if( sz )
      {
        try
        {
          size_and_buf_to_pbuf( b_, val.first, val.second );
        }
        catch(common::exc e)
        {
          if( use_exc() ) throw e;
        }
      }
      else
      {
        (*this) << sz;
      }
      return *this;
    }

    xdrbuf & xdrbuf::operator<<(const pbuf & val)
    {
      uint32_t sz = val.size(); (*this) << sz;

      if( !val.size() ) return *this;

      pbuf::const_iterator it(val.begin());
      pbuf::const_iterator end(val.end());

      for( ;it!=end;++it )
      {
        const pbuf::buf * bp = (*it);
        if( !bp || !(b_->append( (*it)->data_,(*it)->size_ )) ) return *this;
      }

      unsigned char pad[] = { 0, 0, 0, 0 };
      uint64_t new_len, pad_size;
      round_to_4( sz, new_len, pad_size );

      if( pad_size != 0 ) b_->append( pad, pad_size );

      return *this;
    }

    xdrbuf & xdrbuf::operator>>(int32_t & val)
    {
      int32_t tmp;
      uint64_t szrd=0;

      if( (szrd=get_data( reinterpret_cast<unsigned char *>(&tmp),sizeof(tmp))) == sizeof(tmp) )
      {
        val = ntohl(tmp);
      }
      else if( szrd == 0 && it_==b_->end() )
      {
        THR(exc::rs_xdr_eof,*this);
      }
      else if( szrd != sizeof(tmp) )
      {
        THR(exc::rs_xdr_invalid,*this);
      }
      else
      {
        THR(exc::rs_cannot_get,*this);
      }

      return *this;
    }

    xdrbuf & xdrbuf::operator>>(uint32_t & val)
    {
      uint32_t tmp;
      uint64_t szrd=0;

      if( (szrd=get_data( reinterpret_cast<unsigned char *>(&tmp),sizeof(tmp))) == sizeof(tmp) )
      {
        val = ntohl(tmp);
      }
      else if( szrd == 0 && it_==b_->end() )
      {
        THR(exc::rs_xdr_eof,*this);
      }
      else if( szrd != sizeof(tmp) )
      {
        THR(exc::rs_xdr_invalid,*this);
      }
      else
      {
        THR(exc::rs_cannot_get,*this);
      }

      return *this;
    }

    xdrbuf & xdrbuf::operator>>(int64_t & val)
    {
      int64_t tmp;
      uint64_t szrd=0;

      if( (szrd=get_data( reinterpret_cast<unsigned char *>(&tmp),sizeof(tmp))) == sizeof(tmp) )
      {
        val = ntohll(tmp);
      }
      else if( szrd == 0 && it_==b_->end() )
      {
        THR(exc::rs_xdr_eof,*this);
      }
      else if( szrd != sizeof(tmp) )
      {
        THR(exc::rs_xdr_invalid,*this);
      }
      else
      {
        THR(exc::rs_cannot_get,*this);
      }

      return *this;
    }

    xdrbuf & xdrbuf::operator>>(uint64_t & val)
    {
      uint64_t tmp;
      uint64_t szrd=0;

      if( (szrd=get_data( reinterpret_cast<unsigned char *>(&tmp),sizeof(tmp))) == sizeof(tmp) )
      {
        val = ntohll(tmp);
      }
      else if( szrd == 0 && it_==b_->end() )
      {
        THR(exc::rs_xdr_eof,*this);
      }
      else if( szrd != sizeof(tmp) )
      {
        THR(exc::rs_xdr_invalid,*this);
      }
      else
      {
        THR(exc::rs_cannot_get,*this);
      }

      return *this;
    }

    xdrbuf & xdrbuf::operator>>(common::var & val)
    {
      val.from_xdr( *this );
      return *this;
    }

    xdrbuf & xdrbuf::operator>>(common::str & val)
    {
      uint64_t sz = 0;
      (*this) >> sz;

      uint64_t szrd=0;

      if( !sz ) { val.clear(); return *this; }

      wchar_t * wcp = reinterpret_cast<wchar_t *>(val.buffer().allocate(sz));

      if( sz > 0 )
      {
        if( (szrd=get_data( reinterpret_cast<unsigned char *>(wcp),sz)) == sz )
        {
          val.ensure_trailing_zero();
        }
        else if( szrd == 0 && it_==b_->end() )
        {
          THR(exc::rs_xdr_eof,*this);
        }
        else if( szrd != sz )
        {
          THR(exc::rs_xdr_invalid,*this);
        }
        else
        {
          THR(exc::rs_cannot_get,*this);
        }
      }
      return *this;
    }

    xdrbuf & xdrbuf::operator>>(common::ustr & val)
    {
      uint64_t sz = 0;
      (*this) >> sz;
      uint64_t szrd=0;

      if( !sz ) { val.clear(); return *this; }

      unsigned char * cp = val.buffer().allocate(sz);

      if( sz > 0 )
      {
        if( (szrd=get_data(cp,sz)) == sz )
        {
          val.ensure_trailing_zero();
        }
        else if( szrd == 0 && it_==b_->end() )
        {
          THR(exc::rs_xdr_eof,*this);
        }
        else if( szrd != sz )
        {
          THR(exc::rs_xdr_invalid,*this);
        }
        else
        {
          THR(exc::rs_cannot_get,*this);
        }
      }
      return *this;
    }

    xdrbuf & xdrbuf::operator>>(pbuf & val)
    {
      uint32_t size = 0;
      uint32_t saved_size = 0;
      uint64_t szrd = 0;

      if( it_ == b_->end() ) { goto bail; }

      (*this) >> size;
      saved_size = size;

      /* need to step forward */
      if( pos_ >= (*it_)->size_ ) { ++it_; pos_=0; }

      while( size != 0 )
      {
        if( it_ == b_->end() ) goto bail;

        pbuf::buf * bf = (*it_);
        uint64_t ts = bf->size_-pos_;

        /* have full size */
        if( ts >= size )
        {
          if( !val.append(bf->data_+pos_,size) )
          {
            THRNORET(exc::rs_cannot_append);
            goto bail;
          }

          {
            szrd += size;
            uint64_t new_size,pad_size;
            round_to_4( size, new_size, pad_size );
            pos_ += new_size;
          }

          goto bail;
        }
        else if( ts > 0 )
        {
          if( !val.append(bf->data_+pos_,ts) )
          {
            THRNORET(exc::rs_cannot_append);
            goto bail;
          }
          szrd  += ts;
          size  -= ts;
          pos_   = 0;
          ++it_;
        }
        else
        {
          pos_ = 0;
          ++it_;
        }
      }

    bail:
        if( saved_size != szrd ) { THRNORET(exc::rs_xdr_invalid); }
        return *this;
    }

    bool xdrbuf::get_data(unsigned char * where, uint64_t & size, uint64_t max_size)
    {
      pbuf::iterator oldit = it_;
      uint64_t oldpos  = pos_;

      uint32_t sz = 0;
      (*this) >> sz;

      size = sz;
      if( sz > max_size )
      {
        it_  = oldit;
        pos_ = oldpos;
        size = sz;
        return false;
      }
      uint64_t szrd=get_data( where, sz );

      if( szrd == sz ) return true;
      else
      {
        THR(exc::rs_xdr_invalid,false);
        return false;
      }
    }

    uint64_t xdrbuf::get_data(unsigned char * where, uint64_t size)
    {
      uint64_t ret = 0;
      if( it_ == b_->end() ) return ret;

      /* need to step forward */
      if( pos_ >= (*it_)->size_ ) { ++it_; pos_=0; }

      while( size != 0 )
      {
        if( it_ == b_->end() ) return ret;

        pbuf::buf * bf = (*it_);
        uint64_t ts = bf->size_-pos_;

        /* have full size */
        if( ts >= size )
        {
          ::memcpy( where, bf->data_+pos_, static_cast<size_t>(size) );
          {
            ret += size;
            uint64_t new_size,pad_size;
            round_to_4( size, new_size, pad_size );
            pos_ += new_size;
          }

          return ret;
        }
        else if( ts > 0 )
        {
          ::memcpy( where, bf->data_+pos_, static_cast<size_t>(ts) );
          ret   += ts;
          size  -= ts;
          where += ts;
          pos_   = 0;
          ++it_;
        }
        else
        {
          pos_ = 0;
          ++it_;
        }
      }
      return ret;
    }

    bool xdrbuf::forward(uint64_t n)
    {
      if( it_ == b_->end() ) return false;

      /* need to step forward */
      if( pos_ >= (*it_)->size_ ) { ++it_; pos_=0; }

      while( n != 0 )
      {
        if( it_ == b_->end() ) return false;

        pbuf::buf * bf = (*it_);
        uint64_t ts = bf->size_-pos_;

        /* have full size */
        if( ts >= n )
        {
          {
            uint64_t new_n, pad_size;
            round_to_4( n, new_n, pad_size );
            pos_ += new_n;
          }
          return true;
        }
        else if( ts > 0 )
        {
          n   -= ts;
          pos_ = 0;
          ++it_;
        }
        else
        {
          pos_ = 0;
          ++it_;
        }
      }

      if( n == 0 ) return true;
      else         return false;
    }

    void xdrbuf::rewind()
    {
      pos_ = 0;
      it_  = b_->begin();
    }

    uint64_t xdrbuf::position()
    {
      uint64_t ret = 0;
      pbuf::iterator it = b_->begin();

      while( it != it_ )
      {
        ret += (*it)->size_;
        ++it;
      }

      if( it != b_->end() )
      {
        ret += pos_;
      }

      return ret;
    }
  }
}

/* EOF */
