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

#include "codesloop/common/logger.hh"
#include "codesloop/common/hlprs.hh"
#include "codesloop/common/exc.hh"
#include "codesloop/common/common.h"
#ifdef __cplusplus

namespace csl
{
  namespace common
  {
    template <typename T> class inpvec
    {
      public:
        // note: if width is changed, it implies lots of changes in the code....
        typedef uint32_t  width_t;
        typedef uint32_t  mul_t;
        typedef uint8_t   free_t;

        static const uint16_t  width_         = 64;
        static const uint16_t  width_in_bits_ = 6;

        typedef uint64_t       bitmap_t;

        struct item
        {
          CSL_OBJ(csl::common,inpvec::item);

          free_t      free_;
          mul_t       mul_;
          uint8_t *   buffer_;
          bitmap_t  * bmap_;
          T         * items_;
          item *      next_;
          inpvec *    parent_;

          ~item()
          {
            destroy();
            if( free_ ) { free(buffer_); }
          }

          uint64_t size() const { return mul_*width_; }
          uint64_t n_items() const;
          uint64_t last_free() const;
          uint64_t first_free() const;
          uint64_t n_free() const { return ((width_*mul_)-n_items()); }
          void destroy();
          void destroy( uint64_t at );
          void mul_alloc( mul_t m );

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

          T * construct(uint64_t at);
          T * set(uint64_t at, const T & t);

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

          bool is_empty() const;
          bool is_empty(uint64_t at) const;
          void debug();
          bool is_last(uint64_t at) const;
          inline T & get(uint64_t at) const;
          T * get_ptr(uint64_t at) const;
          T * next_used(uint64_t & at);
        };

        friend struct item;

      private:
        bitmap_t         pre_bmap_;
        unsigned char *  pre_items_[width_*sizeof(T)];

        uint64_t  n_items_;
        item      head_;
        item *    tail_;

        void allocate();
        void allocate(mul_t mul);

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
              ENTER_FUNCTION_X();
              CSL_DEBUGF_X(L"iterator()");
              LEAVE_FUNCTION_X();
            }

          public:
            /// @brief initializer constructor
            inline iterator(item * i, uint64_t pos, uint64_t gp) : i_(i), pos_(pos), gpos_(gp)
            {
              ENTER_FUNCTION_X();
              CSL_DEBUGF_X(L"iterator(item:%p, pos:%lld, gpos:%lld)",i,pos,gp);
              LEAVE_FUNCTION_X();
            }

            void init(item * i, uint64_t pos, uint64_t gp);

            /// @brief copy constructor
            inline iterator(const iterator & other) : i_(other.i_), pos_(other.pos_), gpos_(other.gpos_)
            {
              ENTER_FUNCTION_X();
              CSL_DEBUGF_X(L"iterator(iterator ref& %p)",&other);
              LEAVE_FUNCTION_X();
            }

            /// @brief creates an iterator of ls
            /// @param ls is the pvlist to be iterated over
            inline iterator(inpvec & ipv) : i_(&(ipv.head_)), pos_(0), gpos_(0)
            {
              ENTER_FUNCTION_X();
              CSL_DEBUGF_X(L"iterator(inpvec ref& %p)",&ipv);
              if(!ipv.n_items_) i_ = 0;
              LEAVE_FUNCTION_X();
            }

            inline iterator & operator=(const iterator & other);
            bool operator==(const iterator & other) const;
            bool operator!=(const iterator & other) const;
            void operator++();
            T * next_used();
            void zero();
            bool at_end() const;
            T * operator*();
            bool is_empty() const;
            void free();
            uint64_t n_free() const;
            uint64_t get_pos() const;
            T * construct();
            T * set(const T & t);

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
        const iterator & end();
        const iterator & begin();

        ~inpvec()
        {
          ENTER_FUNCTION_X( );
          item * p = head_.next_;
          item * x = p;

          while( p )
          {
            x = p;
            p = p->next_;
            delete x;
          }
          LEAVE_FUNCTION_X( );
        }

        inpvec() : pre_bmap_(0), begin_(&head_,0,0), end_(0,0,0)
        {
          ENTER_FUNCTION_X( );
          CSL_DEBUGF_X(L"inpvec()");
          head_.buffer_      = 0;
          head_.free_        = 0;
          head_.mul_         = 1;
          head_.bmap_        = &pre_bmap_;
          head_.items_       = reinterpret_cast<T *>(pre_items_);
          head_.next_        = 0;
          head_.parent_      = this;
          tail_              = &head_;
          n_items_           = 0;
          LEAVE_FUNCTION_X( );
        }

        uint64_t n_items();
        uint64_t size();
        void debug();
        iterator last_free();
        iterator & last_free(iterator & ii);
        iterator iterator_at(uint64_t pos);
        iterator force_iterator_at(uint64_t pos);
        iterator & force_iterator_at(uint64_t pos,iterator & ii);
        bool free_at(uint64_t pos);
        bool is_free_at(uint64_t pos);
        T & get(uint64_t at);
        uint64_t last_free_pos() const;
        uint64_t first_free_pos() const;
        T * get_ptr(uint64_t at);
        uint64_t iterator_pos(const iterator & it);
        T * construct(uint64_t pos);

        //////////////////////////////////////////
        // push_back
        /////////////////////////////////////////
        T * push_back(const T & t);

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

        //////////////////////////////////////////
        // set
        /////////////////////////////////////////

        T * set(uint64_t pos,const T & t);

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

#include "inpvec_impl.hh"

#endif /* _csl_common_inpvec_hh_included_ */
