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

#ifndef _csl_common_inpvec_hh_included_
#define _csl_common_inpvec_hh_included_

/**
   @file inpvec.hh
   @brief vector that utilizes in-place construction
 */

#include "logger.hh"
#include "hlprs.hh"
#include "exc.hh"
#include "common.h"
#ifdef __cplusplus

namespace csl
{
  namespace common
  {
    template <typename T> class inpvec
    {
      public:
        // note: if width is changed, it implies lots of changes in the code....
        static const uint16_t  width_         = 64;
        static const uint16_t  width_in_bits_ = 6;
        typedef uint64_t       bitmap_t;

        struct item
        {
          CSL_OBJ(csl::common,inpvec::item);

          uint8_t     free_;
          uint16_t    mul_;
          bitmap_t  * bmap_;
          T         * items_;
          item *      next_;
          inpvec *    parent_;

          ~item()
          {
            destroy();
            if( free_ ) { free(bmap_); free(items_); }
          }

          uint64_t size() const { return mul_*width_; }

          uint64_t n_items() const
          {
            const unsigned char * bb = byte_bits();

            uint64_t ret = 0;
            for( uint32_t i=0;i<mul_; ++i )
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

          uint64_t last_free() const
          {
            ENTER_FUNCTION();
            const unsigned char * bl = byte_last_free();
            uint64_t ret = size();
            uint8_t x1,x2,x3,x4;
            uint8_t y1,y2,y3,y4;

            for( int32_t i=(mul_-1);i>=0;--i )
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
#if 0
          uint64_t last_used() const
          {
            const unsigned char * bl = byte_last_used();
            uint64_t ret = size();

            for( int32_t i=(mul_-1);i>=0;--i )
            {
              if( bmap_[i] == 0x0UL ) { }
              else
              {
                TODO
                if( bl[(bmap_[i]>>24)&0xff] == 255 )       return ret;
                else if( bl[(bmap_[i]>>24)&0xff] != 0   )  return i*32+24+bl[(bmap_[i]>>24)&0xff];
                else                                       ret = i*32+24;

                if( bl[(bmap_[i]>>16)&0xff] == 255 )       return ret;
                else if( bl[(bmap_[i]>>16)&0xff] != 0   )  return i*32+16+bl[(bmap_[i]>>16)&0xff];
                else                                       ret = i*32+16;

                if( bl[(bmap_[i]>>8)&0xff] == 255 )        return ret;
                else if( bl[(bmap_[i]>>8)&0xff] != 0   )   return i*32+8+bl[(bmap_[i]>>8)&0xff];
                else                                       ret = i*32+8;

                if( bl[(bmap_[i])&0xff] == 255 )           return ret;
                else if( bl[(bmap_[i])&0xff] != 0   )      return i*32+bl[(bmap_[i])&0xff];
                else                                       ret = i*32;
              }
            }
            return ret;
          }
#endif

          uint64_t n_free() const { return ((width_*mul_)-n_items()); }

          void destroy()
          {
            uint64_t n_i = 0;
            uint64_t * px = &n_i;
            if( parent_ ) px = &(parent_->n_items_);

            for( uint32_t i=0;i<mul_; ++i )
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
          }

          void destroy( uint64_t at )
          {
            uint64_t off = at/width_;
            uint64_t pos = at%width_;

            if( (bmap_[off]>>pos)&static_cast<bitmap_t>(1ULL) )
            {
              in_place_destruct( items_+at );
              bmap_[off] = (bmap_[off] & (~(static_cast<bitmap_t>(1ULL)<<pos)));
              if( parent_ ) --(parent_->n_items_);
            }
          }

          void mul_alloc( uint32_t m )
          {
            mul_         = m;
            free_        = 1;
            items_       = reinterpret_cast<T *>(malloc(m*width_*sizeof(T)));
            bmap_        = reinterpret_cast<bitmap_t *>(malloc(m*sizeof(bitmap_t)));
            next_        = 0;
            parent_      = 0;
            memset( bmap_,0,(m*sizeof(bitmap_t)) );
          }

          inline void in_place_destruct(signed char * d) {}
          inline void in_place_destruct(unsigned char * d) {}
          inline void in_place_destruct(unsigned short * d) {}
          inline void in_place_destruct(signed short * d) {}
          inline void in_place_destruct(unsigned int * d) {}
          inline void in_place_destruct(signed int * d) {}
          inline void in_place_destruct(unsigned long * d) {}
          inline void in_place_destruct(signed long * d) {}
          inline void in_place_destruct(unsigned long long * d) {}
          inline void in_place_destruct(signed long long * d) {}

          template <typename DESTR> inline void in_place_destruct(DESTR * d)
          {
            d->~DESTR();
          }

          T * construct(uint64_t at)
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

          T * set(uint64_t at, const T & t)
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

          template <typename T1>
          T * set(uint64_t at, const T1 & t1)
          {
            ENTER_FUNCTION();
            uint64_t off = at/width_;
            uint64_t pos = at%width_;

            CSL_DEBUGF(L"set(%lld,%p) => [off:%lld pos:%lld mul:%d free:%d]",at,&t1,off,pos,mul_,free_);

            if( (bmap_[off]>>pos)&static_cast<bitmap_t>(1ULL) )
            { // already have an item
              items_[at] = t1;
            }
            else
            { // "allocate" = copy construct an item
              new (items_+at) T(t1);
              bmap_[off] = (bmap_[off]) | (static_cast<bitmap_t>(1ULL)<<pos);
              if( parent_ ) ++(parent_->n_items_);
            }
            RETURN_FUNCTION(items_+at);
          }

          template <typename T1, typename T2>
          T * set(uint64_t at, const T1 & t1, const T2 & t2)
          {
            ENTER_FUNCTION();
            uint64_t off = at/width_;
            uint64_t pos = at%width_;

            CSL_DEBUGF(L"set(%lld,%p,%p) => [off:%lld pos:%lld mul:%d free:%d]",at,&t1,&t2,off,pos,mul_,free_);

            if( (bmap_[off]>>pos)&static_cast<bitmap_t>(1ULL) )
            { // already have an item
              items_[at] = T(t1,t2);
            }
            else
            { // "allocate" = copy construct an item
              new (items_+at) T(t1,t2);
              bmap_[off] = (bmap_[off]) | (static_cast<bitmap_t>(1ULL)<<pos);
              if( parent_ ) ++(parent_->n_items_);
            }
            RETURN_FUNCTION(items_+at);
          }

          template <typename T1, typename T2, typename T3>
          T * set(uint64_t at, const T1 & t1, const T2 & t2, const T3 & t3)
          {
            ENTER_FUNCTION();
            uint64_t off = at/width_;
            uint64_t pos = at%width_;

            CSL_DEBUGF(L"set(%lld,%p,%p,%p) => [off:%lld pos:%lld mul:%d free:%d]",
                       at,&t1,&t2,&t3,off,pos,mul_,free_);

            if( (bmap_[off]>>pos)&static_cast<bitmap_t>(1ULL) )
            { // already have an item
              items_[at] = T(t1,t2,t3);
            }
            else
            { // "allocate" = copy construct an item
              new (items_+at) T(t1,t2,t3);
              bmap_[off] = (bmap_[off]) | (static_cast<bitmap_t>(1ULL)<<pos);
              if( parent_ ) ++(parent_->n_items_);
            }
            RETURN_FUNCTION(items_+at);
          }

          template <typename T1, typename T2, typename T3, typename T4>
          T * set(uint64_t at, const T1 & t1, const T2 & t2, const T3 & t3, const T4 & t4)
          {
            ENTER_FUNCTION();
            uint64_t off = at/width_;
            uint64_t pos = at%width_;

            CSL_DEBUGF(L"set(%lld,%p,%p,%p,%p) => [off:%lld pos:%lld mul:%d free:%d]",
                       at,&t1,&t2,&t3,&t4,off,pos,mul_,free_);

            if( (bmap_[off]>>pos)&static_cast<bitmap_t>(1ULL) )
            { // already have an item
              items_[at] = T(t1,t2,t3,t4);
            }
            else
            { // "allocate" = copy construct an item
              new (items_+at) T(t1,t2,t3,t4);
              bmap_[off] = (bmap_[off]) | (static_cast<bitmap_t>(1ULL)<<pos);
              if( parent_ ) ++(parent_->n_items_);
            }
            RETURN_FUNCTION(items_+at);
          }

          bool is_empty() const
          {
            ENTER_FUNCTION();
            bool ret = true;
            for( uint16_t j=0;j<mul_;++j )
            {
              if( bmap_[j] != 0xFFFFFFFFUL )
              {
                ret = false;
                break;
              }
            }
            CSL_DEBUGF(L"is_empty() => %s",(ret == true?"true":"false"));
            RETURN_FUNCTION( ret );
          }

          bool is_empty(uint64_t at) const
          {
            ENTER_FUNCTION();
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
            CSL_DEBUGF(L"is_empty(%lld) [%lld:%lld] => %s",at,off,pos,(ret == true?"true":"false"));
            RETURN_FUNCTION( ret );
          }

          void debug()
          {
#ifdef DEBUG
            ENTER_FUNCTION();
            for( uint64_t i=0;i<mul_;++i )
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

          bool is_last(uint64_t at) const
          {
            ENTER_FUNCTION();
            uint64_t off = at/width_;
            uint64_t pos = at%width_;

            CSL_DEBUGF(L"position: %lld [%lld:%lld mul:%d]",at,off,pos,mul_);

            if( off > mul_ )
            {
              CSL_DEBUGF(L"invalid position: %lld",at);
              RETURN_FUNCTION( true );
            }

            unsigned char i,j;

            for( j=pos+1;j<width_;++j )
            {
              if( (bmap_[off]>>j)&static_cast<bitmap_t>(1ULL) )
              {
                CSL_DEBUGF(L"ret 1: %d",j);
                RETURN_FUNCTION( false );
              }
            }

            for( i=off+1;i<mul_;++i )
            {
              if( bmap_[i] == 0x0UL ) continue;

              for( j=0;j<width_;++j )
              {
                if( (bmap_[i]>>j)&static_cast<bitmap_t>(1ULL) )
                {
                  CSL_DEBUGF(L"ret 2: [i:%d j:%d]",i,j);
                  RETURN_FUNCTION( false );
                }
              }
            }
            CSL_DEBUGF(L"ret 3");
            RETURN_FUNCTION( true );
          }

          inline T & get(uint64_t at) const
          {
            ENTER_FUNCTION();
            T * ret = (items_+at);
            CSL_DEBUGF(L"returning ref to: %p at %lld",ret,at);
            RETURN_FUNCTION( *ret );
          }

          inline T * get_ptr(uint64_t at) const
          {
            ENTER_FUNCTION();
            uint64_t off = at/width_;
            uint64_t pos = at%width_;

            CSL_DEBUGF(L"get_ptr(%lld) [off:%lld pos:%lld]",at,off,pos);

            T * ret = 0;

            if( (bmap_[off]>>pos)&static_cast<bitmap_t>(1ULL) ) { ret = items_+at; }

            CSL_DEBUGF(L"returning ptr: %p at %lld",ret,at);

            RETURN_FUNCTION( ret );
          }

          T * next_used(uint64_t & at)
          {
            ENTER_FUNCTION();
            uint64_t newpos   = at+1;
            uint64_t off      = (newpos)/width_;
            uint64_t pos      = (newpos)%width_;
            CSL_DEBUGF(L"next_used(pos: %lld) [off:%lld pos:%lld]",at,off,pos);

            T * ret = 0;

            uint16_t i,j;

            for( i=off;i<mul_;++i )
            {
              if( bmap_[i] == 0ULL )
              {
                CSL_DEBUGF(L"Skip %d positions",width_);
              }
              else
              {
                for( j=pos;j<width_;++j )
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
        };

        friend struct item;

      private:
        bitmap_t         pre_bmap_;
        unsigned char *  pre_items_[width_*sizeof(T)];

        uint64_t  n_items_;
        item      head_;
        item *    tail_;

        void allocate()
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"allocate()");
          allocate( (tail_->mul_+2) );
          LEAVE_FUNCTION();
        }

        void allocate(uint64_t mul)
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

      public:

        class iterator
        {
          public:
            friend class inpvec;

          private:
            item *    i_;
            uint64_t  pos_;
            uint64_t  gpos_;

            iterator()
            {
              ENTER_FUNCTION();
              CSL_DEBUGF(L"iterator()");
              LEAVE_FUNCTION();
            }

          public:
            /// @brief initializer constructor
            inline iterator(item * i, uint64_t pos, uint64_t gp) : i_(i), pos_(pos), gpos_(gp)
            {
              ENTER_FUNCTION();
              CSL_DEBUGF(L"iterator(item:%p, pos:%lld, gpos:%lld)",i,pos,gp);
              LEAVE_FUNCTION();
            }

            void init(item * i, uint64_t pos, uint64_t gp)
            {
              ENTER_FUNCTION();
              CSL_DEBUGF(L"init(%p,%lld,%lld)",i,pos,gp);
              i_ = i; pos_ = pos; gpos_ = gp;
              LEAVE_FUNCTION();
            }

            /// @brief copy constructor
            inline iterator(const iterator & other) : i_(other.i_), pos_(other.pos_), gpos_(other.gpos_)
            {
              ENTER_FUNCTION();
              CSL_DEBUGF(L"iterator(iterator ref& %p)",&other);
              LEAVE_FUNCTION();
            }

            /// @brief copy operator
            inline iterator & operator=(const iterator & other)
            {
              ENTER_FUNCTION();
              CSL_DEBUGF(L"operator=(iterator ref& %p)",&other);
              i_     = other.i_;
              pos_   = other.pos_;
              gpos_  = other.gpos_;
              RETURN_FUNCTION(*this);
            }

            /// @brief creates an iterator of ls
            /// @param ls is the pvlist to be iterated over
            inline iterator(inpvec & ipv) : i_(&(ipv.head_)), pos_(0), gpos_(0)
            {
              ENTER_FUNCTION();
              CSL_DEBUGF(L"iterator(inpvec ref& %p)",&ipv);
              if(!ipv.n_items_) i_ = 0;
              LEAVE_FUNCTION();
            }

            /// @brief checks equality
            bool operator==(const iterator & other) const
            {
              ENTER_FUNCTION();
              CSL_DEBUGF(L"operator==(iterator ref& %p)",&other);
              RETURN_FUNCTION((i_ == other.i_ && gpos_ == other.gpos_) ? true : false );
            }

            /// @brief checks if not equal
            bool operator!=(const iterator & other) const
            {
              ENTER_FUNCTION();
              CSL_DEBUGF(L"operator!=(iterator ref& %p)",&other);
              RETURN_FUNCTION(!(operator==(other)));
            }

            /// @brief step forward
            void operator++()
            {
              ENTER_FUNCTION();
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
              LEAVE_FUNCTION();
            }

            T * next_used()
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

            /// @brief sets the iterator to end
            void zero()
            {
              ENTER_FUNCTION();
              i_ = 0; pos_ = 0; gpos_ = 0;
              LEAVE_FUNCTION();
            }

            bool at_end() const
            {
              ENTER_FUNCTION();
              RETURN_FUNCTION((i_ == 0 && pos_ == 0));
            }

            /// @brief returns the pointed item
            T * operator*()
            {
              ENTER_FUNCTION();
              T * ret = 0;
              if( i_ ) { ret = ( i_->get_ptr(pos_) ); }
              else
              {
                CSL_DEBUGF(L"i_ is null: %s",((pos_==0 && gpos_==0)?"(end iterator)":"(unknown error)") );
              }
              RETURN_FUNCTION( ret );
            }

            /// @brief checks if the item is empty
            bool is_empty() const
            {
              ENTER_FUNCTION();
              bool ret = true;
              if( i_ ) { ret = i_->is_empty(pos_); }
              CSL_DEBUGF(L"is_empty() : %s",(ret == true?"true":"false"));
              RETURN_FUNCTION( ret );
            }

            /// @brief free item at the given iterator position
            void free()
            {
              ENTER_FUNCTION();
              if( i_ ) { i_->destroy( pos_ ); }
              LEAVE_FUNCTION();
            }

            uint64_t n_free() const
            {
              ENTER_FUNCTION();
              if( i_ ) { RETURN_FUNCTION( i_->n_free() ); }
              else     { RETURN_FUNCTION( 0 ); }
            }

            uint64_t get_pos() const
            {
              ENTER_FUNCTION();
              CSL_DEBUGF(L"Returning position: %lld",gpos_);
              RETURN_FUNCTION( gpos_ );
            }

            /// @brief default construct the item at the iterator position
            T * construct()
            {
              ENTER_FUNCTION();
              T * ret = 0;
              if( i_ ) ret = i_->construct( pos_ );
              CSL_DEBUG_ASSERT( i_ != 0 );
              RETURN_FUNCTION( ret );
            }

            /// @brief sets the item at the iterator position
            T * set(const T & t)
            {
              ENTER_FUNCTION();
              T * ret = 0;
              if( i_ ) ret = i_->set( pos_,t );
              CSL_DEBUG_ASSERT( i_ != 0 );
              RETURN_FUNCTION( ret );
            }

            /// @brief sets the item at the iterator position
            template <typename T1>
            T * set(const T1 & t1)
            {
              ENTER_FUNCTION();
              T * ret = 0;
              if( i_ ) ret = i_->set( pos_,t1 );
              CSL_DEBUG_ASSERT( i_ != 0 );
              RETURN_FUNCTION( ret );
            }

            /// @brief sets the item at the iterator position
            template <typename T1,typename T2>
            T * set(const T1 & t1,const T2 & t2)
            {
              ENTER_FUNCTION();
              T * ret = 0;
              if( i_ ) ret = i_->set( pos_,t1,t2 );
              CSL_DEBUG_ASSERT( i_ != 0 );
              RETURN_FUNCTION( ret );
            }

            /// @brief sets the item at the iterator position
            template <typename T1,typename T2,typename T3>
            T * set(const T1 & t1,const T2 & t2,const T3 & t3)
            {
              ENTER_FUNCTION();
              T * ret = 0;
              if( i_ ) ret = i_->set( pos_,t1,t2,t3 );
              CSL_DEBUG_ASSERT( i_ != 0 );
              RETURN_FUNCTION( ret );
            }

            /// @brief sets the item at the iterator position
            template <typename T1,typename T2,typename T3,typename T4>
            T * set(const T1 & t1,const T2 & t2,const T3 & t3,const T4 & t4)
            {
              ENTER_FUNCTION();
              T * ret = 0;
              if( i_ ) ret = i_->set( pos_,t1,t2,t3,t4 );
              CSL_DEBUG_ASSERT( i_ != 0 );
              RETURN_FUNCTION( ret );
            }

            CSL_OBJ(csl::common,inpvec::iterator);
        };

      private:
        iterator begin_;
        iterator end_;

      public:
        /** @brief returns iterator represents the end of this container */
        const iterator & end()
        {
          ENTER_FUNCTION( );
          RETURN_FUNCTION( end_ );
        }

        /** @brief returns iterator pointed at the beginning of the container */
        const iterator & begin()
        {
          ENTER_FUNCTION( );
          if( n_items_ == 0 )
          {
            CSL_DEBUGF(L"no items: returning end_");
            RETURN_FUNCTION( end_ );
          }
          RETURN_FUNCTION( begin_ );
        }

        ~inpvec()
        {
          ENTER_FUNCTION( );
          item * p = head_.next_;
          item * x = p;

          while( p )
          {
            x = p;
            p = p->next_;
            delete x;
          }
          LEAVE_FUNCTION( );
        }

        inpvec() : pre_bmap_(0), begin_(&head_,0,0), end_(0,0,0)
        {
          ENTER_FUNCTION( );
          CSL_DEBUGF(L"inpvec()");
          head_.free_        = 0;
          head_.mul_         = 1;
          head_.bmap_        = &pre_bmap_;
          head_.items_       = reinterpret_cast<T *>(pre_items_);
          head_.next_        = 0;
          head_.parent_      = this;
          tail_              = &head_;
          n_items_           = 0;
          LEAVE_FUNCTION( );
        }

        uint64_t n_items()
        {
          ENTER_FUNCTION( );
          CSL_DEBUGF(L"n_items() => %lld",n_items_);
          RETURN_FUNCTION( n_items_ );
        }

        uint64_t size()
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

        void debug()
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

        iterator last_free()
        {
          ENTER_FUNCTION();
          uint64_t mul = (tail_->mul_+2);
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

        iterator & last_free(iterator & ii)
        {
          ENTER_FUNCTION();
          uint64_t mul = (tail_->mul_+2);
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

        iterator iterator_at(uint64_t pos)
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
            p->debug();
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

        iterator force_iterator_at(uint64_t pos)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"force_iterator_at(%lld)",pos);
          uint64_t px       = pos;
          uint64_t mul      = (tail_->mul_+2);
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
          if( mul < ((px/width_)+2) ) mul = (px/width_)+2;
          allocate( mul );

          // recursive call : XXX may be dangerous
          RETURN_FUNCTION(force_iterator_at(pos));
        }

        iterator & force_iterator_at(uint64_t pos,iterator & ii)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"force_iterator_at(%lld)",pos);
          uint64_t px       = pos;
          uint64_t mul      = (tail_->mul_+2);
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
          if( mul < ((px/width_)+2) ) mul = (px/width_)+2;
          allocate( mul );

          // recursive call : XXX may be dangerous
          RETURN_FUNCTION( force_iterator_at(pos,ii) );
        }

        bool free_at(uint64_t pos)
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

        bool is_free_at(uint64_t pos)
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

        T & get(uint64_t at)
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

        uint64_t last_free_pos() const
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

        T * get_ptr(uint64_t at)
        {
          ENTER_FUNCTION();

          CSL_DEBUGF(L"get_ptr(%lld)",at);
          item * p = &head_;
          uint64_t max_pos = 0;
          T * ret = 0;

          while( p != 0 )
          {
            max_pos += (p->size());
            CSL_DEBUGF(L"max_pos:%lld += size:%lld => max_pos:%lld",max_pos-p->size(),p->size(),max_pos);
            if( at >= max_pos )
            {
              p = p->next_;
            }
            else
            {
              uint64_t pos = (max_pos-p->size());
              CSL_DEBUGF(L"pos:%lld = (max_pos:%lld - size:%lld)",pos,max_pos,p->size());
              CSL_DEBUGF(L"at:%lld - pos:%lld = %lld",at,pos,at-pos);
              ret = p->get_ptr(at-pos);
              break;
            }
          }

          CSL_DEBUGF(L"get_ptr(%lld) => %p",at,ret);
          RETURN_FUNCTION( ret );
        }

        uint64_t iterator_pos(const iterator & it)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"iterator gpos=%lld",it.get_pos());
          RETURN_FUNCTION(it.get_pos());
        }

        T * push_back(const T & t)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"push_back(t)");
          iterator ii;
          RETURN_FUNCTION(last_free(ii).set(t));
        }

        template <typename T1>
        T * push_back(const T1 & t1)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"push_back(t1)");
          iterator ii;
          RETURN_FUNCTION(last_free(ii).set(t1));
        }

        template <typename T1,typename T2>
        T * push_back(const T1 & t1,const T2 & t2)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"push_back(t1,t2)");
          iterator ii;
          RETURN_FUNCTION(last_free(ii).set(t1,t2));
        }

        template <typename T1,typename T2,typename T3>
        T * push_back(const T1 & t1,const T2 & t2,const T3 & t3)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"push_back(t1,t2,t3)");
          iterator ii;
          RETURN_FUNCTION(last_free(ii).set(t1,t2,t3));
        }

        template <typename T1,typename T2,typename T3,typename T4>
        T * push_back(const T1 & t1,const T2 & t2,const T3 & t3,const T4 & t4)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"push_back(t1,t2,t3,t4)");
          iterator ii;
          RETURN_FUNCTION(last_free(ii).set(t1,t2,t3,t4));
        }

        T * construct(uint64_t pos)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"construct(%lld)",pos);
          iterator ii;
          RETURN_FUNCTION( force_iterator_at(pos,ii).construct() );
        }

        T * set(uint64_t pos,const T & t)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"set(%lld,t)",pos);
          iterator ii;
          RETURN_FUNCTION(force_iterator_at(pos,ii).set(t));
        }

        template <typename T1>
        T * set(uint64_t pos, const T1 & t1)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"set(%lld,t1)",pos);
          iterator ii;
          RETURN_FUNCTION(force_iterator_at(pos,ii).set(t1));
        }

        template <typename T1,typename T2>
        T * set(uint64_t pos, const T1 & t1,const T2 & t2)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"set(%lld,t1,t2)",pos);
          iterator ii;
          RETURN_FUNCTION(force_iterator_at(pos,ii).set(t1,t2));
        }

        template <typename T1,typename T2,typename T3>
        T * set(uint64_t pos, const T1 & t1,const T2 & t2,const T3 & t3)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"set(%lld,t1,t2,t3)",pos);
          iterator ii;
          RETURN_FUNCTION(force_iterator_at(pos,ii).set(t1,t2,t3));
        }

        template <typename T1,typename T2,typename T3,typename T4>
        T * set(uint64_t pos, const T1 & t1,const T2 & t2,const T3 & t3,const T4 & t4)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"set(%lld,t1,t2,t3,t4)",pos);
          iterator ii;
          RETURN_FUNCTION(force_iterator_at(pos,ii).set(t1,t2,t3,t4));
        }

        CSL_OBJ(csl::common,inpvec);
        USE_EXC();
    };
  } /* end of ns:common */
} /* end of ns:csl */

#endif /* __cplusplus */
#endif /* _csl_common_inpvec_hh_included_ */
