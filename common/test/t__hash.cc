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

/**
   @file t__hash.cc
   @brief Tests to verify hash table
 */

#ifndef DEBUG
#define DEBUG
#endif /* DEBUG */

#include "inpvec.hh"
#include "pvlist.hh"
#include "hash.hh"
#include "test_timer.h"
#include "logger.hh"
#include "common.h"
#include <assert.h>
#include <vector>

using namespace csl::common;

/** @brief contains tests related to hash */
namespace test_hash {

  template <typename K>
  struct default_hash_fun
  {
    inline uint64_t operator()(const K & k)
    {
      return k.hash_key();
    }
  };

  template <typename K, typename V, typename F=default_hash_fun<K> > class hash
  {
    CSL_OBJ(csl::common,hash);

    public:
      typedef K key_t;
      typedef V value_t;
      typedef uint64_t hashed_t;

      struct page;
      typedef inpvec<page>                       page_vec_t;
      typedef typename inpvec<page>::iterator    page_iterator_t;

      struct index;
      typedef inpvec<index>                      index_vec_t;
      typedef typename inpvec<index>::iterator   index_iterator_t;

      static const uint16_t index_bits_ = page_vec_t::width_in_bits_;
      static const uint16_t index_size_ = page_vec_t::width_;

      struct contained
      {
        hashed_t  hash_key_;
        key_t     key_;
        value_t   value_;

        contained( hashed_t h, const key_t & k, const value_t & v ) : hash_key_(h), key_(k), value_(v) {}

        CSL_OBJ(csl::common,hash::contained);
      };

      typedef inpvec<contained>                     contained_vec_t;
      typedef typename contained_vec_t::iterator    contained_iterator_t;
      typedef inpvec<uint32_t>                      pageid_vec_t;
      typedef typename inpvec<uint32_t>::iterator   pageid_iterator_t;

      struct page
      {
        static const uint16_t max_size_hint_ = page_vec_t::width_;

        contained_vec_t data_;

        void split(uint32_t shift, page_vec_t & pages, pageid_vec_t & idxs)
        {
          contained_iterator_t it  = data_.begin();
          contained_iterator_t en  = data_.end();

          for( ;it!=en;++it )
          {
            if( it.is_empty() == false )
            {
              /* calculate which page will have the given item */
              uint8_t pos = (((*it)->hash_key_ >> shift) & 0x1ff);

              if( pos == 0 )
              {
                /* if the page id is 0 then it will stay where it is */
              }
              else
              {
                /* check if a page has already been allocated for the given position */
                pageid_iterator_t pit = idxs.iterator_at( pos );
                page * p = 0;

                if( pit.is_empty() )
                {
                  /* default construct a page */
                  page_iterator_t pgit = pages.last_free();
                  pgit.construct();

                  /* set the index of the new page */
                  pit.set( pages.iterator_pos( pgit ) );

                  /* save a pointer to the given page */
                  p = *pgit;
                }
                else
                {
                  page_iterator_t pgit = pages.iterator_at( (*pit)[0] );
                  p = *pgit;
                }

                contained * c = *it;
                p->add( c->hash_key_,c->key_,c->value_ );
                it.free();
              }
            }
          }
        }

        static const uint8_t over_limit           = 1;
        static const uint8_t overwrite_duplicate  = 2;
        static const uint8_t samekey_append       = 3;
        static const uint8_t samehash_append      = 4;
        static const uint8_t succeed              = 5;
        static const uint8_t failed               = 6;

        uint8_t add(hashed_t hash_key, const key_t & key, const value_t & value)
        {
          contained_iterator_t it  = data_.begin();
          contained_iterator_t en  = data_.end();
          contained_iterator_t fr(en);

          bool hash_key_exists     = false;
          bool object_key_exists   = false;
          size_t used_items        = 0;
          size_t n_items           = data_.n_items();
          uint8_t retval           = succeed;

          // TODO: speedup this:
          // goes through each element

          for( ;it!=en;++it )
          {
            if( it.is_empty() && fr == en )
            {
              // memorize the first free element found
              fr = it;
            }
            else
            {
              ++used_items;
              if( (*it)->hash_key_ == hash_key )
              {
              // have that hash key already, this page cannot refuse
              // the item even if it has lots of items
                hash_key_exists = true;

                retval = samehash_append;

                if( (*it)->key_ == key )
                {
                // not just the hash key, but the object key field is
                // equal as well
                  object_key_exists = true;

                  if( (*it)->value_ == value )
                  {
                  // if value is equal then we overwrite and return
                    (*it)->value_ = value;
                    return ( overwrite_duplicate );
                  }
                  else
                  {
                  // if value is not equal then flag that
                    object_key_exists = true;
                    retval = samekey_append;
                  }
                }
              }

              // break the long loop to speedup the search
              if( used_items >= n_items ) break;
            }
          }

          if( hash_key_exists == true || object_key_exists == true ||  used_items < max_size_hint_ )
          {
            if( fr != en )   fr.set( hash_key, key, value );
            else             data_.push_back( hash_key, key, value );

            return retval;
          }
          else
          {
            return over_limit;
          }
        }

        CSL_OBJ(csl::common,hash::page);
      };

      struct index
      {
        static const uint8_t is_page = 1;
        static const uint8_t is_next = 1;

        uint8_t   page_or_next_;
        uint32_t  ptr_;

        CSL_OBJ(csl::common,hash::index);
      };

      bool has_key(const key_t & key) { return false; }
      bool get(const key_t & key, value_t & value) { return false; }
      bool set(const key_t & key, const value_t & value) { return false; }
      bool del(const key_t & key) { return false; }

    private:
      page_vec_t    pages_;
      index_vec_t   indices_;

#if 0
      typedef unsigned long long key_t;

    private:
      struct hdata
      {
        key_t   key_;
        T       dta_;

        hdata() { }
        hdata( key_t key, const T & dta ) : key_( key ), dta_( dta ) { }

        void set( key_t key, const T & dta )
        {
          key_   = key;
          dta_   = dta;
        }
      };

      typedef inpvec<hdata> hdata_vec_t;

      struct page
      {
        hdata_vec_t datas_;

        typedef typename hdata_vec_t::iterator hdata_vec_iterator_t;

        bool add( key_t key, const T & dta )
        {
          if( datas_.size() >= 16 )
          {
            hdata_vec_iterator_t it(datas_.begin());
            hdata_vec_iterator_t en(datas_.end());

            // slow here ...
            for( ;it!=en;++it )
            {
              hdata * h = *it;
              if( h && h->key_ == key )
              {
                h->set( key, dta );
                return true;
              }
            }
            return false;
          }

          datas_.push_back( hdata( key, dta ) );
          return true;
        }

        page() {}

        page(key_t key, const T & dta)
        {
          datas_.push_back( new hdata( key, dta ) );
        }
      };

      struct idx;
      typedef inpvec<idx> idx_vec_t;

      struct idx
      {
        static const unsigned char empty   = 0;
        static const unsigned char is_page = 1;
        static const unsigned char is_idxv = 2;

        unsigned char type_;

        union {
          idx_vec_t  * next_;
          page       * page_;
        };

        ~idx()
        {
          switch( type_ )
          {
            case is_page:
              if( page_ ) delete page_;
              break;

            case is_idxv:
              if( next_ ) delete next_;
              break;

            case empty:
            default:
              break;
          }
        }

        idx(page * pg) { set_page(pg); }

        void set_page(page * p)
        {
          type_ = is_page;
          page_ = p;
        }

        typedef typename hdata_vec_t::iterator hdata_vec_iterator_t;

        idx_vec_t * split(unsigned int shift)
        {
          if( type_ == is_page && page_ != 0 )
          {
            page * p = page_;
            next_    = new idx_vec_t();

            for( unsigned char i=0;i<16;++i ) next_->push_back(0);

            hdata_vec_iterator_t it(p->datas_.begin());
            hdata_vec_iterator_t en(p->datas_.end());

            for( ;it!=en;++it )
            {
              hdata * h = *it;
              key_t   k = ((h->key_)>>shift) & 15ULL;
              idx *  ix = next_->get_at(k);
              if( ix == 0 )
              {
                ix       = new idx( new page() );
                next_->set_at( k,ix );
              }
              ix->page_->add( h->key_, h->dta_ );
            }

            delete p;
          }
          else
          {
            return next_;
          }
          return 0;
        }
      };

      idx_vec_t  indices_;

    public:
      hash() { }

      void set(key_t key, const T & dta)
      {
        key_t          k = 0;
        idx_vec_t * idxs = &indices_;

        for( unsigned char i=0;i<16;++i )
        {
          k          = (key>>(i*4)) & 15ULL;
          idx * ix   = idxs->get_at(k);

          if( ix == 0 )
          {
            page * pg = new page( key, dta );
            ix        = new idx(pg);
            if( idxs->set_at(k,ix) == false )
            {
              for( unsigned char j=0;j<16;++j ) idxs->push_back(0);
              idxs->set_at(k,ix);
              return;
            }
            return;
          }
          else if( ix->page_ == 0 )
          {
            ix->set_page(new page( key, dta ));
            return;
          }
          else if( ix->type_ == idx::is_idxv  )
          {
            idxs = ix->next_;
          }
          else if( ix->type_ == idx::is_page )
          {
            page * pg = ix->page_;
            if( pg->add( key,dta ) == false )
            {
              idxs = ix->split((i+1)*4);
            }
            else
            {
              return;
            }
          }
        }
      }

      bool get(key_t key, T & dta)
      {
        return false;
      }

      bool del(key_t key)
      {
        return false;
      }
#endif
  };

  // -- pvlist --
  // baseline               1909.710 ms, 33554430 calls,   0.000057 ms/call,   17570432.159857 calls/sec
  // -- inpvec --
  // baseline               2818.410 ms, 67108862 calls,   0.000042 ms/call,   23810894.085672 calls/sec

  void baseline()
  {
    hash<uint64_t,uint64_t> o;
  }

  void page0()
  {
    typedef hash<uint64_t,uint64_t> hash_t;
    hash_t::page p;
  }

  void page1()
  {
    typedef hash<uint64_t,uint64_t> hash_t;
    hash_t::page p;

    for( uint64_t i=0;i<hash_t::page::max_size_hint_;++i )
    {
      assert( p.add( i,i,i ) == hash_t::page::succeed );
    }

    // add an existing item
    assert( p.add( 0,0,0 ) == hash_t::page::overwrite_duplicate );

    // add an item with an existing key, but different value
    assert( p.add( 0,0,1 ) == hash_t::page::samekey_append );

    // add an item with an existing hash key, but different key and value
    assert( p.add( 0,1,1 ) == hash_t::page::samehash_append );

    // add an item that should cause overflow
    assert( p.add( hash_t::page::max_size_hint_+1,1,1 ) == hash_t::page::over_limit );
  }

  static inline const wchar_t * get_namespace()   { return L"test_hash"; }
  static inline const wchar_t * get_class_name()  { return L"test_hash::noclass"; }
  static inline const wchar_t * get_class_short() { return L"noclass"; }

  void page_split()
  {
    typedef hash<uint64_t,uint64_t> hash_t;
    hash_t::page p;

    for( uint64_t i=0;i<hash_t::page::max_size_hint_;++i )
    {
      int r = p.add( i%32,i,i );
      assert(  r == hash_t::page::succeed );
      // CSL_DEBUGF( L"add: %ld",i );
    }

    hash_t::page_vec_t    res;
    hash_t::pageid_vec_t  ids;
    uint32_t              shift=0;

    assert( p.data_.n_items() == hash_t::page::max_size_hint_ );

    p.split(shift,res,ids);

    // CSL_DEBUGF( L"Page size: %ld [res=%ld ids=%ld]", p.data_.n_items(),res.n_items(),ids.n_items() );
    // CSL_DEBUGF( L"hash_t::page::max_size_hint_ =  %d", hash_t::page::max_size_hint_ );

    // the split creates n-1 pages evenly spreading the keys into them
    assert( res.n_items() == (hash_t::page::max_size_hint_ - 1) );

    // each page is created in the res vector, and its page ids (indexes) are stored
    // into the ids vector
    assert( ids.n_items() == (hash_t::page::max_size_hint_ - 1) );

    hash_t::page_vec_t::iterator it = res.begin();
    hash_t::page_vec_t::iterator en = res.begin();

    for( ;it!=en;++it )
    {
      assert( (*it)->data_.n_items() == 1 );
    }
  }

  void page_add(int n)
  {
    typedef hash<int,int> hash_t;
    hash_t::page p;

    for( int i=0;i<n;++i )
    {
      assert( p.add(i,i,i) == hash_t::page::succeed );
    }
  }

} // end of test_hash

using namespace test_hash;

int main()
{
  csl_common_print_results( "page_split          ", csl_common_test_timer_v0(page_split),"" );

  csl_common_print_results( "page_add (1)        ", csl_common_test_timer_i1(page_add,1),"" );
  csl_common_print_results( "page_add (2)        ", csl_common_test_timer_i1(page_add,2),"" );
  csl_common_print_results( "page_add (3)        ", csl_common_test_timer_i1(page_add,3),"" );
  csl_common_print_results( "page_add (4)        ", csl_common_test_timer_i1(page_add,4),"" );
  csl_common_print_results( "page_add (5)        ", csl_common_test_timer_i1(page_add,5),"" );

  csl_common_print_results( "baseline            ", csl_common_test_timer_v0(baseline),"" );
  csl_common_print_results( "page0               ", csl_common_test_timer_v0(page0),"" );
  csl_common_print_results( "page1               ", csl_common_test_timer_v0(page1),"" );

  return 0;
}

/* EOF */
