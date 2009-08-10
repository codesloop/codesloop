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

#ifndef _csl_common_hash_helpers_hh_included_
#define _csl_common_hash_helpers_hh_included_

#include "inpvec.hh"
#include "obj.hh"
#include "logger.hh"
#include "common.h"

#ifdef __cplusplus

namespace csl
{
  namespace common
  {
    namespace hash_helpers
    {
      typedef uint64_t hash_key_t;

      template <typename K, typename V>
      class contained
      {
        public:
          typedef K    key_t;
          typedef V    value_t;

        private:
          key_t        key_;
          value_t      value_;
          hash_key_t   hash_key_;
          uint64_t     pos_;
          contained *  next_;

        public:
          contained( const contained & other ) :
            key_(other.key_), value_(other.value_), hash_key_(other.hash_key_), pos_(0), next_(0) {}

          contained( key_t k, value_t v, hash_key_t hk, uint64_t p  ) :
              key_(k), value_(v), hash_key_(hk), pos_(p), next_(0) {}

          contained() :
              hash_key_(0), pos_(0), next_(0) {}

          bool is_equal( const key_t k, const value_t v )
          {
            if( k == key_ && v == value_ ) { return true;  }
            else                           { return false; }
          }

          contained * next() const   { return next_;  }
          void next(contained * n)   { next_ = n;     }

          hash_key_t hash_key() const   { return hash_key_;  }
          void hash_key(hash_key_t hk)  { hash_key_ = hk;    }

          const key_t & key() const      { return key_; }
          const value_t & value() const  { return value_; }

          uint64_t pos() const { return pos_; }
      };

      typedef inpvec< uint64_t > pos_vec_t;

      template <typename K, typename V>
      class page
      {
        public:
          typedef K                                               key_t;
          typedef V                                               value_t;
          typedef contained<K,V>                                  contained_t;
          typedef inpvec<contained_t>                             contained_vec_t;
          typedef typename inpvec<contained_t>::iterator          contained_iter_t;
          typedef inpvec< page >                                  page_vec_t;

          static const uint64_t sz_       = 32ULL;
          static const uint64_t bits_     = 5ULL;
          static const hash_key_t mask_   = 0x1f;

          static const int  ok_            = 1;
          static const int  bad_pos_       = 2;
          static const int  has_already_   = 3;
          static const int  append_ok_     = 4;

          int add(uint64_t pos, const key_t & k, const value_t & v, hash_key_t hk)
          {
            ENTER_FUNCTION();
            CSL_DEBUGF(L"add(%lld,k,v,%lld)",pos,hk);
            int ret = ok_;

            if( pos > sz_ ) { THR(exc::rs_invalid_param,bad_pos_); }

            CSL_DEBUGF(L"Adding [%p:%p] to pos #%lld [hk:%lld]",&k,&v,pos,hk);

            contained_t * p = data_.get_ptr(pos);

            if( p == 0 )
            {
              /* the given position is empty */
              CSL_DEBUGF(L"Filling empty pos at #%lld [hk:%lld]",pos,hk);
              data_.set( pos,k,v,hk,pos );
              ret = ok_;
            }
            else
            {
              /* there is an item at the given position
              ** now check if this is a duplicate or not
              ** if duplicate then skip, otherwise append
              ** to the end of the list
              **/

              CSL_DEBUGF(L"Find suitable slot for: [hk:%lld]",hk);

              while( p )
              {
                CSL_DEBUGF(L"Checking #%lld [hk:%lld]",p->pos(),hk);

                if( p->is_equal( k,v ) )
                {
                  /* already have this item */
                  CSL_DEBUGF(L"[%p:%v] is already on the page [hk:%lld]",&k,&v,hk);
                  RETURN_FUNCTION(has_already_);
                }

                if( p->next() == 0 )
                {
                  uint64_t newpos = sz_;
                  contained_t * tp = data_.get_ptr(newpos);

                  if( tp != 0 ) { newpos = data_.last_free_pos(); }

                  CSL_DEBUGF(L"data_.set(%lld,k,v,%lld,%ld)",newpos,hk,newpos);
                  data_.set( newpos,k,v,hk,newpos );
                  tp = data_.get_ptr( newpos );
                  p->next(tp);
                  CSL_DEBUGF(L"Appended item. New pos is #%lld",newpos);
                  ret = append_ok_;
                  break;
                }
                p = p->next();
              }
            }

            RETURN_FUNCTION(ret);
          }

          void remove(uint64_t pos, const key_t & k)
          {
            /* TODO */
          }

          void split(uint64_t shift, page_vec_t & pagev, pos_vec_t & posv)
          {
            typedef typename page_vec_t::iterator it_t;

            contained_iter_t it = data_.begin();
            const contained_iter_t & en(data_.end());

            uint64_t pos = 0;

            for( ;(it!=en) && (pos<32);++it )
            {
              if( it.is_empty() == false && pos != 0 )
              {
                it_t pit = pagev.last_free();
                uint64_t pageid = pagev.iterator_pos(pit);

                /* create the new page */
                pit.construct();

                /* register the new page position */
                posv.set( pos, pageid );
                contained_t * p = (*it);
                uint64_t px = 0;

                while( p )
                {
                  hash_key_t hk = p->hash_key();
                  (*pit)->add( ((hk>>shift)&mask_),p->key(),p->value(),hk );
                  px = p->pos();
                  p = p->next();
                  /* remove the item from current container */
                  data_.iterator_at(px).free();
                }
              }
              ++pos;
            }
          }

          contained_t * get(uint64_t pos)
          {
            if( pos > sz_ ) return 0;
            else return *(data_.iterator_at(pos));
          }

          uint64_t n_items() { return data_.n_items(); }

          uint64_t n_free()
          {
            const contained_iter_t & it(data_.begin());
            return it.n_free();
          }

          bool has_item(uint64_t pos)
          {
            ENTER_FUNCTION();
            bool ret = false;
            if( pos > sz_ ) { THR(exc::rs_invalid_param,false); }
            else ret = (data_.is_free_at(pos) == false);
            CSL_DEBUGF(L"%shaving item at pos: %lld",(ret==true?"":"Not "),pos);
            RETURN_FUNCTION(ret);
          }

          void debug()
          {
#ifdef DEBUG
            ENTER_FUNCTION();
            data_.debug();
            LEAVE_FUNCTION();
#endif /*DEBUG*/
          }

        private:
          contained_vec_t  data_;

          CSL_OBJ(csl::common::hash_helpers,page);
          USE_EXC();
      };

      class index
      {
        public:
          typedef inpvec< uint64_t >    item_vec_t;
          typedef item_vec_t::iterator  item_iter_t;
          typedef pos_vec_t::iterator   pos_iter_t;

          bool internal_get( uint64_t at, uint64_t & pos, bool & is_page )
          {
            ENTER_FUNCTION();
            item_vec_t::iterator i = items_.iterator_at( at );

            if( i.is_empty() )
            {
              /* the given position at 'at' is empty */
              RETURN_FUNCTION( false );
            }
            /* extract 'page id' or 'index position' */
            uint64_t x = (*i)[0];
            pos = (x>>1ULL);

            /* decide wether it is a page id? */
            is_page = ((x&1ULL) == 1ULL);
            RETURN_FUNCTION( true );
          }

          void internal_set( uint64_t at, uint64_t pos, bool is_page )
          {
            ENTER_FUNCTION();
            uint64_t p = (pos<<1ULL) | (is_page&1ULL);
            CSL_DEBUGF(L"set #%lld to [%s:%lld] => %lld",at,(is_page==true?"page":"link"),pos,p);
            items_.set( at, p );
            LEAVE_FUNCTION();
          }

          static const uint64_t not_found_ = 0xffffffffffffffffULL;

          uint64_t lookup_pos( hash_key_t hk, uint64_t & shift )
          {
            ENTER_FUNCTION();
            uint64_t tpos,pg=0ULL,off=0ULL;
            bool is_page;

            /* iterator through the index records only checking the relevant ones
            ** of which the corresponding part of the hash key matches. the iterator i
            ** tells which part of the hash key to be checked
            */
            for( uint64_t i=0;i<64;i+=5 )
            {
              tpos = ((hk>>i)&0x1f);

              if( internal_get( off+tpos, pg, is_page ) == false )
              {
                CSL_DEBUGF(L"empty position at: %lld [%lld:%lld]",(off+tpos),off,tpos);
                RETURN_FUNCTION( not_found_ );
              }

              if( is_page == true )
              {
                CSL_DEBUGF(L"page #%lld found at: %lld",pg,off+tpos);
                shift = i;
                RETURN_FUNCTION( off+tpos );
              }
              else
              {
                CSL_DEBUGF(L"link to %lld/%lld found at: %lld",pg,pg*32,off+tpos);
              }
              off = pg*32;
            }
            RETURN_FUNCTION( not_found_ );
          }

          bool split( hash_key_t hk, uint64_t & shift, pos_vec_t & posv )
          {
            ENTER_FUNCTION();

            pos_iter_t   it      = posv.begin();
            pos_iter_t   en      = posv.end();
            uint64_t     pos     = lookup_pos( hk, shift );
            uint64_t     ii      = items_.last_free_pos();
            uint64_t     endp    = (ii+31)/32;

            if( pos == not_found_ )
            {
              CSL_DEBUGF(L"couldn't find index position for [%lld:0x%llx]",hk,hk);
              RETURN_FUNCTION( false );
            }

            /* set this to be a link */
            CSL_DEBUGF(L"Linking pos:%lld to [%lld / %lld]",pos,endp,endp*32);
            internal_set( pos, endp, false );

            for( uint64_t i=(endp*32);i<(endp+1)*32;++i )
            {
              if( it.is_empty() == false )
              {
                CSL_DEBUGF(L"Setting pos:%lld to [%lld]",i,((*it)[0]));
                //items_.set( i,(*it)[0] );
                internal_set( i,(*it)[0],true );
              }
              ++it;
            }

            RETURN_FUNCTION( false );
          }

          bool get( hash_key_t hk, uint64_t & pgpos, uint64_t & shift )
          {
            ENTER_FUNCTION();
            uint64_t pg,pos = lookup_pos( hk, shift );
            bool is_page;

            if( internal_get( pos, pg, is_page ) == false )
            {
              RETURN_FUNCTION( false );
            }

            if( is_page == true )
            {
              pgpos = pg;
              RETURN_FUNCTION( true );
            }

            RETURN_FUNCTION( false );
          }

          uint64_t n_items() { return items_.n_items(); }

          void debug()
          {
#ifdef DEBUG
            ENTER_FUNCTION();
            items_.debug();
            LEAVE_FUNCTION();
#endif /*DEBUG*/
          }

        private:
          item_vec_t items_;

          CSL_OBJ(csl::common::hash_helpers,index);
          USE_EXC();
      };
    }
  }
}

#endif /* __cplusplus */
#endif /* _csl_common_hash_helpers_hh_included_ */
