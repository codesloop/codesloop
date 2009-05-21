/*
Copyright (c) 2008,2009, David Beck

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
   @file t__chns.cc
   @brief Tests to check channel implementation possibilities
 */

#include "thread.hh"
#include "pbuf.hh"
#include "csl_sec.h"
#include "csl_sec.hh"
#include "common.h"
#include "test_timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <map>

using namespace csl::common;
using namespace csl::nthread;
using namespace csl::sec;
using namespace std;

/** @brief contains tests related to channel impl */
namespace test_chns {

  typedef map<unsigned long long,void *> map_t;
  static unsigned long long ids_[8*4096];
  static map_t mp_;

  void init()
  {
    csl_sec_gen_rand( ids_,sizeof(ids_) );

    for( int i=0;i<8*4096;++i )
      mp_.insert( std::pair<unsigned long long,void *>(ids_[i],0) );
  }

  void insert_map()
  {
    map_t m;

    for( int i=0;i<8*4096;++i )
    {
      m.insert( std::pair<unsigned long long,void *>(ids_[i],0) );
    }
  }

  void lookup_map()
  {
    for( int i=0;i<8*4096;++i )
    {
      mp_.find( ids_[i] );
    }
  }

  void lookup_map2()
  {
    for( unsigned long long i=0;i<8*4096;++i )
    {
      mp_.find( i );
    }
  }

  void change()
  {
    static int i=0;
    ++i;
    mp_.erase( ids_[i%4096] );
    mp_.insert( std::pair<unsigned long long,void *>(ids_[i%4096]/2+13,0) );
  }

  struct dta
  {
    void * ptr_;
    char   peer_pos_;
    unsigned long long clock_;
    unsigned long long peer_[8];
    unsigned long long me_[8];

    dta() : ptr_(0), peer_pos_(0), clock_(0) {}

    void swap_me(unsigned long long oldr, unsigned long long newr)
    {
      for( int i=0;i<8;++i )
      {
        if( me_[i] == oldr ) { me_[i] = newr; return; }
      }
      // bail
      me_[0] = newr;
    }
  };

  struct chns
  {
    unsigned long long max_;
    unsigned long long clock_;

    typedef map<unsigned long long,dta *>  clock_2_dta_t;
    typedef map<unsigned long long,dta *>  salt_2_dta_t;

    salt_2_dta_t   map_me_;
    clock_2_dta_t  clock_peer_;

    /* register peer */
    void reg( unsigned long long me[8], unsigned long long peer[8], void * p )
    {
      dta * d = new dta();

      memcpy( d->peer_,peer,sizeof(peer) );
      memcpy( d->me_,me,sizeof(me) );

      d->clock_ = (clock_++);

      for( int i=0;i<8;++i )
      {
        map_me_.insert( std::pair<unsigned long long,dta *>(me[i],d) );
      }

      clock_peer_.insert( std::pair<unsigned long long,dta *>(d->clock_,d) );

      if( clock_peer_.size() > max_ ) cleanup();
    }

    void cleanup()
    {
    }

    /* receiving data */
    dta * swap_find( unsigned long long oldr, unsigned long long newr )
    {
      dta * ret = 0;

      salt_2_dta_t::iterator it = map_me_.find(oldr);
      if( it == map_me_.end() ) return 0;

      ret = (*it).second;

      clock_2_dta_t::iterator itc = clock_peer_.find( ret->clock_ );
      if( itc == clock_peer_.end() ) return 0; // TODO : remove client ???

      ret->clock_ = (clock_++);
      ret->swap_me( oldr, newr );

      clock_peer_.erase( itc );
      map_me_.erase( it );

      clock_peer_.insert( std::pair<unsigned long long,dta *>(ret->clock_,ret) );
      map_me_.insert( std::pair<unsigned long long,dta *>(newr,ret) );

      return ret;
    }

    /* sending data */
    void update_peer(  )
    {
      // ???
    }

    chns() : max_(2), clock_(0) {}
  };

  void test_chn()
  {
    chns c;
    unsigned long long me[8];
    unsigned long long peer[8];

    /* gen rand */
    csl_sec_gen_rand(me,sizeof(me));
    csl_sec_gen_rand(peer,sizeof(peer));

    /**/
    c.reg( me,peer,0 );
  }
} // end of test_chns

using namespace test_chns;

int main()
{
  init();
  test_chn();

  csl_common_print_results( "insert_map    ", csl_common_test_timer_v0(insert_map),"" );
  csl_common_print_results( "lookup_map    ", csl_common_test_timer_v0(lookup_map),"" );
  csl_common_print_results( "lookup_map2   ", csl_common_test_timer_v0(lookup_map2),"" );
  csl_common_print_results( "change        ", csl_common_test_timer_v0(change),"" );

  return 0;
}

/* EOF */
