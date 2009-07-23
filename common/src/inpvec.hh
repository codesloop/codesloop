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
        static const uint16_t  width_ = 32;
        typedef uint32_t       bitmap_t;

        struct item
        {
          uint32_t    free_:16;
          uint32_t    mul_:16;
          bitmap_t  * bmap_;
          T         * items_;
          item *      next_;

          ~item()
          {
            destroy();
            if( free_ ) { free(bmap_); free(items_); }
          }

          size_t size() { return mul_*width_; }

          size_t n_items()
          {
            static const unsigned char byte_bits_[] = {
              0, 1, 1, 2, 1, 2, 2, 3,    1, 2, 2, 3, 2, 3, 3, 4,    1, 2, 2, 3, 2, 3, 3, 4,    2, 3, 3, 4, 3, 4, 4, 5,
              1, 2, 2, 3, 2, 3, 3, 4,    2, 3, 3, 4, 3, 4, 4, 5,    2, 3, 3, 4, 3, 4, 4, 5,    3, 4, 4, 5, 4, 5, 5, 6,
              1, 2, 2, 3, 2, 3, 3, 4,    2, 3, 3, 4, 3, 4, 4, 5,    2, 3, 3, 4, 3, 4, 4, 5,    3, 4, 4, 5, 4, 5, 5, 6,
              2, 3, 3, 4, 3, 4, 4, 5,    3, 4, 4, 5, 4, 5, 5, 6,    3, 4, 4, 5, 4, 5, 5, 6,    4, 5, 5, 6, 5, 6, 6, 7,
              1, 2, 2, 3, 2, 3, 3, 4,    2, 3, 3, 4, 3, 4, 4, 5,    2, 3, 3, 4, 3, 4, 4, 5,    3, 4, 4, 5, 4, 5, 5, 6,
              2, 3, 3, 4, 3, 4, 4, 5,    3, 4, 4, 5, 4, 5, 5, 6,    3, 4, 4, 5, 4, 5, 5, 6,    4, 5, 5, 6, 5, 6, 6, 7,
              2, 3, 3, 4, 3, 4, 4, 5,    3, 4, 4, 5, 4, 5, 5, 6,    3, 4, 4, 5, 4, 5, 5, 6,    4, 5, 5, 6, 5, 6, 6, 7,
              3, 4, 4, 5, 4, 5, 5, 6,    4, 5, 5, 6, 5, 6, 6, 7,    4, 5, 5, 6, 5, 6, 6, 7,    5, 6, 6, 7, 6, 7, 7, 8
            };

            size_t ret = 0;
            for( uint32_t i=0;i<mul_; ++i )
            {
              ret += byte_bits_[((bmap_[i])&0xff)];
              ret += byte_bits_[((bmap_[i]>>8)&0xff)];
              ret += byte_bits_[((bmap_[i]>>16)&0xff)];
              ret += byte_bits_[((bmap_[i]>>24)&0xff)];
              /*
              ret += byte_bits_[((bmap_[i]>>32)&0xff)];
              ret += byte_bits_[((bmap_[i]>>40)&0xff)];
              ret += byte_bits_[((bmap_[i]>>48)&0xff)];
              ret += byte_bits_[((bmap_[i]>>56)&0xff)];
              */
              // TODO fix here
            }
            return ret;
          }

          size_t n_free() { return ((width_*mul_)-n_items()); }

          inline void destroy()
          {
            for( uint32_t i=0;i<mul_; ++i )
            {
              if( bmap_[i] != 0 )
              {
                for( unsigned char j=0;j<width_;++j )
                {
                  if( (bmap_[i]>>j)&static_cast<bitmap_t>(1) )
                  {
                    in_place_destruct( items_+((i*width_)+j) );
                  }
                }
              }
            }
          }

          void mul_alloc(uint32_t m)
          {
            mul_         = m;
            free_        = 1;
            items_       = reinterpret_cast<T *>(malloc(m*width_*sizeof(T)));
            bmap_        = reinterpret_cast<bitmap_t *>(malloc(m*sizeof(bitmap_t)));
            next_        = 0;
            memset( bmap_,0,(m*sizeof(bitmap_t)) );
          }

          inline T & get(size_t at) { return *(items_+at); }

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

            if( (bmap_[off]>>pos)&1 )
            { // already have an item
              items_[at] = t;
              return false;
            }
            else
            { // "allocate" = copy construct an item
              new (items_+at) T(t);
              bmap_[off] = (bmap_[off]) | (static_cast<bitmap_t>(1)<<pos);
              return true;
            }
          }
        };

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
          tail_->next_ = n;
          tail_ = n;
        }

      public:

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
              if( p->set(pos-px,t) ) n_items_++;
              return;
            }
          }

          px  = (pos-max_pos);
          if( mul < px/width_ ) mul = px/width_;
          allocate( mul );

          /* recursive call : XXX may be dangerous */
          set(pos,t);
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
    };
  } /* end of ns:common */
} /* end of ns:csl */

#endif /* __cplusplus */
#endif /* _csl_common_inpvec_hh_included_ */
