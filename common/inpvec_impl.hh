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

#ifndef _csl_common_inpvec_impl_hh_included_
#define _csl_common_inpvec_impl_hh_included_

/**
   @file inpvec_impl.hh
   @brief implementation of inpvec
 */

#ifdef __cplusplus

namespace csl
{
  namespace common
  {

          template <typename T> uint64_t inpvec<T>::item::n_items() const
          {
            const unsigned char * bb = byte_bits();

            uint64_t ret = 0;
            for( mul_t i=0;i<mul_;++i )
            {
              ret += bb[((bmap_[i])&0xff)];
              ret += bb[((bmap_[i]>>8)&0xff)];
              ret += bb[((bmap_[i]>>16)&0xff)];
              ret += bb[((bmap_[i]>>24)&0xff)];
              // for 64 bit
              ret += bb[((bmap_[i]>>32)&0xff)];
              ret += bb[((bmap_[i]>>40)&0xff)];
              ret += bb[((bmap_[i]>>48)&0xff)];
              ret += bb[((bmap_[i]>>56)&0xff)];
            }
            return ret;
          }

          template <typename T> uint64_t inpvec<T>::item::last_free() const
          {
            ENTER_FUNCTION();
            const unsigned char * bl = byte_last_free();
            uint64_t ret = size();
            uint8_t x1,x2,x3,x4, y1,y2,y3,y4;

            for( int64_t i=static_cast<int64_t>(mul_)-1;i>=0;--i )
            {
              if( bmap_[i] == 0x0UL ) ret = i*width_;
              else
              {
                // for 64 bits
                if( (y1 = (bl[(bmap_[i]>>56)&0xff])) == 0 )  { ret = i*width_+56;           }
                else                                         { ret = i*width_+56+y1; break; }
                if( (y2 = (bl[(bmap_[i]>>48)&0xff])) == 0 )  { ret = i*width_+48;           }
                else                                         { ret = i*width_+48+y2; break; }
                if( (y3 = (bl[(bmap_[i]>>40)&0xff])) == 0 )  { ret = i*width_+40;           }
                else                                         { ret = i*width_+40+y3; break; }
                if( (y4 = (bl[(bmap_[i]>>32)&0xff])) == 0 )  { ret = i*width_+32;           }
                else                                         { ret = i*width_+32+y4; break; }
                // for 32 bits
                if( (x1 = (bl[(bmap_[i]>>24)&0xff])) == 0 )  { ret = i*width_+24;           }
                else                                         { ret = i*width_+24+x1; break; }
                if( (x2 = (bl[(bmap_[i]>>16)&0xff])) == 0 )  { ret = i*width_+16;           }
                else                                         { ret = i*width_+16+x2; break; }
                if( (x3 = (bl[(bmap_[i]>>8)&0xff])) == 0 )   { ret = i*width_+8;            }
                else                                         { ret = i*width_+8+x3;  break; }
                if( (x4 = (bl[(bmap_[i])&0xff])) == 0 )      { ret = i*width_;              }
                else                                         { ret = i*width_+x4;    break; }
              }
            }
            RETURN_FUNCTION(ret);
          }


          template <typename T> uint64_t inpvec<T>::item::first_free() const
          {
            ENTER_FUNCTION();
            const unsigned char * bf = byte_first_free();
            uint64_t ret = size();
            uint8_t x1,x2,x3,x4, y1,y2,y3,y4;

            for( int64_t i=0;i<static_cast<int64_t>(mul_);++i )
            {
              if( bmap_[i] == 0xFFFFFFFFFFFFFFFFULL ) ret = (i+1)*width_;
              else
              {
                // for 32 bits
                if( (x4 = (bf[(bmap_[i])&0xff])) == 0xff )      { ret = i*width_;              }
                else                                            { ret = i*width_+x4;    break; }
                if( (x3 = (bf[(bmap_[i]>>8)&0xff])) == 0xff )   { ret = i*width_+8;            }
                else                                            { ret = i*width_+8+x3;  break; }
                if( (x2 = (bf[(bmap_[i]>>16)&0xff])) == 0xff )  { ret = i*width_+16;           }
                else                                            { ret = i*width_+16+x2; break; }
                if( (x1 = (bf[(bmap_[i]>>24)&0xff])) == 0xff )  { ret = i*width_+24;           }
                else                                            { ret = i*width_+24+x1; break; }
                // for 64 bits
                if( (y4 = (bf[(bmap_[i]>>32)&0xff])) == 0xff )  { ret = i*width_+32;           }
                else                                            { ret = i*width_+32+y4; break; }
                if( (y3 = (bf[(bmap_[i]>>40)&0xff])) == 0xff )  { ret = i*width_+40;           }
                else                                            { ret = i*width_+40+y3; break; }
                if( (y2 = (bf[(bmap_[i]>>48)&0xff])) == 0xff )  { ret = i*width_+48;           }
                else                                            { ret = i*width_+48+y2; break; }
                if( (y1 = (bf[(bmap_[i]>>56)&0xff])) == 0xff )  { ret = i*width_+56;           }
                else                                            { ret = i*width_+56+y1; break; }
              }
            }
            RETURN_FUNCTION(ret);
          }

          template <typename T> void inpvec<T>::item::destroy()
          {
            ENTER_FUNCTION_X();
            CSL_DEBUGF_X(L"destroy()");

            uint64_t n_i = 0;
            uint64_t * px = &n_i;
            if( parent_ ) px = &(parent_->n_items_);

            for( mul_t i=0;i<mul_; ++i )
            {
              if( bmap_[i] != 0 )
              {
                for( unsigned char j=0;j<width_;++j )
                {
                  if( (bmap_[i]>>j)&static_cast<bitmap_t>(1ULL) )
                  {
                    in_place_destruct( items_+((i*width_)+j) );
                    --(*px);
                  }
                }
              }
            }
            LEAVE_FUNCTION_X();
          }

          template <typename T> void inpvec<T>::item::destroy( uint64_t at )
          {
            ENTER_FUNCTION_X();
            CSL_DEBUGF_X(L"destroy(%lld)",at);
            uint64_t off = at/width_;
            uint64_t pos = at%width_;

            if( (bmap_[off]>>pos)&static_cast<bitmap_t>(1ULL) )
            {
              in_place_destruct( items_+at );
              bmap_[off] = (bmap_[off] & (~(static_cast<bitmap_t>(1ULL)<<pos)));
              if( parent_ ) --(parent_->n_items_);
            }
            LEAVE_FUNCTION_X();
          }

          template <typename T> void inpvec<T>::item::mul_alloc( mul_t m )
          {
            ENTER_FUNCTION();

            uint64_t item_size    = (m*width_*sizeof(T));  // preallocated size for items
            uint64_t bitmap_size  = (m*sizeof(bitmap_t));   // preallocated size for bitmap

            mul_         = m;
            free_        = 1;

            CSL_DEBUGF(L"allocating %lld bytes (items:%lld bitmap:%lld)",
                       item_size+bitmap_size,item_size,bitmap_size);

            /* one allocation is done here for the bitmap and the items */
            buffer_      = reinterpret_cast<uint8_t *>(malloc(static_cast<size_t>(item_size+bitmap_size)));

            /* set the pointers pointing to the previously allocated buffer */
            bmap_        = reinterpret_cast<bitmap_t *>(buffer_);
            items_       = reinterpret_cast<T *>(buffer_+bitmap_size);

            /* set pointers */
            next_        = 0;
            parent_      = 0;

            memset( bmap_,0,static_cast<size_t>(bitmap_size));
            LEAVE_FUNCTION();
          }

          template <typename T> T * inpvec<T>::item::construct(uint64_t at)
          {
            ENTER_FUNCTION();
            uint64_t off = at/width_;
            uint64_t pos = at%width_;

            CSL_DEBUGF(L"construct(%lld) => [off:%lld pos:%lld mul:%d free:%d]",at,off,pos,mul_,free_);

            if( (bmap_[off]>>pos)&static_cast<bitmap_t>(1ULL) )
            { // already have an item
            }
            else
            { // "allocate" = default construct an item
              new (items_+at) T();
              bmap_[off] = (bmap_[off]) | (static_cast<bitmap_t>(1ULL)<<pos);
              if( parent_ ) ++(parent_->n_items_);
            }
            RETURN_FUNCTION(items_+at);
          }

          template <typename T> T * inpvec<T>::item::set(uint64_t at, const T & t)
          {
            ENTER_FUNCTION();
            uint64_t off = at/width_;
            uint64_t pos = at%width_;

            CSL_DEBUGF(L"set(%lld,%p) => [off:%lld pos:%lld mul:%d free:%d]",at,&t,off,pos,mul_,free_);

            if( (bmap_[off]>>pos)&static_cast<bitmap_t>(1ULL) )
            { // already have an item
              items_[at] = t;
            }
            else
            { // "allocate" = copy construct an item
              new (items_+at) T(t);
              bmap_[off] = (bmap_[off]) | (static_cast<bitmap_t>(1ULL)<<pos);
              if( parent_ ) ++(parent_->n_items_);
            }
            RETURN_FUNCTION(items_+at);
          }

          template <typename T> bool inpvec<T>::item::is_empty() const
          {
            ENTER_FUNCTION_X();
            bool ret = true;
            for( mul_t j=0;j<mul_;++j )
            {
              if( bmap_[j] != 0xFFFFFFFFUL )
              {
                ret = false;
                break;
              }
            }
            CSL_DEBUGF_X(L"is_empty() => %s",(ret == true?"true":"false"));
            RETURN_FUNCTION_X( ret );
          }

          template <typename T> bool inpvec<T>::item::is_empty(uint64_t at) const
          {
            ENTER_FUNCTION_X();
            uint64_t off = at/width_;
            uint64_t pos = at%width_;

            bool ret = true;

            if( off <= mul_ )
            {
              if( (bmap_[off]>>pos)&static_cast<bitmap_t>(1ULL) )
              {
                ret = false;
              }
            }
            CSL_DEBUGF_X(L"is_empty(%lld) [%lld:%lld] => %s",at,off,pos,(ret == true?"true":"false"));
            RETURN_FUNCTION_X( ret );
          }

          template <typename T> void inpvec<T>::item::debug()
          {
#ifdef DEBUG
            ENTER_FUNCTION();
            for( mul_t i=0;i<mul_;++i )
            {
              char xd[width_+1];
              for( unsigned char j=0;j<width_;++j )
              {
                if( (bmap_[i]>>j)&static_cast<bitmap_t>(1ULL) ) xd[j] = 'X';
                else                                            xd[j] = '.';
              }
              xd[width_]=0;
              CSL_DEBUGF(L"%s",xd);
            }
            CSL_DEBUGF(L"mul:%d  width:%d  free-me:%d",mul_,width_,free_);
            LEAVE_FUNCTION();
#endif /*DEBUG*/
          }

          template <typename T> bool inpvec<T>::item::is_last(uint64_t at) const
          {
            ENTER_FUNCTION_X();
            uint64_t off = at/width_;
            uint64_t pos = at%width_;

            CSL_DEBUGF_X(L"position: %lld [%lld:%lld mul:%d]",at,off,pos,mul_);

            if( off > mul_ )
            {
              CSL_DEBUGF_X(L"invalid position: %lld",at);
              RETURN_FUNCTION_X( true );
            }

            mul_t i;
            width_t j;

            for( j=static_cast<width_t>(pos+1);j<width_;++j )
            {
              if( (bmap_[off]>>j)&static_cast<bitmap_t>(1ULL) )
              {
                CSL_DEBUGF_X(L"ret 1: %d",j);
                RETURN_FUNCTION_X( false );
              }
            }

            for( i=static_cast<mul_t>(off+1);i<mul_;++i )
            {
              if( bmap_[i] == 0x0UL ) continue;

              for( j=0;j<width_;++j )
              {
                if( (bmap_[i]>>j)&static_cast<bitmap_t>(1ULL) )
                {
                  CSL_DEBUGF_X(L"ret 2: [i:%d j:%d]",i,j);
                  RETURN_FUNCTION_X( false );
                }
              }
            }
            RETURN_FUNCTION_X( true );
          }

          template <typename T> T & inpvec<T>::item::get(uint64_t at) const
          {
            ENTER_FUNCTION_X();
            T * ret = (items_+at);
            CSL_DEBUGF_X(L"returning ref to: %p at %lld",ret,at);
            RETURN_FUNCTION_X( *ret );
          }

          template <typename T> T * inpvec<T>::item::get_ptr(uint64_t at) const
          {
            ENTER_FUNCTION_X();
            uint64_t off = at/width_;
            uint64_t pos = at%width_;

            CSL_DEBUGF_X(L"get_ptr(%lld) [off:%lld pos:%lld]",at,off,pos);

            T * ret = 0;

            if( (bmap_[off]>>pos)&static_cast<bitmap_t>(1ULL) ) { ret = items_+at; }

            CSL_DEBUGF_X(L"returning ptr: %p at %lld",ret,at);

            RETURN_FUNCTION_X( ret );
          }

          template <typename T> T * inpvec<T>::item::next_used(uint64_t & at)
          {
            ENTER_FUNCTION();
            uint64_t newpos   = at+1;
            uint64_t off      = (newpos)/width_;
            uint64_t pos      = (newpos)%width_;
            CSL_DEBUGF(L"next_used(pos: %lld) [off:%lld pos:%lld]",at,off,pos);

            T * ret = 0;

            for( mul_t i=static_cast<mul_t>(off);i<mul_;++i )
            {
              if( bmap_[i] == 0ULL )
              {
                CSL_DEBUGF(L"Skip %d positions",width_);
              }
              else
              {
                for( width_t j=static_cast<width_t>(pos);j<width_;++j )
                {
                  uint64_t k = (bmap_[i]>>j);
                  if( k&static_cast<bitmap_t>(1ULL) )
                  {
                    newpos = (i*width_)+j;
                    ret    = items_+newpos;
                    goto break_cycle;
                  }
                  else if( k == 0ULL )
                  {
                    /* skip next (width_-pos) items */
                    CSL_DEBUGF(L"Skip %d positions",(width_-j));
                    break;
                  }
                }
              }
              pos = 0;
            }

          break_cycle:

            CSL_DEBUGF(L"next_used(pos: %lld -> %lld) => %p",at,newpos,ret);
            at = newpos;
            RETURN_FUNCTION( ret );
          }

        template <typename T> void inpvec<T>::allocate()
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"allocate()");
          allocate( (tail_->mul_+2) );
          LEAVE_FUNCTION();
        }

        template <typename T> void inpvec<T>::allocate(mul_t mul)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"allocate(%lld)",mul);
          item * n = new item( );
          n->mul_alloc( mul );
          n->parent_ = this;
          tail_->next_ = n;
          tail_ = n;
          LEAVE_FUNCTION();
        }

            template <typename T> void inpvec<T>::iterator::init(item * i, uint64_t pos, uint64_t gp)
            {
              ENTER_FUNCTION_X();
              CSL_DEBUGF_X(L"init(%p,%lld,%lld)",i,pos,gp);
              i_ = i; pos_ = pos; gpos_ = gp;
              LEAVE_FUNCTION_X();
            }

            template <typename T> typename inpvec<T>::iterator & inpvec<T>::iterator::operator=(const iterator & other)
            {
              ENTER_FUNCTION_X();
              CSL_DEBUGF_X(L"operator=(iterator ref& %p)",&other);

              i_     = other.i_;
              pos_   = other.pos_;
              gpos_  = other.gpos_;

              RETURN_FUNCTION_X(*this);
            }

            template <typename T> bool inpvec<T>::iterator::operator==(const iterator & other) const
            {
              ENTER_FUNCTION_X();
              CSL_DEBUGF_X(L"operator==(iterator ref& %p)",&other);
              RETURN_FUNCTION_X((i_ == other.i_ && gpos_ == other.gpos_) ? true : false );
            }

            template <typename T> bool inpvec<T>::iterator::operator!=(const iterator & other) const
            {
              ENTER_FUNCTION_X();
              CSL_DEBUGF_X(L"operator!=(iterator ref& %p)",&other);
              RETURN_FUNCTION_X(!(operator==(other)));
            }

            template <typename T> void inpvec<T>::iterator::operator++()
            {
              ENTER_FUNCTION_X();
              if( i_ == 0 )
              {
                i_    = 0;
                pos_  = 0;
                gpos_ = 0;
              }
              else if( i_->is_last(pos_) == true )
              {
                i_   = i_->next_;
                pos_ = 0;
                if( !i_ ) { gpos_ = 0; }
                else      { ++gpos_;   }
              }
              else
              {
                ++pos_;
                ++gpos_;
              }
              LEAVE_FUNCTION_X();
            }

            template <typename T> T * inpvec<T>::iterator::next_used()
            {
              ENTER_FUNCTION();
              T * ret = 0;
              uint64_t newpos = pos_;

              CSL_DEBUGF(L"next_used() [i:%p pos:%lld gpos:%lld]",i_,pos_,gpos_);

              item * p = i_;

              while( p )
              {
                ret = p->next_used( newpos );

                if( ret )
                {
                  gpos_ += (newpos - pos_);
                  CSL_DEBUGF(L"jumping %lld positions",(newpos-pos_));
                  CSL_DEBUGF(L"old gpos:%lld ==> new gpos:%lld",gpos_-(newpos-pos_),gpos_);
                  pos_   = newpos;
                  i_     = p;
                  CSL_DEBUGF(L"pos is:%lld i:%p",pos_,i_);
                  break;
                }
                else
                {
                  newpos = pos_;
                  gpos_ += (p->size() - pos_);
                  pos_   = 0;
                  i_     = p;
                }
                p = p->next_;
              }

              if( !ret ) { gpos_ = 0; pos_ = 0; i_ = 0; }

              CSL_DEBUGF(L"next_used() [i:%p pos:%lld gpos:%lld] => %p",i_,pos_,gpos_,ret);

              RETURN_FUNCTION( ret );
            }

            template <typename T> void inpvec<T>::iterator::zero()
            {
              ENTER_FUNCTION_X();
              i_ = 0; pos_ = 0; gpos_ = 0;
              LEAVE_FUNCTION_X();
            }

            template <typename T> bool inpvec<T>::iterator::at_end() const
            {
              ENTER_FUNCTION_X();
              CSL_DEBUGF_X(L"at_end() => %s",((i_ == 0 && pos_ == 0)?"true":"false"));
              RETURN_FUNCTION_X((i_ == 0 && pos_ == 0));
            }

            template <typename T> T * inpvec<T>::iterator::operator*()
            {
              ENTER_FUNCTION_X();
              T * ret = 0;
              if( i_ ) { ret = ( i_->get_ptr(pos_) ); }
              else
              {
                CSL_DEBUGF_X(L"i_ is null: %s",((pos_==0 && gpos_==0)?"(end iterator)":"(unknown error)") );
              }
              RETURN_FUNCTION_X( ret );
            }

            template <typename T> bool inpvec<T>::iterator::is_empty() const
            {
              ENTER_FUNCTION_X();
              bool ret = true;
              if( i_ ) { ret = i_->is_empty(pos_); }
              CSL_DEBUGF_X(L"is_empty() : %s",(ret == true?"true":"false"));
              RETURN_FUNCTION_X( ret );
            }

            template <typename T> void inpvec<T>::iterator::free()
            {
              ENTER_FUNCTION();
              if( i_ ) { i_->destroy( pos_ ); }
              LEAVE_FUNCTION();
            }

            template <typename T> uint64_t inpvec<T>::iterator::n_free() const
            {
              ENTER_FUNCTION();
              if( i_ ) { RETURN_FUNCTION( i_->n_free() ); }
              else     { RETURN_FUNCTION( 0 ); }
            }

            template <typename T> uint64_t inpvec<T>::iterator::get_pos() const
            {
              ENTER_FUNCTION();
              CSL_DEBUGF(L"Returning position: %lld",gpos_);
              RETURN_FUNCTION( gpos_ );
            }

            template <typename T> T * inpvec<T>::iterator::construct()
            {
              ENTER_FUNCTION();
              T * ret = 0;
              if( i_ ) ret = i_->construct( pos_ );
              CSL_DEBUG_ASSERT( i_ != 0 );
              RETURN_FUNCTION( ret );
            }

            template <typename T> T * inpvec<T>::iterator::set(const T & t)
            {
              ENTER_FUNCTION();
              T * ret = 0;
              if( i_ ) ret = i_->set( pos_,t );
              CSL_DEBUG_ASSERT( i_ != 0 );
              RETURN_FUNCTION( ret );
            }

        template <typename T> const typename inpvec<T>::iterator & inpvec<T>::end()
        {
          ENTER_FUNCTION_X( );
          RETURN_FUNCTION_X( end_ );
        }

        template <typename T> const typename inpvec<T>::iterator & inpvec<T>::begin()
        {
          ENTER_FUNCTION_X( );
          if( n_items_ == 0 )
          {
            CSL_DEBUGF_X(L"no items: returning end_");
            RETURN_FUNCTION_X( end_ );
          }
          RETURN_FUNCTION_X( begin_ );
        }

        template <typename T> uint64_t inpvec<T>::n_items()
        {
          ENTER_FUNCTION( );
          CSL_DEBUGF(L"n_items() => %lld",n_items_);
          RETURN_FUNCTION( n_items_ );
        }

        template <typename T> uint64_t inpvec<T>::size()
        {
          ENTER_FUNCTION( );
          uint64_t ret = 0;
          item * p = &head_;
          while( p != 0 )
          {
            ret += ((p->mul_)*width_);
            p = p->next_;
          }
          CSL_DEBUGF(L"size() => %lld",ret);
          RETURN_FUNCTION( ret );
        }

        template <typename T> void inpvec<T>::debug()
        {
#ifdef DEBUG
          ENTER_FUNCTION();
          CSL_DEBUGF(L"%d items",n_items_);
          item * p = &head_;
          while( p )
          {
            p->debug();
            p = p->next_;
          }
          LEAVE_FUNCTION();
#endif /*DEBUG*/
        }

        template <typename T> typename inpvec<T>::iterator inpvec<T>::last_free()
        {
          ENTER_FUNCTION();
          mul_t    mul = (tail_->mul_+2);
          uint64_t pos = tail_->last_free();

          if( pos == tail_->size() )
          {
            uint64_t sz  = size();
            allocate(mul);
            CSL_DEBUGF(L"Allocate. Returning last allocated position: %lld [it:%p]",pos,tail_);
            RETURN_FUNCTION(iterator(tail_,tail_->last_free(),sz));
          }
          else
          {
            uint64_t gp = pos;
            item * p = &head_;

            while( p!=tail_ )
            {
              gp += p->size();
              p = p->next_;
            }

            CSL_DEBUGF(L"last_free() => [it:%p pos:%lld gp:%lld]",tail_,pos,gp);
            RETURN_FUNCTION(iterator(tail_,pos,gp));
          }
        }

        template <typename T> typename inpvec<T>::iterator & inpvec<T>::last_free(iterator & ii)
        {
          ENTER_FUNCTION();
          mul_t mul = (tail_->mul_+2);
          uint64_t pos = tail_->last_free();

          if( pos == tail_->size() )
          {
            uint64_t sz  = size();
            allocate(mul);
            CSL_DEBUGF(L"Allocate. Returning last allocated position: %lld [it:%p]",pos,tail_);
            ii.init( tail_,tail_->last_free(),sz );
          }
          else
          {
            uint64_t gp = pos;
            item * p = &head_;

            while( p!=tail_ )
            {
              gp += p->size();
              p = p->next_;
            }

            CSL_DEBUGF(L"last_free() => ref ii& [it:%p pos:%lld gp:%lld]",tail_,pos,gp);
            ii.init(tail_,pos,gp);
          }
          RETURN_FUNCTION( ii );
        }

        template <typename T> typename inpvec<T>::iterator inpvec<T>::iterator_at(uint64_t pos)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"iterator_at(%lld)",pos);

          if( n_items_ == 0 )
          {
            CSL_DEBUGF(L"no items: returning end()");
            RETURN_FUNCTION( end_ );
          }

          uint64_t px       = pos;
          uint64_t max_pos  = 0;
          item * p          = &head_;

          while( p )
          {
            max_pos += (p->size());
            CSL_DEBUGF(L"Checking at: #%lld <? maxpos:%lld [sz:%lld]",pos,max_pos,p->size());
#ifdef DEBUG
#ifdef DEBUG_VERBOSE
            p->debug();
#endif /*DEBUG_VERBOSE*/
#endif /*DEBUG*/
            if( pos >= max_pos )
            {
              p = p->next_;
            }
            else
            {
              px = (max_pos-p->size());
              CSL_DEBUGF(L"iterator_at(%lld) => [it:%p pos:%lld gp:%lld]",pos,p,pos-px,pos);
              RETURN_FUNCTION( iterator(p,pos-px,pos) );
            }
          }
          CSL_DEBUGF(L"Returning end iterator");
          RETURN_FUNCTION( end_ );
        }

        template <typename T> typename inpvec<T>::iterator inpvec<T>::force_iterator_at(uint64_t pos)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"force_iterator_at(%lld)",pos);
          uint64_t px       = pos;
          mul_t mul   = (tail_->mul_+2);
          uint64_t max_pos  = 0;
          item * p          = &head_;

          while( p )
          {
            max_pos += (p->size());
            CSL_DEBUGF(L"Checking at: #%lld <? maxpos:%lld [sz:%lld]",pos,max_pos,p->size());
            if( pos >= max_pos )
            {
              p = p->next_;
            }
            else
            {
              px = (max_pos-p->size());
              CSL_DEBUGF(L"force_iterator_at(%lld) => [it:%p pos:%lld gp:%lld]",pos,p,pos-px,pos);
              RETURN_FUNCTION( iterator(p,pos-px,pos) );
            }
          }

          px  = (pos-max_pos);
          if( mul < ((px/width_)+2) ) mul = static_cast<mul_t>((px/width_)+2);
          allocate( mul );

          // recursive call : XXX may be dangerous
          RETURN_FUNCTION(force_iterator_at(pos));
        }

        template <typename T> typename inpvec<T>::iterator & inpvec<T>::force_iterator_at(uint64_t pos,iterator & ii)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"force_iterator_at(%lld)",pos);
          uint64_t px       = pos;
          mul_t mul         = (tail_->mul_+2);
          uint64_t max_pos  = 0;
          item * p          = &head_;

          while( p )
          {
            max_pos += (p->size());
            CSL_DEBUGF(L"Checking at: #%lld <? maxpos:%lld [sz:%lld]",pos,max_pos,p->size());
            if( pos >= max_pos )
            {
              p = p->next_;
            }
            else
            {
              px = (max_pos-p->size());
              ii.init( p,pos-px,pos );
              CSL_DEBUGF(L"force_iterator_at(%lld) => ref& [it:%p pos:%lld gp:%lld]",pos,p,pos-px,pos);
              RETURN_FUNCTION( ii );
            }
          }

          px  = (pos-max_pos);
          if( mul < ((px/width_)+2) ) mul = static_cast<mul_t>((px/width_)+2);
          allocate( mul );

          // recursive call : XXX may be dangerous
          RETURN_FUNCTION( force_iterator_at(pos,ii) );
        }

        template <typename T> bool inpvec<T>::free_at(uint64_t pos)
        {
          ENTER_FUNCTION();
          iterator it = iterator_at(pos);
          bool ret = false;
          if( it != end() && it.is_empty() == false )
          {
            it.free();
            ret = true;
          }
          CSL_DEBUGF(L"free_at(%lld) => %s",pos,(ret==true?"true":"false"));
          RETURN_FUNCTION( ret );
        }

        template <typename T> bool inpvec<T>::is_free_at(uint64_t pos)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"is_free_at(%lld)",pos);
          iterator it = iterator_at(pos);
          if( it == end() )
          {
            CSL_DEBUGF(L"No item at invalid position: %lld",pos);
            CSL_DEBUGF(L"is_free_at(%lld) => TRUE",pos);
            RETURN_FUNCTION(true);
          }
          else if( it.is_empty() == true )
          {
            CSL_DEBUGF(L"Empty item at position: %lld",pos);
            CSL_DEBUGF(L"is_free_at(%lld) => TRUE",pos);
            RETURN_FUNCTION(true);
          }
          else
          {
            CSL_DEBUGF(L"Have item at position: %lld",pos);
            CSL_DEBUGF(L"is_free_at(%lld) => FALSE",pos);
            RETURN_FUNCTION(false);
          }
        }

        template <typename T> T & inpvec<T>::get(uint64_t at)
        {
          item * p = &head_;
          uint64_t max_pos = 0;

          while( p != 0 )
          {
            max_pos += (p->size());
            if( at >= max_pos )
            {
              p = p->next_;
            }
            else
            {
              uint64_t pos = (max_pos-p->size());
              return p->get(at-pos);
            }
          }

          /* handle invalid index position */
          THR( exc::rs_invalid_param, (tail_->get(0)) );
        }

        template <typename T> uint64_t inpvec<T>::last_free_pos() const
        {
          ENTER_FUNCTION();
          uint64_t pos = tail_->last_free();

          const item * p = &head_;

          while( p != tail_ )
          {
            pos += p->size();
            p = p->next_;
          }

          CSL_DEBUGF(L"last_free_pos() => %lld",pos);
          RETURN_FUNCTION( pos );
        }

        template <typename T> uint64_t inpvec<T>::first_free_pos() const
        {
          ENTER_FUNCTION();
          uint64_t pos = 0;
          uint64_t f   = 0;

          const item * p = &head_;

          while( p != NULL )
          {
            f = p->first_free();
            pos += f;
            if( f != p->size() ) break;
            p = p->next_;
          }
          CSL_DEBUGF(L"first_free_pos() => %lld",pos);
          RETURN_FUNCTION( pos );
        }

        template <typename T> T * inpvec<T>::get_ptr(uint64_t at)
        {
          ENTER_FUNCTION_X();

          CSL_DEBUGF_X(L"get_ptr(%lld)",at);
          item * p = &head_;
          uint64_t max_pos = 0;
          T * ret = 0;

          while( p != 0 )
          {
            max_pos += (p->size());
            CSL_DEBUGF_X(L"max_pos:%lld += size:%lld => max_pos:%lld",max_pos-p->size(),p->size(),max_pos);
            if( at >= max_pos )
            {
              p = p->next_;
            }
            else
            {
              uint64_t pos = (max_pos-p->size());
              CSL_DEBUGF_X(L"pos:%lld = (max_pos:%lld - size:%lld)",pos,max_pos,p->size());
              CSL_DEBUGF_X(L"at:%lld - pos:%lld = %lld",at,pos,at-pos);
              ret = p->get_ptr(at-pos);
              break;
            }
          }

          CSL_DEBUGF_X(L"get_ptr(%lld) => %p",at,ret);
          RETURN_FUNCTION_X( ret );
        }


        template <typename T> uint64_t inpvec<T>::iterator_pos(const iterator & it)
        {
          ENTER_FUNCTION_X();
          CSL_DEBUGF_X(L"iterator gpos=%lld",it.get_pos());
          RETURN_FUNCTION_X(it.get_pos());
        }


        template <typename T> T * inpvec<T>::set(uint64_t pos,const T & t)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"set(%lld,t)",pos);
          iterator ii;
          RETURN_FUNCTION(force_iterator_at(pos,ii).set(t));
        }

        template <typename T> T * inpvec<T>::push_back(const T & t)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"push_back(t)");
          iterator ii;
          RETURN_FUNCTION(last_free(ii).set(t));
        }

        template <typename T> T * inpvec<T>::construct(uint64_t pos)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"construct(%lld)",pos);
          iterator ii;
          RETURN_FUNCTION( force_iterator_at(pos,ii).construct() );
        }

  } /* end of ns:common */
} /* end of ns:csl */

#endif /* __cplusplus */
#endif /* _csl_common_inpvec_impl_hh_included_ */
