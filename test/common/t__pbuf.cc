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
   @file t__pbuf.cc
   @brief Tests to verify pbuf
 */

#include "codesloop/common/pbuf.hh"
#include "codesloop/common/test_timer.h"
#include "codesloop/common/zfile.hh"
#include "codesloop/common/common.h"
#include <assert.h>

using csl::common::pbuf;
using csl::common::zfile;

/** @brief contains tests related to pbuf */
namespace test_pbuf {

  /** @test baseline for performance comparison */
  void baseline() { pbuf pb; }

  /** @test to allocate at least 13 buffer pages */
  void alloc_13()
  {
    pbuf pb;
    unsigned char tmp[] = { "Helloworld" }; // 11 bytes

    unsigned int sz = 0;
    while( sz < (pbuf::buf_size*13) )
    {
      assert( pb.append(tmp,sizeof(tmp)) == true );
      sz += sizeof(tmp);
      assert( pb.size() == sz );
      assert( pb.n_bufs() >= sz / pbuf::buf_size );
    }

    pbuf::iterator it(pb.begin());
    pbuf::iterator end(pb.end());

    unsigned int ck = 0;
    unsigned int pk = 0;

    for( ;it!=end;++it )
    {
      for( unsigned int i=0; i<(*it)->size_; ++i )
      {
        assert( (*it)->data_[i] == tmp[(ck%11)] );
        ++ck;
      }
      ++pk;
    }
    assert( pk > 11 );
    assert( ck == pb.size() );
    assert( ck == sz );
  }

  /** @test to check pbuf iterators */
  void test_iterator()
  {
    pbuf pb;
    pbuf::iterator it(pb.begin());
    pbuf::iterator end(pb.end());

    /* if empty, then begin==end */
    assert( it == end );

    assert( pb.append( reinterpret_cast<const unsigned char *>("Hello"),5) == true );
    it = pb.begin();

    assert( it != end );
    assert( end == pb.end() );
  }

  /** @test to check pbuf constant iterators */
  void test_const_iterator()
  {
    pbuf pb;
    const pbuf & pbc(pb);

    pbuf::const_iterator it(pbc.begin());
    pbuf::const_iterator end(pbc.end());

    /* if empty, then begin==end */
    assert( it == end );

    assert( pb.append( reinterpret_cast<const unsigned char *>("Hello"),5) == true );
    it = pbc.begin();

    assert( it != end );
    assert( end == pbc.end() );
  }

  /** @test copy operator and constructor */
  void test_copy()
  {
    /* hello world string */
    {
      pbuf pb1;
      assert( pb1.append( reinterpret_cast<const unsigned char *>("Hello world"),12) == true );
      pbuf pb2(pb1);
      assert( pb1 == pb2 );
      pbuf pb3;
      pb3 = pb1;
      assert( pb1 == pb3 );
      assert( pb1.size() == 12 );
      assert( pb2.size() == 12 );
      assert( pb3.size() == 12 );

      unsigned char tmp1[12]; assert( pb1.copy_to(tmp1) == true );
      unsigned char tmp2[12]; assert( pb2.copy_to(tmp2) == true );
      unsigned char tmp3[12]; assert( pb3.copy_to(tmp3) == true );

      assert( ::memcmp(tmp1,tmp2,12) == 0 );
      assert( ::memcmp(tmp1,tmp3,12) == 0 );
      assert( ::memcmp(tmp1,"Hello world",12) == 0 );
    }

    /* random data */
    {
      zfile zf;
      assert( zf.read_file("random.204800") == true );
      assert( zf.get_size() == 204800 );

      pbuf pb1;
      assert( zf.get_data(pb1) == true );
      assert( pb1.size() == 204800 );

      pbuf pb2(pb1);
      assert( pb1 == pb2 );
      assert( pb2.size() == 204800 );

      pbuf pb3;
      pb3 = pb1;
      assert( pb3.size() == 204800 );
      assert( pb1 == pb3 );

      unsigned char tmp1[204800]; assert( pb1.copy_to(tmp1) == true );
      unsigned char tmp2[204800]; assert( pb2.copy_to(tmp2) == true );
      unsigned char tmp3[204800]; assert( pb3.copy_to(tmp3) == true );

      assert( ::memcmp(tmp1,tmp2,204800) == 0 );
      assert( ::memcmp(tmp1,tmp3,204800) == 0 );
    }
  }

} // end of test_pbuf

using namespace test_pbuf;

int main()
{
  csl_common_print_results( "baseline            ", csl_common_test_timer_v0(baseline),"" );
  csl_common_print_results( "alloc_13            ", csl_common_test_timer_v0(alloc_13),"" );
  csl_common_print_results( "test_iterator       ", csl_common_test_timer_v0(test_iterator),"" );
  csl_common_print_results( "test_const_iterator ", csl_common_test_timer_v0(test_const_iterator),"" );
  csl_common_print_results( "test_copy           ", csl_common_test_timer_v0(test_copy),"" );

  return 0;
}

/* EOF */
