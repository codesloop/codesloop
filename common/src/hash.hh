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

#ifndef _csl_common_hash_hh_included_
#define _csl_common_hash_hh_included_

/**
   @file hash.hh
   @brief generic hash table class
 */

#include "obj.hh"
#include "hash_helpers.hh"
#include "exc.hh"
#include "logger.hh"
#include "common.h"
#ifdef __cplusplus

namespace csl
{
  namespace common
  {
    template <typename K> struct default_hash_fun
    {
      hash_helpers::hash_key_t operator()(const K & k) { return k.hash_key(); }
    };

    template <> struct default_hash_fun<hash_helpers::hash_key_t>
    {
      hash_helpers::hash_key_t operator()(hash_helpers::hash_key_t k) { return k; }
    };

    template <typename K, typename V, typename F=default_hash_fun<K> > class hash
    {
      public:
        typedef K                         key_t;
        typedef V                         value_t;
        typedef hash_helpers::hash_key_t  hash_key_t;

        typedef hash_helpers::page<key_t,value_t>   page_t;
        typedef typename page_t::page_vec_t         page_vec_t;
        typedef typename page_vec_t::iterator       page_iter_t;
        typedef hash_helpers::index                 index_t;
        typedef hash_helpers::pos_vec_t             pos_vec_t;

        bool has_key(const key_t & key) { return false; }
        bool get(const key_t & key, value_t & value) { return false; }
        bool del(const key_t & key) { return false; }

        bool set( const key_t & key, const value_t & value )
        {
          ENTER_FUNCTION();

          F          hash_fun;
          uint64_t   pgpos      = 0;
          uint64_t   pageid     = 0;
          hash_key_t hk         = hash_fun( key );
          uint64_t   shift      = 0;

          if( index_.get( hk, pageid, shift ) == false )
          {
            /* no page for hash key yet */
            CSL_DEBUGF( L"no page for hash_key: [hk:%lld]", hk );

            pgpos        = (hk&(page_t::mask_));
            page_t * pg  = create_page( pageid );

            CSL_DEBUG_ASSERT( pg != 0 );
            CSL_DEBUGF( L"page: %lld created for: [hk:%lld]", pageid, hk );

            index_.internal_set( pgpos, pageid, true );
#ifdef DEBUG
            int result =
#endif /* DEBUG */
            pg->add( (hk>>5)&(page_t::mask_), key, value, hk );

            /* this should return ok_ indicating that it wen smoothly */
            CSL_DEBUG_ASSERT( result == page_t::ok_ );
            RETURN_FUNCTION( true );
          }

          /* now we have a page id at 'pageid' so try to add the data to it */
          page_t * pg = pages_.get_ptr( pageid );
          CSL_DEBUG_ASSERT( pg != 0 );

          /* check if there is an item at the desired position */
          if( pg->has_item( (hk>>(5+shift))&(page_t::mask_) ) == false )
          {
            /* noone at the given position, the item should always be added, no split can happen */
            CSL_DEBUGF( L"Add the first element to: [hk:%lld:%lld] pg:%lld",hk,(hk>>(5+shift))&(page_t::mask_),pageid );

#ifdef DEBUG
            int result =
#endif /*DEBUG*/
            pg->add( (hk>>(5+shift))&(page_t::mask_), key, value, hk );

            /* this should return ok_ indicating that it wen smoothly */
            CSL_DEBUG_ASSERT( result == page_t::ok_ );
            RETURN_FUNCTION( true );
          }

          int result = pg->add( (hk>>(5+shift))&(page_t::mask_), key, value, hk );

          if( result == page_t::has_already_ )
          {
            /* this means that this key <=> value pair already there, so
            ** page didn't replace that */
            CSL_DEBUGF( L"not replacing existing value. [hk:%lld]",hk);
            RETURN_FUNCTION( false );
          }
          else if( result == page_t::append_ok_ )
          {
            /* k,v has been appended to the given position. this tells us
            ** to check for the need to split the page */
            CSL_DEBUGF( L"k,v appended at page:%lld pos:%lld ** [hk:%lld]",pageid,(hk>>(5+shift))&(page_t::mask_),hk );

            uint64_t nf = pg->n_free();

            if( (pg->n_items() > 1000 && nf < 31) || nf == 0 )
            {
              /* do split */
              pos_vec_t  iv;

              /* TODO : SPEEDUP HERE XXX */
              pg->split( shift+10, pages_, iv );

#ifdef DEBUG
              bool res =
#endif /*DEBUG*/
              index_.split( hk, shift, iv );

              CSL_DEBUG_ASSERT( res == true );
            }

            RETURN_FUNCTION( true );
          }
          else
          {
            /* the page returned an unexpected value, telling us
            ** something is wrong with its state */
            THR(exc::rs_invalid_state,false);
          }
          RETURN_FUNCTION( false );
        }

        void debug()
        {
#ifdef DEBUG
          ENTER_FUNCTION();
          CSL_DEBUGF(L"DEBUG: %s","INDEX");
          index_.debug();
          CSL_DEBUGF(L"DEBUG: %s","PAGES");
          pages_.debug();
          CSL_DEBUGF(L"DEBUG: %s","PAGEDATA");
          page_iter_t it = pages_.begin();
          page_iter_t en = pages_.end();
          for( ;it!=en;++it )
          {
            (*it)->debug();
          }
          LEAVE_FUNCTION();
#endif /*DEBUG*/
        }

      private:
        page_t * create_page( uint64_t & pgid )
        {
          page_iter_t it(pages_.last_free());
          it.construct();
          pgid = pages_.iterator_pos( it );
          return *it;
        }

        page_vec_t    pages_;
        index_t       index_;

        CSL_OBJ(csl::common,hash);
        USE_EXC();
    };

  } /* end of ns:common */
} /* end of ns:csl */

#endif /* __cplusplus */
#endif /* _csl_common_hash_hh_included_ */
