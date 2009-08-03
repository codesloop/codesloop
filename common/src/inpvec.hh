/*
Copyright (c) 2008,2009, David Beck, Tamas Foldi

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
        static const unsigned char * byte_bits()
        {
          static const unsigned char byte_bits_[] = {
            0,1,1,2,1,2,2,3,    1,2,2,3,2,3,3,4,    1,2,2,3,2,3,3,4,    2,3,3,4,3,4,4,5,
            1,2,2,3,2,3,3,4,    2,3,3,4,3,4,4,5,    2,3,3,4,3,4,4,5,    3,4,4,5,4,5,5,6,
            1,2,2,3,2,3,3,4,    2,3,3,4,3,4,4,5,    2,3,3,4,3,4,4,5,    3,4,4,5,4,5,5,6,
            2,3,3,4,3,4,4,5,    3,4,4,5,4,5,5,6,    3,4,4,5,4,5,5,6,    4,5,5,6,5,6,6,7,
            1,2,2,3,2,3,3,4,    2,3,3,4,3,4,4,5,    2,3,3,4,3,4,4,5,    3,4,4,5,4,5,5,6,
            2,3,3,4,3,4,4,5,    3,4,4,5,4,5,5,6,    3,4,4,5,4,5,5,6,    4,5,5,6,5,6,6,7,
            2,3,3,4,3,4,4,5,    3,4,4,5,4,5,5,6,    3,4,4,5,4,5,5,6,    4,5,5,6,5,6,6,7,
            3,4,4,5,4,5,5,6,    4,5,5,6,5,6,6,7,    4,5,5,6,5,6,6,7,    5,6,6,7,6,7,7,8
          };
          return byte_bits_;
        }

        static const unsigned char * byte_last_free()
        {
          static const unsigned char byte_last_free_[] = {
            0,1,2,2,3,3,3,3,    4,4,4,4,4,4,4,4,    5,5,5,5,5,5,5,5,    5,5,5,5,5,5,5,5,
            6,6,6,6,6,6,6,6,    6,6,6,6,6,6,6,6,    6,6,6,6,6,6,6,6,    6,6,6,6,6,6,6,6,
            7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,
            7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,
            255,255,255,255,255,255,255,255,    255,255,255,255,255,255,255,255,
                                255,255,255,255,255,255,255,255,    255,255,255,255,255,255,255,255,
            255,255,255,255,255,255,255,255,    255,255,255,255,255,255,255,255,
                                255,255,255,255,255,255,255,255,    255,255,255,255,255,255,255,255,
            255,255,255,255,255,255,255,255,    255,255,255,255,255,255,255,255,
                                255,255,255,255,255,255,255,255,    255,255,255,255,255,255,255,255,
            255,255,255,255,255,255,255,255,    255,255,255,255,255,255,255,255,
                                255,255,255,255,255,255,255,255,    255,255,255,255,255,255,255,255
          };
          return byte_last_free_;
        }

        static const unsigned char * byte_last_used()
        {
          static const unsigned char byte_last_used_[] = {
            255,0,1,1,2,2,2,2,    3,3,3,3,3,3,3,3,    4,4,4,4,4,4,4,4,    4,4,4,4,4,4,4,4,
              5,5,5,5,5,5,5,5,    5,5,5,5,5,5,5,5,    5,5,5,5,5,5,5,5,    5,5,5,5,5,5,5,5,
              6,6,6,6,6,6,6,6,    6,6,6,6,6,6,6,6,    6,6,6,6,6,6,6,6,    6,6,6,6,6,6,6,6,
              6,6,6,6,6,6,6,6,    6,6,6,6,6,6,6,6,    6,6,6,6,6,6,6,6,    6,6,6,6,6,6,6,6,
              7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,
              7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,
              7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,
              7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7
          };
          return byte_last_used_;
        }

        // note: if width is changed, it implies lots of changes in the code....
        static const uint16_t  width_ = 32;
        static const uint16_t  width_in_bits_ = 5;
        typedef uint32_t       bitmap_t;

        struct item
        {
          uint32_t    free_:16;
          uint32_t    mul_:16;
          bitmap_t  * bmap_;
          T         * items_;
          item *      next_;
          inpvec *    parent_;

          ~item()
          {
            destroy();
            if( free_ ) { free(bmap_); free(items_); }
          }

          size_t size() { return mul_*width_; }

          size_t n_items()
          {
            const unsigned char * bb = byte_bits();

            size_t ret = 0;
            for( uint32_t i=0;i<mul_; ++i )
            {
              ret += bb[((bmap_[i])&0xff)];
              ret += bb[((bmap_[i]>>8)&0xff)];
              ret += bb[((bmap_[i]>>16)&0xff)];
              ret += bb[((bmap_[i]>>24)&0xff)];
            }
            return ret;
          }

          size_t last_free()
          {
            const unsigned char * bl = byte_last_free();
            size_t ret = size();

            for( int32_t i=(mul_-1);i>=0;--i )
            {
              if( bmap_[i] == 0x0UL ) ret = i*32;
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

          size_t last_used()
          {
            const unsigned char * bl = byte_last_used();
            size_t ret = size();

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

          size_t n_free() { return ((width_*mul_)-n_items()); }

          void destroy()
          {
            size_t n_i = 0;
            size_t * px = &n_i;
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

          void destroy( uint32_t at )
          {
            size_t off = at/width_;
            size_t pos = at%width_;

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

          bool set(size_t at, const T & t)
          {
            size_t off = at/width_;
            size_t pos = at%width_;

            if( (bmap_[off]>>pos)&static_cast<bitmap_t>(1UL) )
            { // already have an item
              items_[at] = t;
              return false;
            }
            else
            { // "allocate" = copy construct an item
              new (items_+at) T(t);
              bmap_[off] = (bmap_[off]) | (static_cast<bitmap_t>(1UL)<<pos);
              if( parent_ ) ++(parent_->n_items_);
              return true;
            }
          }

          template <typename T1>
          bool set(size_t at, const T1 & t1)
          {
            size_t off = at/width_;
            size_t pos = at%width_;

            if( (bmap_[off]>>pos)&static_cast<bitmap_t>(1UL) )
            { // already have an item
              items_[at] = t1;
              return false;
            }
            else
            { // "allocate" = copy construct an item
              new (items_+at) T(t1);
              bmap_[off] = (bmap_[off]) | (static_cast<bitmap_t>(1UL)<<pos);
              if( parent_ ) ++(parent_->n_items_);
              return true;
            }
          }

          template <typename T1, typename T2>
          bool set(size_t at, const T1 & t1, const T2 & t2)
          {
            size_t off = at/width_;
            size_t pos = at%width_;

            if( (bmap_[off]>>pos)&static_cast<bitmap_t>(1UL) )
            { // already have an item
              items_[at] = T(t1,t2);
              return false;
            }
            else
            { // "allocate" = copy construct an item
              new (items_+at) T(t1,t2);
              bmap_[off] = (bmap_[off]) | (static_cast<bitmap_t>(1UL)<<pos);
              if( parent_ ) ++(parent_->n_items_);
              return true;
            }
          }

          template <typename T1, typename T2, typename T3>
          bool set(size_t at, const T1 & t1, const T2 & t2, const T3 & t3)
          {
            size_t off = at/width_;
            size_t pos = at%width_;

            if( (bmap_[off]>>pos)&static_cast<bitmap_t>(1UL) )
            { // already have an item
              items_[at] = T(t1,t2,t3);
              return false;
            }
            else
            { // "allocate" = copy construct an item
              new (items_+at) T(t1,t2,t3);
              bmap_[off] = (bmap_[off]) | (static_cast<bitmap_t>(1UL)<<pos);
              if( parent_ ) ++(parent_->n_items_);
              return true;
            }
          }

          template <typename T1, typename T2, typename T3, typename T4>
          bool set(size_t at, const T1 & t1, const T2 & t2, const T3 & t3, const T4 & t4)
          {
            size_t off = at/width_;
            size_t pos = at%width_;

            if( (bmap_[off]>>pos)&static_cast<bitmap_t>(1UL) )
            { // already have an item
              items_[at] = T(t1,t2,t3,t4);
              return false;
            }
            else
            { // "allocate" = copy construct an item
              new (items_+at) T(t1,t2,t3,t4);
              bmap_[off] = (bmap_[off]) | (static_cast<bitmap_t>(1UL)<<pos);
              if( parent_ ) ++(parent_->n_items_);
              return true;
            }
          }

          bool is_empty()
          {
            for( unsigned char j=0;j<mul_;++j )
            {
              if( bmap_[j] != 0xFFFFFFFFUL ) return false;
            }
            return true;
          }

          bool is_empty(size_t at)
          {
            size_t off = at/width_;
            size_t pos = at%width_;

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
            printf("==============ITEM==============\n");
            for( size_t i=0;i<mul_;++i )
            {
              for( unsigned char j=0;j<width_;++j )
              {
                if( (bmap_[i]>>j)&static_cast<bitmap_t>(1) ) printf("X");
                else                                         printf(".");
              }
              printf("\n");
            }
            printf("mul:%d  width:%d  free:%d\n",mul_,width_,free_);
          }

          bool is_last(size_t at)
          {
            size_t off = at/width_;
            size_t pos = at%width_;

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

          inline T & get(size_t at) { return *(items_+at); }

          inline T * get_ptr(size_t at)
          {
            size_t off = at/width_;
            size_t pos = at%width_;

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

        size_t  n_items_;
        item    head_;
        item *  tail_;

        void allocate() { allocate( (tail_->mul_+2) ); }

        void allocate(size_t mul)
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
          private:
            item * i_;
            size_t pos_;

            iterator() {}

          public:
            ~iterator() {}

            /// @brief initializer constructor
            ///
            iterator(item * i, size_t pos) : i_(i), pos_(pos) {}

            /// @brief copy constructor
            ///
            iterator(const iterator & other) : i_(other.i_), pos_(other.pos_) {}

            /// @brief copy operator
            ///
            iterator & operator=(const iterator & other)
            {
              i_   = other.i_;
              pos_ = other.pos_;
              return *this;
            }

            /// @brief creates an iterator of ls
            /// @param ls is the pvlist to be iterated over
            ///
            iterator(inpvec & ipv) : i_(&(ipv.head_)), pos_(0)
            {
              if(!ipv.n_items_) i_ = 0;
            }

            /// @brief checks equality
            ///
            bool operator==(const iterator & other)
            {
              return ((i_ == other.i_ && pos_ == other.pos_) ? true : false );
            }

            /// @brief checks if not equal
            ///
            bool operator!=(const iterator & other)
            {
              return (!(operator==(other)));
            }

            /// @brief step forward
            ///
            void operator++()
            {
              if( i_ == 0 )
              {
                i_   = 0;
                pos_ = 0;
              }
              else if( i_->is_last(pos_) == true )
              {
                i_   = i_->next_;
                pos_ = 0;
              }
              else
              {
                ++pos_;
              }
            }

            /// @brief sets the iterator to end
            ///
            void zero() { i_ = 0; pos_ = 0; }

            /// @brief returns the pointed item
            ///
            T * operator*()
            {
              if( i_ ) { return i_->get_ptr(pos_); }
              else     { return 0;                 }
            }

            /// @brief checks if the item is empty
            ///
            bool is_empty()
            {
              if( i_ ) { return i_->is_empty(pos_); }
              else     { return true; }
            }

            /// @brief free item at the given iterator position
            ///
            void free()
            {
              if( i_ ) { i_->destroy( pos_ ); }
            }

            /// @brief sets the item at the iterator position
            ///
            bool set(const T & t)
            {
              if( !i_ ) return false;
              return i_->set( pos_,t );
            }

            /// @brief sets the item at the iterator position
            ///
            template <typename T1>
            bool set(const T1 & t1)
            {
              if( !i_ ) return false;
              return i_->set( pos_,t1 );
            }

            /// @brief sets the item at the iterator position
            ///
            template <typename T1,typename T2>
            bool set(const T1 & t1,const T2 & t2)
            {
              if( !i_ ) return false;
              return i_->set( pos_,t1,t2 );
            }

            /// @brief sets the item at the iterator position
            ///
            template <typename T1,typename T2,typename T3>
            bool set(const T1 & t1,const T2 & t2,const T3 & t3)
            {
              if( !i_ ) return false;
              return i_->set( pos_,t1,t2,t3 );
            }

            /// @brief sets the item at the iterator position
            ///
            template <typename T1,typename T2,typename T3,typename T4>
            bool set(const T1 & t1,const T2 & t2,const T3 & t3,const T4 & t4)
            {
              if( !i_ ) return false;
              return i_->set( pos_,t1,t2,t3,t4 );
            }
        };

        /** @brief returns iterator pointed at the beginning of the container */
        iterator begin()
        {
          iterator ret(*this);
          return ret;
        }

        /** @brief returns iterator represents the end of this container */
        iterator end()
        {
          iterator ret(*this);
          ret.zero();
          return ret;
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

        inpvec() : pre_bmap_(0)
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

        size_t n_items() { return n_items_; }

        size_t size()
        {
          size_t ret = 0;
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
          printf("################################\nDebug INPVEC: n:%d\n",n_items_);
          item * p = &head_;
          while( p )
          {
            p->debug();
            p = p->next_;
          }
        }

        iterator last_free()
        {
          size_t mul = (tail_->mul_+2);
          size_t pos = tail_->last_free();

          if( pos == tail_->size() )
          {
            allocate(mul);
            return iterator(tail_,tail_->last_free());
          }
          else
          {
            return iterator(tail_,pos);
          }
        }

        void set(size_t pos,const T & t)
        {
          size_t px       = pos;
          size_t mul      = (tail_->mul_+2);
          size_t max_pos  = 0;
          item * p        = &head_;

          while( p )
          {
            max_pos += (p->size());
            if( pos >= max_pos )
            {
              p = p->next_;
            }
            else
            {
              px = (max_pos-p->size());
              p->set(pos-px,t);
              return;
            }
          }

          px  = (pos-max_pos);
          if( mul < px/width_ ) mul = px/width_;
          allocate( mul );

          // recursive call : XXX may be dangerous
          set(pos,t);
        }

        iterator iterator_at(size_t pos)
        {
          size_t px       = pos;
          size_t max_pos  = 0;
          item * p        = &head_;

          while( p )
          {
            max_pos += (p->size());
            if( pos >= max_pos )
            {
              p = p->next_;
            }
            else
            {
              px = (max_pos-p->size());
              return iterator(p,pos-px);
            }
          }
          return iterator(0,0);
        }

        T & get(size_t at)
        {
          item * p = &head_;
          size_t max_pos = 0;

          while( p != 0 )
          {
            max_pos += (p->size());
            if( at >= max_pos )
            {
              p = p->next_;
            }
            else
            {
              size_t pos = (max_pos-p->size());
              return p->get(at-pos);
            }
          }

          /* TODO XXX : handle invalid index position */
          throw csl::common::exc(L"inpvec");
          return tail_->get(0);
        }

        T & n_get(size_t at)
        {
          item * p = &head_;
          size_t max_pos = 0;

          while( p != 0 )
          {
            max_pos += (p->n_items());
            if( at >= max_pos )
            {
              p = p->next_;
            }
            else
            {
              size_t pos = (max_pos-n_items());
              return p->get(at-pos);
            }
          }

          /* TODO XXX : handle invalid index position */
          throw csl::common::exc(L"inpvec");
          return tail_->get(0);
        }

        void push_back(const T & t)
        {
          last_free().set(t);
        }

        template <typename T1>
        void push_back(const T1 & t1)
        {
          last_free().set(t1);
        }

        template <typename T1,typename T2>
        void push_back(const T1 & t1,const T2 & t2)
        {
          last_free().set(t1,t2);
        }

        template <typename T1,typename T2,typename T3>
        void push_back(const T1 & t1,const T2 & t2,const T3 & t3)
        {
          last_free().set(t1,t2,t3);
        }

        template <typename T1,typename T2,typename T3,typename T4>
        void push_back(const T1 & t1,const T2 & t2,const T3 & t3,const T4 & t4)
        {
          last_free().set(t1,t2,t3,t4);
        }
    };
  } /* end of ns:common */
} /* end of ns:csl */

#endif /* __cplusplus */
#endif /* _csl_common_inpvec_hh_included_ */
