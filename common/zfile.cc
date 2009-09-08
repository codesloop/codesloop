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

#include "codesloop/common/common.h"
#include "codesloop/common/mpool.hh"
#include "codesloop/common/pvlist.hh"
#include "codesloop/common/zfile.hh"
#include "codesloop/common/str.hh"
#include <zlib.h>

/**
  @file zfile.cc
  @brief implementation of zfile
*/

#ifndef CSL_COMMON_ZFILE_CUSTOM_ALLOC_SIZE
#define CSL_COMMON_ZFILE_CUSTOM_ALLOC_SIZE 512
#endif /* CSL_COMMON_ZFILE_CUSTOM_ALLOC_SIZE */

CSL_CDECL
voidpf _csl_common_alloc_func__(voidpf opaque, uInt items, uInt size);

CSL_CDECL
void _csl_common_free_func__(voidpf opaque, voidpf address);

namespace csl
{
  namespace common
  {
    /** @brief Private implementation of zfile */
    struct zfile::impl
    {
      /* typedef helpers */

      /* variables */
      bool          custom_zlib_allocator_;
      bool          init_custom_memory_;
      mpool<>       pool_;
      pbuf          p_data_;
      pbuf          p_zdata_;

      /* initialization */
      impl() : custom_zlib_allocator_(false), init_custom_memory_(false) { }

      /* private functions */
      voidpf custom_alloc( uInt items, uInt size )
      {
        size_t alloc_size = (items*size);
        voidpf ret = pool_.allocate(alloc_size);
        if( init_custom_memory_ ) memset( ret,0,alloc_size );
        return ret;
      }

      void custom_free( voidpf address ) { }

      bool drop_data()
      {
        bool ret = (p_data_.size() > 0);
        p_data_.free_all();
        return ret;
      }

      bool drop_zdata()
      {
        bool ret = (p_zdata_.size() > 0);
        p_zdata_.free_all();
        return ret;
      }

      void reset_files() { drop_data(); drop_zdata(); }

      bool ensure_file()
      {
        if( p_data_.size() == 0 )
        {
          if( p_zdata_.size() == 0 ) return false; /* empty */
          else
          {
            size_t          sz = 0;
            int             rc = Z_OK;
            unsigned char * b  = 0;

            /* decompress */
            z_stream strm;
            memset( &strm,0,sizeof(strm) );

            if( custom_zlib_allocator_ )
            {
              strm.zalloc = _csl_common_alloc_func__;
              strm.zfree  = _csl_common_free_func__;
              strm.opaque = this;
            }

            if( (rc = inflateInit2(&strm, -MAX_WBITS)) != Z_OK ) return false;

            pbuf::iterator it(p_zdata_.begin());
            pbuf::iterator end(p_zdata_.end());
            unsigned char tmp_buf[pbuf::buf_size];

            for( ;it!=end;++it )
            {
              strm.next_in      = (*it)->data_;
              strm.avail_in     = (*it)->size_;
              strm.next_out = b = tmp_buf;
              strm.avail_out    = pbuf::buf_size;

              while( strm.avail_in != 0 && strm.avail_out != 0 )
              {
                do
                {
                  rc = inflate( &strm, Z_NO_FLUSH );
                  if( rc < 0 ) /* any error */
                  {
                    inflateEnd(&strm);
                    drop_data();
                    return false;
                  }
                  sz = (pbuf::buf_size-strm.avail_out);
                  if( sz > 0 )
                  {
                    p_data_.append(b,sz);
                    strm.next_out = b = tmp_buf;
                    strm.avail_out = pbuf::buf_size;
                  }
                } while( sz > 0 && rc == Z_OK && strm.avail_in > 0 );
              }
            }
            inflateEnd(&strm);
          }
        }
        return true;
      }

      bool ensure_zfile()
      {
        if( p_zdata_.size() == 0  )
        {
          if ( p_data_.size() == 0 ) return false;  /* empty */
          else
          {
            size_t          sz = 0;
            int             rc = Z_OK;
            unsigned char * b = 0;

            /* compress */
            z_stream strm ;
            memset( &strm,0,sizeof(strm) );

            if( custom_zlib_allocator_ )
            {
              strm.zalloc = _csl_common_alloc_func__;
              strm.zfree  = _csl_common_free_func__;
              strm.opaque = this;
            }


            if( deflateInit2( &strm, Z_BEST_SPEED, Z_DEFLATED, -MAX_WBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY ) != Z_OK ) return false;

            pbuf::iterator it(p_data_.begin());
            pbuf::iterator end(p_data_.end());
            unsigned char tmp_buf[pbuf::buf_size];

            strm.next_out = b = tmp_buf;
            strm.avail_out = pbuf::buf_size;

            for( ;it!=end;++it )
            {
              strm.next_in  = (*it)->data_;
              strm.avail_in = (*it)->size_;

              while( strm.avail_in != 0 && strm.avail_out != 0 )
              {
                if( (rc = deflate(&strm, Z_NO_FLUSH )) == Z_STREAM_ERROR )
                {
                  deflateEnd(&strm);
                  drop_zdata();
                  return false;
                }
                sz = (pbuf::buf_size - strm.avail_out);
                if( sz == pbuf::buf_size )
                {
                  p_zdata_.append(b,sz);
                  strm.next_out = b = tmp_buf;
                  strm.avail_out = pbuf::buf_size;
                }
              }
            }

            sz = (pbuf::buf_size - strm.avail_out);
            if( sz == pbuf::buf_size )
            {
              /* ensure we have space */
              p_zdata_.append(b,sz);
              strm.next_out = b = tmp_buf;
              strm.avail_out = pbuf::buf_size;
            }

            do
            {
              if( (rc=deflate(&strm,Z_FINISH)) == Z_STREAM_ERROR )
              {
                deflateEnd(&strm);
                drop_zdata();
                return false;
              }
              sz = (pbuf::buf_size - strm.avail_out);
              if( sz > 0 )
              {
                /* eat all output data */
                p_zdata_.append(b,sz);
                strm.next_out = b = tmp_buf;
                strm.avail_out = (pbuf::buf_size);
              }

            } while( sz > 0 && rc == Z_OK );

            deflateEnd(&strm);
          }
        }
        return true;
      }

      bool get_data_common(unsigned char * data, const pbuf & b) const
      {
        pbuf::const_iterator it(b.const_begin());
        pbuf::const_iterator end(b.const_end());

        for( ;it!=end;++it )
        {
          const pbuf::buf * bf = (*it);
          if( bf->size_ )
          {
            memcpy( data,bf->data_,bf->size_ );
            data += bf->size_;
          }
        }
        return true;
      }

      bool write_file_common( const char * filename, const pbuf & bf ) const
      {
        bool ret = false;
        FILE * fp = fopen( filename, "wb" );
        if( !fp ) return false;

        pbuf::const_iterator it(bf.const_begin());
        pbuf::const_iterator end(bf.const_end());

        for( ;it!=end;++it )
        {
          const pbuf::buf * bfit = (*it);
          if( bfit->size_ )
          {
            if( fwrite( bfit->data_, 1, bfit->size_, fp) != bfit->size_ ) goto bail;
          }
        }

        ret = true;
      bail:
          fclose( fp );
      return ret;
      }

      bool put_data_common(const unsigned char * data, size_t len, pbuf & b)
      {
        if( !data || !len ) return false;
        reset_files();

        b.append( data,len );
        return true;
      }

      size_t get_size_common(const pbuf & b) const { return b.size(); }

      bool read_file_common( const char * filename, pbuf & bf )
      {
        FILE * fp = fopen( filename, "rb" );
        if( !fp ) return false;
        reset_files();

        int rc = 0;
        unsigned char tmp[pbuf::buf_size];

        while( (rc = fread( tmp,1,pbuf::buf_size,fp )) > 0 )
        {
          bf.append( tmp,rc );
        }

        fclose( fp );
        return true;
      }

      bool dump_data_common( common::str & str, const pbuf & bf, char prefix ) const
      {
        wchar_t tmp[32];
        str.clear();

        SWPRINTF(tmp,sizeof(tmp),L"[sz=%ld]\n",static_cast<unsigned long>(get_size_common(bf)));
        str += tmp;

        size_t b = 0;

        pbuf::const_iterator it(bf.begin());
        pbuf::const_iterator end(bf.end());

        for( ;it!=end;++it )
        {
          const pbuf::buf * bx = (*it);
          size_t i,l=0;
          size_t k=0;
          while( k<(bx)->size_ )
          {
            SWPRINTF( tmp,sizeof(tmp),L"%c[%.2d:%.2d:%.4d:%.4d]: ",
                prefix,
                static_cast<unsigned short>(b),
                static_cast<unsigned short>(l),
                static_cast<unsigned int>(k),
                static_cast<unsigned int>(bx->size_) );
            str += tmp;
            for( i=0;(k<(bx->size_)&& i<16);++i )
            {
              SWPRINTF( tmp,sizeof(tmp),L"%.2x", bx->data_[k] );
              str += tmp;
              ++k;
            }
            ++l;
            str += L"\n";
          }
          ++b;
        }
        return true;
      }

      /* interface */
      void custom_zlib_allocator(bool yesno) { custom_zlib_allocator_ = yesno; }

      void init_custom_memory(bool yesno) { init_custom_memory_ = yesno; }

      bool read_file( const char * filename )
      {
        if( !filename ) return false;
        return read_file_common( filename, p_data_ );
      }

      bool read_zfile( const char * filename )
      {
        if( !filename ) return false;
        return read_file_common( filename, p_zdata_ );
      }

      bool write_file( const char * filename )
      {
        if( !filename ) return false;
        if( !ensure_file() ) return false;
        return write_file_common( filename, p_data_ );
      }

      bool write_zfile( const char * filename )
      {
        if( !filename ) return false;
        if( !ensure_zfile() ) return false;
        return write_file_common( filename, p_zdata_ );
      }

      bool dump_data(common::str & str)
      {
        if( !ensure_file() ) return false;
        return dump_data_common( str, p_data_, '-' );
      }

      bool dump_zdata(common::str & str)
      {
        if( !ensure_zfile() ) return false;
        return dump_data_common( str, p_zdata_, 'z' );
      }

      unsigned char * get_buff()
      {
        size_t sz = get_size();
        if( !sz ) return 0;
        unsigned char * ret = reinterpret_cast<unsigned char *>(pool_.allocate( sz ));
        if( !ret || !get_data( ret ) ) return 0;
        return ret;
      }

      unsigned char * get_zbuff()
      {
        size_t sz = get_zsize();
        if( !sz ) return 0;
        unsigned char * ret = reinterpret_cast<unsigned char *>(pool_.allocate( sz ));
        if( !ret || !get_zdata( ret ) ) return 0;
        return ret;
      }

      size_t get_size()
      {
        if( !ensure_file() ) return 0;
        return get_size_common( p_data_ );
      }

      size_t get_zsize()
      {
        if( !ensure_zfile() ) return 0;
        return get_size_common( p_zdata_ );
      }

      size_t get_size_const() const
      {
        return get_size_common( p_data_ );
      }

      size_t get_zsize_const() const
      {
        return get_size_common( p_zdata_ );
      }

      bool get_data(unsigned char * data)
      {
        if( !data ) return false;
        if( !ensure_file() ) return false;
        return get_data_common( data, p_data_ );
      }

      bool get_zdata(unsigned char * data)
      {
        if( !data ) return false;
        if( !ensure_zfile() ) return false;
        return get_data_common( data, p_zdata_ );
      }

      bool get_data(pbuf & dta)
      {
        if( !ensure_file() ) return false;
        dta = p_data_;
        return true;
      }

      bool get_zdata(pbuf & dta)
      {
        if( !ensure_zfile() ) return false;
        dta = p_zdata_;
        return true;
      }

      const pbuf & get_data()
      {
        ensure_file();
        return p_data_;
      }

      const pbuf & get_zdata()
      {
        ensure_zfile();
        return p_zdata_;
      }

      bool get_data_const(unsigned char * data) const
      {
        if( !data ) return false;
        return get_data_common( data, p_data_ );
      }

      bool get_zdata_const(unsigned char * data) const
      {
        if( !data ) return false;
        return get_data_common( data, p_zdata_ );
      }

      bool get_data_const(pbuf & dta) const
      {
        if( !p_data_.size() ) return false;
        dta = p_data_;
        return true;
      }

      bool get_zdata_const(pbuf & dta) const
      {
        if( !p_zdata_.size() ) return false;
        dta = p_zdata_;
        return true;
      }

      bool put_data(const unsigned char * data,size_t len)
      {
        if( !data || !len ) return false;
        return put_data_common( data, len, p_data_ );
      }

      bool put_zdata(const unsigned char * data,size_t len)
      {
        if( !data || !len ) return false;
        return put_data_common( data, len, p_zdata_ );
      }

      bool put_data(const pbuf & dta)
      {
        if( dta.size() )
        {
          p_data_ = dta;
          return true;
        }
        else return false;
      }

      bool put_zdata(const pbuf & dta)
      {
        if( dta.size() )
        {
          p_zdata_ = dta;
          return true;
        }
        else return false;
      }
    };

    /* public interface */
    void zfile::init_custom_memory(bool yesno)    { impl_->init_custom_memory(yesno); }
    void zfile::custom_zlib_allocator(bool yesno) { impl_->custom_zlib_allocator(yesno); }

    size_t zfile::get_size()  { return impl_->get_size();  }
    size_t zfile::get_zsize() { return impl_->get_zsize(); }

    size_t zfile::get_size_const() const  { return impl_->get_size_const();  }
    size_t zfile::get_zsize_const() const { return impl_->get_zsize_const(); }

    bool zfile::get_data(unsigned char * data)  { return impl_->get_data(data);  }
    bool zfile::get_zdata(unsigned char * data) { return impl_->get_zdata(data); }

    bool zfile::get_data(pbuf & dta)  { return impl_->get_data(dta);  }
    bool zfile::get_zdata(pbuf & dta) { return impl_->get_zdata(dta); }

    bool zfile::get_data_const(unsigned char * data) const  { return impl_->get_data_const(data);  }
    bool zfile::get_zdata_const(unsigned char * data) const { return impl_->get_zdata_const(data); }

    bool zfile::get_data_const(pbuf & dta) const  { return impl_->get_data_const(dta);  }
    bool zfile::get_zdata_const(pbuf & dta) const { return impl_->get_zdata_const(dta); }

    const pbuf & zfile::get_data()  { return impl_->get_data();  }
    const pbuf & zfile::get_zdata() { return impl_->get_zdata(); }

    unsigned char * zfile::get_buff()  { return impl_->get_buff();  }
    unsigned char * zfile::get_zbuff() { return impl_->get_zbuff(); }

    bool zfile::dump_data(common::str & str)  { return impl_->dump_data(str);  }
    bool zfile::dump_zdata(common::str & str) { return impl_->dump_zdata(str); }

    bool zfile::drop_data()  { return impl_->drop_data();  }
    bool zfile::drop_zdata() { return impl_->drop_zdata(); }

    bool zfile::put_data(const unsigned char * data,size_t len)  { return impl_->put_data(data,len);  }
    bool zfile::put_zdata(const unsigned char * data,size_t len) { return impl_->put_zdata(data,len); }

    bool zfile::put_data(const pbuf & dta)  { return impl_->put_data(dta); }
    bool zfile::put_zdata(const pbuf & dta) { return impl_->put_zdata(dta); }

    bool zfile::read_file( const char * filename )  { return impl_->read_file(filename);  }
    bool zfile::read_zfile( const char * filename ) { return impl_->read_zfile(filename); }

    bool zfile::write_file( const char * filename )  { return impl_->write_file(filename);  }
    bool zfile::write_zfile( const char * filename ) { return impl_->write_zfile(filename); }

    zfile::zfile() : impl_(new impl) {}
    zfile::~zfile() {}

    zfile::zfile(const zfile & other) { *this = other; }

    bool zfile::operator==(const zfile & other) const
    {
      mpool<> mp;
      size_t s1 = this->get_size_const();
      size_t s2 = other.get_size_const();
      unsigned char * p1 = 0;
      unsigned char * p2 = 0;

      if( s1 != s2 ) { return false; }

      if( s1 )
      {
        p1 = reinterpret_cast<unsigned char *>(mp.allocate(s1));
        p2 = reinterpret_cast<unsigned char *>(mp.allocate(s2));

        if( this->get_data_const(p1) && other.get_data_const(p2) )
        {
          return (::memcmp(p1,p2,s1)==0);
        }
        else return false;
      }
      else
      {
        s1 = this->get_zsize_const();
        s2 = this->get_zsize_const();

        if( s1 != s2 ) return false;
        if( !s1 ) return true;

        p1 = reinterpret_cast<unsigned char *>(mp.allocate(s1));
        p2 = reinterpret_cast<unsigned char *>(mp.allocate(s2));

        if( this->get_zdata_const(p1) && other.get_zdata_const(p2) )
        {
          return (::memcmp(p1,p2,s1)==0);
        }
        else return false;
      }
    }

    zfile & zfile::operator=(const zfile & other)
    {
      impl_->custom_zlib_allocator_ = other.impl_->custom_zlib_allocator_;
      impl_->init_custom_memory_    = other.impl_->init_custom_memory_;

      size_t sz = other.get_zsize_const();
      if( sz )
      {
        impl_->p_zdata_ = other.impl_->p_zdata_;
        impl_->drop_data();
      }
      else if( (sz=other.get_size_const()) > 0 )
      {
        impl_->p_data_ = other.impl_->p_data_;
        impl_->drop_zdata();
      }
      else
      {
        impl_->drop_data();
        impl_->drop_zdata();
      }
      return *this;
    }
  }; // common
}; // csl

CSL_CDECL
voidpf _csl_common_alloc_func__(voidpf opaque, uInt items, uInt size)
{
  if( opaque && items && size )
  {
    csl::common::zfile::impl * impl_ptr = (csl::common::zfile::impl *)opaque;
    return impl_ptr->custom_alloc( items, size );
  }
  return 0;
}

CSL_CDECL
void _csl_common_free_func__(voidpf opaque, voidpf address)
{
  if( opaque  && address )
  {
    csl::common::zfile::impl * impl_ptr = (csl::common::zfile::impl *)opaque;
    return impl_ptr->custom_free( address );
  }
}

/* EOF */
