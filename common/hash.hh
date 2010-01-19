/*
Copyright (c) 2008,2009,2010, CodeSLoop Team

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

#include "codesloop/common/obj.hh"
#include "codesloop/common/hash_helpers.hh"
#include "codesloop/common/exc.hh"
#include "codesloop/common/logger.hh"
#include "codesloop/common/common.h"
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

      private:
        page_t * create_page( uint64_t & pgid )
        {
          ENTER_FUNCTION();
          page_iter_t it(pages_.last_free());
          page_t * ret = it.construct();
          pgid = pages_.iterator_pos( it );
          CSL_DEBUGF( L"create_page(%lld) => %p",pgid,ret);
          RETURN_FUNCTION(ret);
        }

        static const uint64_t no_hint_ = 0xFFFFFFFFFFFFFFFFULL;

        uint64_t page_reuse_hint( uint64_t pos )
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"page_reuse_hint(%lld)",pos);

          page_iter_t it(pages_.begin());
          const page_iter_t & en(pages_.end());

          for( ;it!=en;++it )
          {
            if( (*it)->get( pos ) == 0 )
            {
              CSL_DEBUGF(L"page_reuse_hint(%lld) => gpos:%lld at pos:%lld",it.get_pos(),pos);
              RETURN_FUNCTION( it.get_pos() );
            }
          }

          CSL_DEBUGF(L"page_reuse_hint(%lld) => no_hint_");
          RETURN_FUNCTION( no_hint_ );
        }

      public:
        bool set( const key_t & key, const value_t & value )
        {
          ENTER_FUNCTION();

          uint64_t   pageid     = 0;
          hash_key_t hk         = hash_fun_( key );
          uint64_t   shift      = 0;
          uint64_t   pgpos      = (hk&(page_t::mask_));
          page_t *   pg         = 0;

          /* check if there is a page allocated for this key */
          if( index_.get( hk, pageid, shift ) == false )
          {
            CSL_DEBUGF( L"no page for hash_key: [hk:%lld]", hk );

            uint64_t hn = page_reuse_hint( pgpos );

            if( hn == no_hint_ ) { pg = create_page( pageid ); }
            else
            {
              pg = pages_.get_ptr( hn );
              pageid = hn;
            }

            CSL_DEBUG_ASSERT( pg != 0 );

            index_.internal_set( pgpos, pageid, true );
#ifdef DEBUG
            int result =
#endif /*DEBUG*/
            pg->add( pgpos, key, value, hk );

            CSL_DEBUG_ASSERT( result == page_t::ok_ );
            RETURN_FUNCTION( true );
          }
          else
          {
            CSL_DEBUGF(L"page:%lld found for (k,v) [hk:%lld shift:%lld]",pageid,hk,shift);

            pg = pages_.get_ptr( pageid );
            CSL_DEBUG_ASSERT( pg != 0 );

            int result = pg->add( pgpos, key, value, hk );

            if( result == page_t::ok_ )
            {
              CSL_DEBUGF( L"Added the first element [hk:%lld] to [pg:%lld pos:%lld]",hk,pageid,pgpos);
              RETURN_FUNCTION( true );
            }
            else if( result == page_t::has_already_ )
            {
              CSL_DEBUGF( L"Not replacing existing value. [hk:%lld] on [pg:%lld pos:%lld]",hk,pageid,pgpos);
              RETURN_FUNCTION( false );
            }
            else if( result == page_t::append_ok_ )
            {
              CSL_DEBUGF( L"(k,v) appended at [pg:%lld pos:%lld] ** [hk:%lld]",pageid,pgpos,hk );

              uint64_t nf = pg->n_free();
              if( nf == 0 || (pg->n_items() > page_t::sz_ && nf < (page_t::sz_/4*3) ) )
              {
                /* create a new page to share the items */
                uint64_t newpgid = 0;
                page_t * newpage = create_page( newpgid );

                CSL_DEBUG_ASSERT( newpage != 0 );

                pos_vec_t iv;
                pg->split( pgpos,shift,*newpage,newpgid,iv );

                /* TODO : SPEEDUP HERE XXX */
                //pg->split( shift+(2*index_t::bits_), pages_, iv );
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
          }

          RETURN_FUNCTION( false );
        }

        void debug()
        {
#ifdef DEBUG
          ENTER_FUNCTION();
          CSL_DEBUGF(L"DEBUG: INDEX");
          index_.debug();
          CSL_DEBUGF(L"DEBUG: PAGES");
          pages_.debug();
          CSL_DEBUGF(L"DEBUG: PAGEDATA");
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
        page_vec_t    pages_;
        index_t       index_;
        F             hash_fun_;

        CSL_OBJ(csl::common,hash);
        USE_EXC();
    };

  } /* end of ns:common */
} /* end of ns:csl */

#endif /* __cplusplus */
#endif /* _csl_common_hash_hh_included_ */
