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
   @file t__hash_exp.cc
   @brief Tests to verify hash table
 */

#if 0
#ifndef DEBUG
#define DEBUG
#define DEBUG_ENABLE_INDENT
//#define DEBUG_VERBOSE
#endif /* DEBUG */
#endif

// #include "hash.hh"
#include "inpvec.hh"
#include "ustr.hh"
#include "test_timer.h"
#include "logger.hh"
#include "common.h"
#include <assert.h>
#include <vector>

// TODO : problems:
// -- too many pages created because of too many splits
// -- copying is too expensive

using namespace csl::common;

namespace csl_test
{
  namespace common
  {
    namespace hash_helpers
    {
      typedef uint64_t hash_key_t;

      template <typename K, typename V> struct item;
      template <typename K, typename V> struct page;

      template <typename K, typename V> struct item
      {
        typedef K                     key_t;
        typedef V                     value_t;
        typedef page<key_t,value_t>   page_t;

        key_t        key_;
        value_t      value_;
        hash_key_t   hk_;
        uint64_t     pos_;
        page_t *     children_;

        item(key_t k, value_t v, hash_key_t hk,uint64_t pos)
        : key_(k), value_(v), hk_(hk), pos_(pos), /*next_(0), prev_(0)*/ children_(0) { }

        item() : hk_(0), pos_(0), /*next_(0), prev_(0),*/ children_(0) { }

        item(uint64_t pos, page_t * children) : hk_(0), pos_(pos), children_(children) { }

        CSL_OBJ(csl::common::hash_helpers,item);
        // USE_EXC();
      };

      template <typename K, typename V> struct page
      {
        typedef K                key_t;
        typedef V                value_t;
        typedef item<K,V>        item_t;
        typedef inpvec<item_t>   item_vec_t;
        typedef inpvec<page>     page_vec_t;

        /* return codes */
        enum {
          ok_,
          duplicate_error_,
        };

        int set(const key_t & k, const value_t & v, hash_key_t hk, page_vec_t & pv);
        void debug(ustr & s);

        item_vec_t              items_;
        uint64_t                shift_;
        static const uint64_t   mask_ = 63ULL;
        static const uint64_t   bits_ = 6ULL;

        page() : shift_(0) {}

        CSL_OBJ(csl::common::hash_helpers,page);
        USE_EXC();
      };
    };

    template <typename K> struct default_hash_fun
    {
      inline hash_helpers::hash_key_t operator()(const K & k) const { return k.hash_key(); }
    };

    template <> struct default_hash_fun<hash_helpers::hash_key_t>
    {
      inline hash_helpers::hash_key_t operator()(hash_helpers::hash_key_t k) const { return k; }
    };

    template <typename K, typename V, typename F=default_hash_fun<K> > class hash
    {
      public:
        typedef K                         key_t;
        typedef V                         value_t;
        typedef hash_helpers::item<K,V>   item_t;
        typedef hash_helpers::page<K,V>   page_t;
        typedef inpvec<page_t>            page_vec_t;

        bool set(const key_t & k, const value_t & v);
        void debug();

      private:
        page_t       head_;
        page_vec_t   pool_;
        F            hash_fun_;

        CSL_OBJ(csl::common,hash);
        USE_EXC();
    };
  };
};

// implementation
namespace csl_test
{
  namespace common
  {
    namespace hash_helpers
    {
      template <typename K,typename V> int page<K,V>::set(const key_t & k, const value_t & v, hash_key_t hk, page_vec_t & pv)
      {
        ENTER_FUNCTION();
        CSL_DEBUGF(L"set(k,v,hk:%lld,pv)",hk);
        int ret = ok_;
        uint64_t pos = (hk>>shift_)&mask_;
        CSL_DEBUGF(L"pos=%lld=(%lld>>%lld)",pos,hk,shift_,mask_);

        typename item_vec_t::iterator it = items_.iterator_at(pos);

        if( it.is_empty() )
        {
          /* the given position is empty */
          CSL_DEBUGF(L"filling empty pos:%lld [hk:%lld]",pos,hk);
          items_.set( pos,k,v,hk,pos );
          ret = ok_;
        }
        else
        {
          item_t * p = *it;

          /* same hash key, if the key is the same, then refuse to add, otherwise split the page */
          if( p->hk_ == hk )
          {
            if( p->key_ == k )
            {
              /* refuse the item to be stored as we already have this key */
              CSL_DEBUGF(L"not adding duplicate item with key:%p and hashkey:%lld ",&k,hk );
              ret = duplicate_error_;
            }
            else
            {
              // TODO : hash key collision XXX !
              ret = duplicate_error_;
            }
          }
          else
          {
            /* split page : a recursive call */

            // TODO : here we can save a few copies, later ... XXX
            CSL_DEBUGF(L"performance warning, here a few copies could be saved");

            typename page_vec_t::iterator it2(pv.last_free());
            page * pg = it2.construct();
            pg->shift_ = shift_+bits_;

            /* add old value. note that the old object and the new will coexist for while */
            pg->set( p->key_,p->value_,p->hk_,pv );

            /* add new value */
            ret = pg->set( k,v,hk,pv );

            /* free old value */
            it.free();

            /* construct a link at the place of the old value */
            it.set( pos,pg );
          }
        }
        CSL_DEBUGF(L"set(k,v,pv) => %d",ret);
        RETURN_FUNCTION(ret);
      }

      template <typename K,typename V> void page<K,V>::debug(ustr & s)
      {
#ifdef DEBUG
        ENTER_FUNCTION();
        typedef typename item_vec_t::iterator it_t;

        CSL_DEBUGF(L"n_items:%lld shift:%lld",items_.n_items(),shift_);
        it_t it = items_.begin();

        char xx[65]; xx[64] = 0; memset( xx,'.',sizeof(xx) );

        if( it.is_empty() )          xx[0] = '.';
        else if( (*it)->children_ )  xx[0] = 'O';
        else                         xx[0] = 'X';

        /* check others */
        item_t * x = 0;
        while( (x=it.next_used()) != 0 )
        {
          if( x->children_ ) xx[it.get_pos()] = 'O';
          else               xx[it.get_pos()] = 'X';
        }

        s += xx;
        s += "|\n";

#ifdef DEBUG_VERBOSE
        items_.debug();
#endif /*DEBUG_VERBOSE*/

        it = items_.begin();
        const it_t & en(items_.end());

        for( ;it!=en;++it )
        {
          if( it.is_empty() == false && (*it)->children_ != 0 )
          {
            (*it)->children_->debug(s);
          }
        }

        LEAVE_FUNCTION();
#endif /*DEBUG*/
      }
    };

    template <typename K, typename V, typename F> bool hash<K,V,F>::set(const key_t & k, const value_t & v)
    {
      ENTER_FUNCTION();
      CSL_DEBUGF(L"set(k,v)");
      hash_helpers::hash_key_t hk = hash_fun_(k);
      bool ret = head_.set( k,v,hk,pool_ );
      CSL_DEBUGF(L"set(k,v) => %s",(ret==true?"true":"false"));
      RETURN_FUNCTION(ret);
    }

    template <typename K, typename V, typename F> void hash<K,V,F>::debug()
    {
#ifdef DEBUG
      ENTER_FUNCTION();
      ustr s; s +="\n";
      head_.debug(s);
      CSL_DEBUGF(L"%s",s.c_str());
      LEAVE_FUNCTION();
#endif /*DEBUG*/
    }
  };
};

using namespace csl_test::common;
using namespace csl_test::common::hash_helpers;

/** @brief contains tests related to hash */
namespace test_hash {

  static inline const wchar_t * get_namespace()   { return L"test_hash"; }
  static inline const wchar_t * get_class_name()  { return L"test_hash::noclass"; }
  static inline const wchar_t * get_class_short() { return L"noclass"; }

  void t_item() { item<uint64_t,uint64_t> i; }
  void t_page() { page<uint64_t,uint64_t> p; }
  void t_hash() { hash<uint64_t,uint64_t> h; }

  void hash_set()
  {
    hash<uint64_t,uint64_t> h;
    h.set(0ULL,0ULL);     h.debug();
    h.set(64ULL,64ULL);   h.debug();
    h.set(1ULL,1ULL);     h.debug();
    h.set(65ULL,65ULL);   h.debug();
  }

  void funct1(int n)
  {
    hash<uint64_t,uint64_t> h;

    for( uint64_t i=0ULL;i<static_cast<uint64_t>(n);++i )
    {
      CSL_DEBUGF(L"set(%lld,%lld)\n",i+0,i);
      h.set( i,i );

#ifdef DEBUG
      h.debug();
#endif /*DEBUG*/
    }
  }

} // end of test_hash

using namespace test_hash;

int main()
{
#ifdef DEBUG
  //hash_set();
  funct1(3000);
#else
  csl_common_print_results( "hash_set            ", csl_common_test_timer_v0(hash_set),"" );
  csl_common_print_results( "t_item              ", csl_common_test_timer_v0(t_item),"" );
  csl_common_print_results( "t_page              ", csl_common_test_timer_v0(t_page),"" );
  csl_common_print_results( "t_hash              ", csl_common_test_timer_v0(t_hash),"" );

  csl_common_print_results( "funct1(5)                ", csl_common_test_timer_i1(funct1,5),"" );
  csl_common_print_results( "funct1(31)               ", csl_common_test_timer_i1(funct1,31),"" );
  csl_common_print_results( "funct1(50)               ", csl_common_test_timer_i1(funct1,50),"" );
  csl_common_print_results( "funct1(100)              ", csl_common_test_timer_i1(funct1,100),"" );
  csl_common_print_results( "funct1(3000)             ", csl_common_test_timer_i1(funct1,3000),"" );
#endif /*DEBUG*/
  return 0;
}

/* EOF */
