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
#include "hash_helpers.hh"
#include "hash.hh"
#include "tbuf.hh"
#include "pbuf.hh"

#include "test_timer.h"
#include "logger.hh"
#include "common.h"
#include <assert.h>
#include <vector>

using namespace csl::common;
using namespace csl::common::hash_helpers;

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

  template <typename K, typename V, typename F=default_hash_fun<K>, size_t SZ=32, size_t BITS=5 > class hash
  {
    CSL_OBJ(csl::common,hash);

    public:
      typedef K key_t;
      typedef V value_t;

      typedef hash_helpers::page<key_t,value_t,SZ,BITS>   page_t;
      typedef typename page_t::page_vec_t                 page_vec_t;

      bool has_key(const key_t & key) { return false; }
      bool get(const key_t & key, value_t & value) { return false; }
      bool set(const key_t & key, const value_t & value) { return false; }
      bool del(const key_t & key) { return false; }

    private:
      page_vec_t    pages_;

#if 0
    private:
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

  // -- hash --
  // hash_baseline               2717.156 ms, 67108862 calls,   0.000040 ms/call,   24698199.882524 calls/sec

  // -- tbuf --
  // tbuf_baseline               2086.350 ms, 134217726 calls,   0.000016 ms/call,   64331356.675534 calls/sec

  // -- pbuf --
  // pbuf_baseline               2491.725 ms, 16777214 calls,   0.000149 ms/call,   6733172.400646 calls/sec

  static inline const wchar_t * get_namespace()   { return L"test_hash"; }
  static inline const wchar_t * get_class_name()  { return L"test_hash::noclass"; }
  static inline const wchar_t * get_class_short() { return L"noclass"; }

  void hash_baseline() { hash<uint64_t,uint64_t> o; }
  void tbuf_baseline() { tbuf<1024> o; }
  void pbuf_baseline() { pbuf o; }

  

} // end of test_hash

using namespace test_hash;

int main()
{
  csl_common_print_results( "hash_baseline            ", csl_common_test_timer_v0(hash_baseline),"" );
  csl_common_print_results( "tbuf_baseline            ", csl_common_test_timer_v0(tbuf_baseline),"" );
  csl_common_print_results( "pbuf_baseline            ", csl_common_test_timer_v0(pbuf_baseline),"" );

  return 0;
}

/* EOF */
