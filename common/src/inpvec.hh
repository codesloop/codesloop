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
        static const uint16_t  width_ = 32;
        static const uint16_t  width_in_bits_ = 5;
        typedef uint32_t       bitmap_t;

        struct item
        {
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
            }
            return ret;
          }

          uint64_t last_free() const
          {
            ENTER_FUNCTION();
            const unsigned char * bl = byte_last_free();
            uint64_t ret = size();
            uint8_t x1,x2,x3,x4;

            for( int32_t i=(mul_-1);i>=0;--i )
            {
              if( bmap_[i] == 0x0UL ) ret = i*32;
              else
              {
                if( (x1 = (bl[(bmap_[i]>>24)&0xff])) == 0 )  { ret = i*32+24;           }
                else                                         { ret = i*32+24+x1; break; }
                if( (x2 = (bl[(bmap_[i]>>16)&0xff])) == 0 )  { ret = i*32+16;           }
                else                                         { ret = i*32+16+x2; break; }
                if( (x3 = (bl[(bmap_[i]>>8)&0xff])) == 0 )   { ret = i*32+8;            }
                else                                         { ret = i*32+8+x3;  break; }
                if( (x4 = (bl[(bmap_[i])&0xff])) == 0 )      { ret = i*32;              }
                else                                         { ret = i*32+x4;    break; }
              }
            }

            RETURN_FUNCTION(ret);
          }

          uint64_t last_used() const
          {
            const unsigned char * bl = byte_last_used();
            uint64_t ret = size();

            for( int32_t i=(mul_-1);i>=0;--i )
            {
              if( bmap_[i] == 0x0UL ) { }
              else
              {
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
                  if( (bmap_[i]>>j)&static_cast<bitmap_t>(1UL) )
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

            if( (bmap_[off]>>pos)&static_cast<bitmap_t>(1UL) )
            {
              in_place_destruct( items_+at );
              bmap_[off] = (bmap_[off] & (~(static_cast<bitmap_t>(1UL)<<pos)));
              if( parent_ ) --(parent_->n_items_);
            }
          }

          void mul_alloc(uint32_t m)
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
            uint64_t off = at/width_;
            uint64_t pos = at%width_;

            if( (bmap_[off]>>pos)&static_cast<bitmap_t>(1UL) )
            { // already have an item
            }
            else
            { // "allocate" = default construct an item
              new (items_+at) T();
              bmap_[off] = (bmap_[off]) | (static_cast<bitmap_t>(1UL)<<pos);
              if( parent_ ) ++(parent_->n_items_);
            }
            return (items_+at);
          }

          T * set(uint64_t at, const T & t)
          {
            uint64_t off = at/width_;
            uint64_t pos = at%width_;

            if( (bmap_[off]>>pos)&static_cast<bitmap_t>(1UL) )
            { // already have an item
              items_[at] = t;
            }
            else
            { // "allocate" = copy construct an item
              new (items_+at) T(t);
              bmap_[off] = (bmap_[off]) | (static_cast<bitmap_t>(1UL)<<pos);
              if( parent_ ) ++(parent_->n_items_);
            }
            return (items_+at);
          }

          template <typename T1>
          T * set(uint64_t at, const T1 & t1)
          {
            uint64_t off = at/width_;
            uint64_t pos = at%width_;

            if( (bmap_[off]>>pos)&static_cast<bitmap_t>(1UL) )
            { // already have an item
              items_[at] = t1;
            }
            else
            { // "allocate" = copy construct an item
              new (items_+at) T(t1);
              bmap_[off] = (bmap_[off]) | (static_cast<bitmap_t>(1UL)<<pos);
              if( parent_ ) ++(parent_->n_items_);
            }
            return (items_+at);
          }

          template <typename T1, typename T2>
          T * set(uint64_t at, const T1 & t1, const T2 & t2)
          {
            uint64_t off = at/width_;
            uint64_t pos = at%width_;

            if( (bmap_[off]>>pos)&static_cast<bitmap_t>(1UL) )
            { // already have an item
              items_[at] = T(t1,t2);
            }
            else
            { // "allocate" = copy construct an item
              new (items_+at) T(t1,t2);
              bmap_[off] = (bmap_[off]) | (static_cast<bitmap_t>(1UL)<<pos);
              if( parent_ ) ++(parent_->n_items_);
            }
            return (items_+at);
          }

          template <typename T1, typename T2, typename T3>
          T * set(uint64_t at, const T1 & t1, const T2 & t2, const T3 & t3)
          {
            uint64_t off = at/width_;
            uint64_t pos = at%width_;

            if( (bmap_[off]>>pos)&static_cast<bitmap_t>(1UL) )
            { // already have an item
              items_[at] = T(t1,t2,t3);
            }
            else
            { // "allocate" = copy construct an item
              new (items_+at) T(t1,t2,t3);
              bmap_[off] = (bmap_[off]) | (static_cast<bitmap_t>(1UL)<<pos);
              if( parent_ ) ++(parent_->n_items_);
            }
            return (items_+at);
          }

          template <typename T1, typename T2, typename T3, typename T4>
          T * set(uint64_t at, const T1 & t1, const T2 & t2, const T3 & t3, const T4 & t4)
          {
            uint64_t off = at/width_;
            uint64_t pos = at%width_;

            if( (bmap_[off]>>pos)&static_cast<bitmap_t>(1UL) )
            { // already have an item
              items_[at] = T(t1,t2,t3,t4);
            }
            else
            { // "allocate" = copy construct an item
              new (items_+at) T(t1,t2,t3,t4);
              bmap_[off] = (bmap_[off]) | (static_cast<bitmap_t>(1UL)<<pos);
              if( parent_ ) ++(parent_->n_items_);
            }
            return (items_+at);
          }

          bool is_empty() const
          {
            for( uint16_t j=0;j<mul_;++j )
            {
              if( bmap_[j] != 0xFFFFFFFFUL ) return false;
            }
            return true;
          }

          bool is_empty(uint64_t at) const
          {
            uint64_t off = at/width_;
            uint64_t pos = at%width_;

            if( off > mul_ ) return true;

            if( (bmap_[off]>>pos)&static_cast<bitmap_t>(1) )
            {
              return false;
            }
            else
            {
              return true;
            }
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
                if( (bmap_[i]>>j)&static_cast<bitmap_t>(1) ) xd[j] = 'X';
                else                                         xd[j] = '.';
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
            uint64_t off = at/width_;
            uint64_t pos = at%width_;

            if( off > mul_ ) return true;

            unsigned char i,j;

            for( j=pos+1;j<width_;++j )
            {
              if( (bmap_[off]>>j)&static_cast<bitmap_t>(1UL) )
              {
                return false;
              }
            }

            for( i=off+1;i<mul_;++i )
            {
              if( bmap_[i] == 0x0UL ) continue;

              for( j=0;j<width_;++j )
              {
                if( (bmap_[i]>>j)&static_cast<bitmap_t>(1UL) )
                {
                  return false;
                }
              }
            }
            return true;
          }

          inline T & get(uint64_t at) const { return *(items_+at); }

          inline T * get_ptr(uint64_t at) const
          {
            uint64_t off = at/width_;
            uint64_t pos = at%width_;

            if( (bmap_[off]>>pos)&static_cast<bitmap_t>(1) )
            {
              return items_+at;
            }
            else
            {
              return 0;
            }
          }
        };

        friend struct item;

      private:
        bitmap_t         pre_bmap_;
        unsigned char *  pre_items_[width_*sizeof(T)];

        uint64_t  n_items_;
        item      head_;
        item *    tail_;

        void allocate() { allocate( (tail_->mul_+2) ); }

        void allocate(uint64_t mul)
        {
          item * n = new item( );
          n->mul_alloc( mul );
          n->parent_ = this;
          tail_->next_ = n;
          tail_ = n;
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

            iterator() {}

          public:
            /// @brief initializer constructor
            inline iterator(item * i, uint64_t pos, uint64_t gp) : i_(i), pos_(pos), gpos_(gp) {}

            void init(item * i, uint64_t pos, uint64_t gp) { i_ = i; pos_ = pos; gpos_ = gp; }

            /// @brief copy constructor
            iterator(const iterator & other) : i_(other.i_), pos_(other.pos_), gpos_(other.gpos_) {}

            /// @brief copy operator
            iterator & operator=(const iterator & other)
            {
              i_     = other.i_;
              pos_   = other.pos_;
              gpos_  = other.gpos_;
              return *this;
            }

            /// @brief creates an iterator of ls
            /// @param ls is the pvlist to be iterated over
            iterator(inpvec & ipv) : i_(&(ipv.head_)), pos_(0), gpos_(0)
            {
              if(!ipv.n_items_) i_ = 0;
            }

            /// @brief checks equality
            bool operator==(const iterator & other) const
            {
              return ((i_ == other.i_ && gpos_ == other.gpos_) ? true : false );
            }

            /// @brief checks if not equal
            bool operator!=(const iterator & other) const
            {
              return (!(operator==(other)));
            }

            /// @brief step forward
            void operator++()
            {
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
            }

            /// @brief sets the iterator to end
            void zero() { i_ = 0; pos_ = 0; gpos_ = 0; }

            bool at_end() const { return (i_ == 0 && pos_ == 0); }

            /// @brief returns the pointed item
            T * operator*()
            {
              if( i_ ) { return i_->get_ptr(pos_); }
              else     { return 0;                 }
            }

            /// @brief checks if the item is empty
            bool is_empty() const
            {
              if( i_ ) { return i_->is_empty(pos_); }
              else     { return true; }
            }

            /// @brief free item at the given iterator position
            void free()
            {
              if( i_ ) { i_->destroy( pos_ ); }
            }

            uint64_t n_free() const
            {
              if( i_ ) { return i_->n_free(); }
              else     { return 0; }
            }

            uint64_t get_pos() const { return gpos_; }

            /// @brief default construct the item at the iterator position
            T * construct()
            {
              if( !i_ ) return 0;
              return i_->construct( pos_ );
            }

            /// @brief sets the item at the iterator position
            T * set(const T & t)
            {
              if( !i_ ) return 0;
              return i_->set( pos_,t );
            }

            /// @brief sets the item at the iterator position
            template <typename T1>
            T * set(const T1 & t1)
            {
              if( !i_ ) return 0;
              return i_->set( pos_,t1 );
            }

            /// @brief sets the item at the iterator position
            template <typename T1,typename T2>
            T * set(const T1 & t1,const T2 & t2)
            {
              if( !i_ ) return 0;
              return i_->set( pos_,t1,t2 );
            }

            /// @brief sets the item at the iterator position
            template <typename T1,typename T2,typename T3>
            T * set(const T1 & t1,const T2 & t2,const T3 & t3)
            {
              if( !i_ ) return 0;
              return i_->set( pos_,t1,t2,t3 );
            }

            /// @brief sets the item at the iterator position
            template <typename T1,typename T2,typename T3,typename T4>
            T * set(const T1 & t1,const T2 & t2,const T3 & t3,const T4 & t4)
            {
              if( !i_ ) return 0;
              return i_->set( pos_,t1,t2,t3,t4 );
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
          return end_;
        }

        /** @brief returns iterator pointed at the beginning of the container */
        const iterator & begin()
        {
          if( n_items_ == 0 ) return end_;
          return begin_;
        }

        ~inpvec()
        {
          item * p = head_.next_;
          item * x = p;

          while( p )
          {
            x = p;
            p = p->next_;
            delete x;
          }
        }

        inpvec() : pre_bmap_(0), begin_(&head_,0,0), end_(0,0,0)
        {
          head_.free_        = 0;
          head_.mul_         = 1;
          head_.bmap_        = &pre_bmap_;
          head_.items_       = reinterpret_cast<T *>(pre_items_);
          head_.next_        = 0;
          head_.parent_      = this;
          tail_              = &head_;
          n_items_           = 0;
        }

        uint64_t n_items() { return n_items_; }

        uint64_t size()
        {
          uint64_t ret = 0;
          item * p = &head_;
          while( p != 0 )
          {
            ret += ((p->mul_)*width_);
            p = p->next_;
          }
          return ret;
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

            CSL_DEBUGF(L"Return pos: %lld [it:%p gp:%lld]",pos,tail_,gp);
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

            CSL_DEBUGF(L"Return pos: %lld [it:%p gp:%lld]",pos,tail_,gp);
            ii.init(tail_,pos,gp);
          }
          RETURN_FUNCTION( ii );
        }

        iterator iterator_at(uint64_t pos)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"iterator_at(%lld)",pos);
          uint64_t px       = pos;
          uint64_t max_pos  = 0;
          item * p        = &head_;

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
              CSL_DEBUGF(L"Returning iterator to {%p mul:%d [%lld-%lld=%lld]}",p,p->mul_,pos,px,pos-px);
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
              CSL_DEBUGF(L"Returning iterator to {%p mul:%d [%lld-%lld=%lld]}",p,p->mul_,pos,px,pos-px);
              RETURN_FUNCTION( iterator(p,pos-px) );
            }
          }

          px  = (pos-max_pos);
          if( mul < px/width_ ) mul = px/width_;
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
              CSL_DEBUGF(L"Returning iterator to {%p mul:%d [%lld-%lld=%lld]}",p,p->mul_,pos,px,pos-px);
              ii.init( p,pos-px,pos );
              RETURN_FUNCTION( ii );
            }
          }

          px  = (pos-max_pos);
          if( mul < px/width_ ) mul = px/width_;
          allocate( mul );

          // recursive call : XXX may be dangerous
          RETURN_FUNCTION( force_iterator_at(pos,ii) );
        }

        bool free_at(uint64_t pos)
        {
          iterator it = iterator_at(pos);
          if( it != end() && it.is_empty() == false )
          {
            it.free();
            return true;
          }
          return false;
        }

        bool is_free_at(uint64_t pos)
        {
          ENTER_FUNCTION();
          iterator it = iterator_at(pos);
          if( it == end() )
          {
            CSL_DEBUGF(L"No item at invalid position: %lld",pos);
            RETURN_FUNCTION(true);
          }
          else if( it.is_empty() == true )
          {
            CSL_DEBUGF(L"Empty item at position: %lld",pos);
            RETURN_FUNCTION(true);
          }
          else
          {
            CSL_DEBUGF(L"Have item at position: %lld",pos);
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

          RETURN_FUNCTION( pos );
        }

        T * get_ptr(uint64_t at)
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
              return p->get_ptr(at-pos);
            }
          }

          /* handle invalid index position */
          return 0;
        }

        uint64_t iterator_pos(const iterator & it)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"Iterator gpos=%lld",it.get_pos());
          RETURN_FUNCTION(it.get_pos());
        }

        T * push_back(const T & t)
        {
          ENTER_FUNCTION();
          iterator ii;
          RETURN_FUNCTION(last_free(ii).set(t));
        }

        template <typename T1>
        T * push_back(const T1 & t1)
        {
          ENTER_FUNCTION();
          iterator ii;
          RETURN_FUNCTION(last_free(ii).set(t1));
        }

        template <typename T1,typename T2>
        T * push_back(const T1 & t1,const T2 & t2)
        {
          ENTER_FUNCTION();
          iterator ii;
          RETURN_FUNCTION(last_free(ii).set(t1,t2));
        }

        template <typename T1,typename T2,typename T3>
        T * push_back(const T1 & t1,const T2 & t2,const T3 & t3)
        {
          ENTER_FUNCTION();
          iterator ii;
          RETURN_FUNCTION(last_free(ii).set(t1,t2,t3));
        }

        template <typename T1,typename T2,typename T3,typename T4>
        T * push_back(const T1 & t1,const T2 & t2,const T3 & t3,const T4 & t4)
        {
          ENTER_FUNCTION();
          iterator ii;
          RETURN_FUNCTION(last_free(ii).set(t1,t2,t3,t4));
        }

        T * construct(uint64_t pos)
        {
          ENTER_FUNCTION();
          iterator ii;
          RETURN_FUNCTION( force_iterator_at(pos,ii).construct() );
        }

        T * set(uint64_t pos,const T & t)
        {
          ENTER_FUNCTION();
          iterator ii;
          RETURN_FUNCTION(force_iterator_at(pos,ii).set(t));
        }

        template <typename T1>
        T * set(uint64_t pos, const T1 & t1)
        {
          ENTER_FUNCTION();
          iterator ii;
          RETURN_FUNCTION(force_iterator_at(pos,ii).set(t1));
        }

        template <typename T1,typename T2>
        T * set(uint64_t pos, const T1 & t1,const T2 & t2)
        {
          ENTER_FUNCTION();
          iterator ii;
          RETURN_FUNCTION(force_iterator_at(pos,ii).set(t1,t2));
        }

        template <typename T1,typename T2,typename T3>
        T * set(uint64_t pos, const T1 & t1,const T2 & t2,const T3 & t3)
        {
          ENTER_FUNCTION();
          iterator ii;
          RETURN_FUNCTION(force_iterator_at(pos,ii).set(t1,t2,t3));
        }

        template <typename T1,typename T2,typename T3,typename T4>
        T * set(uint64_t pos, const T1 & t1,const T2 & t2,const T3 & t3,const T4 & t4)
        {
          ENTER_FUNCTION();
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
