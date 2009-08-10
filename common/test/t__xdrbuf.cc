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
   @file t__xdrbuf.cc
   @brief Tests to verify xdr utilities
 */

#ifndef DEBUG
#define DEBUG
#endif /* DEBUG */

#include "xdrbuf.hh"
#include "pbuf.hh"
#include "zfile.hh"
#include "mpool.hh"
#include "common.h"
#include "test_timer.h"
#include "str.hh"
#include "ustr.hh"
#include "exc.hh"
#include <assert.h>

using namespace csl::common;

/** @brief contains tests related to xdr buffer */
namespace test_xdrbuf {

  /** @test baseline for performance comparison */
  void baseline()
  {
    pbuf pb;
    xdrbuf xb(pb);
    assert( xb.position() == 0 );
  }

  /** @test copy constructor */
  void test_copy()
  {
    pbuf pb;
    xdrbuf xb(pb);
    xdrbuf xc(xb);
    assert( xb == xc );
  }

  /** @test integer de/serialization */
  void test_int()
  {
    pbuf pb;
    xdrbuf xb(pb);
    unsigned int a = 0xbaddad;
    unsigned int b;
    xb << a;

    xb.rewind();

    xb >> b;
    assert( a == b );
    assert( xb.position() == sizeof(int32_t) );

    bool caught = false;
    try
    {
      unsigned int c;

      /* read more than available */
      xb >> c;
    }
    catch( csl::common::exc e )
    {
      caught = true;
      assert( e.reason_    == csl::common::exc::rs_xdr_eof );
      assert( e.component_ == L"csl::common::xdrbuf" );
    }
    assert( caught == true );
  }

  /** @test string de/serialization */
  void test_string()
  {
    pbuf pb;
    xdrbuf xb(pb);

    xb << L"Hello World";
    str hw;

    xb.rewind();

    assert( pb.size() == sizeof(wchar_t)*11+sizeof(int32_t) );

    xb >> hw;

    assert( hw.size() == 11 );
    assert( hw == "Hello World" );

    assert( xb.position() > 10 );

    /* save position */
    xdrbuf xx(xb);

    bool caught = false;
    try
    {
      unsigned int c;

      /* read more than available */
      xb >> c;
    }
    catch( csl::common::exc e )
    {
      caught = true;
      assert( e.reason_    == csl::common::exc::rs_xdr_eof );
      assert( e.component_ == L"csl::common::xdrbuf" );
    }
    assert( caught == true );

    caught = false;
    try
    {
      /* add invalid pointer */
      xx << reinterpret_cast<const char *>(0);

      str zz;

      xx >> zz;
    }
    catch( csl::common::exc e )
    {
      str es;
      e.to_string(es);
      FPRINTF(stderr,L"Exception caught: %ls\n",es.c_str());
      caught = true;
    }
    /* this should not throw an exception, will add as zero length string */
    assert( caught == false );
  }

  /** @test string de/serialization */
  void test_ustring()
  {
    pbuf pb;
    xdrbuf xb(pb);

    xb << "Hello World";
    ustr hw;

    xb.rewind();

    assert( pb.size() == 16 );

    xb >> hw;

    assert( hw.size() == 11 );
    assert( hw == "Hello World" );

    assert( xb.position() > 10 );

    /* save position */
    xdrbuf xx(xb);

    bool caught = false;
    try
    {
      unsigned int c;

      /* read more than available */
      xb >> c;
    }
    catch( csl::common::exc e )
    {
      caught = true;
      assert( e.reason_    == csl::common::exc::rs_xdr_eof );
      assert( e.component_ == L"csl::common::xdrbuf" );
    }
    assert( caught == true );

    caught = false;
    try
    {
      /* add invalid pointer */
      xx << reinterpret_cast<const char *>(0);

      str zz;

      xx >> zz;
    }
    catch( csl::common::exc e )
    {
      str es;
      e.to_string(es);
      FPRINTF(stderr,L"Exception caught: %ls\n",es.c_str());
      caught = true;
    }
    /* this should not throw an exception, will add as zero length string */
    assert( caught == false );
  }

  /** @test xdrbuf::bindata_t de/serialization */
  void test_bin()
  {
    zfile zf;
    assert( zf.read_file("random.204800") == true );
    assert( zf.get_size() == 204800 );
    mpool<> mp;
    unsigned char * ptr = reinterpret_cast<unsigned char *>(mp.allocate(zf.get_size()));
    assert( ptr != 0 );
    assert( zf.get_data(ptr) == true );

    pbuf pb;
    xdrbuf xb(pb);

    xb << xdrbuf::bindata_t(ptr,zf.get_size());

    unsigned char * ptr2 = reinterpret_cast<unsigned char *>(mp.allocate(zf.get_size()));
    assert( ptr2 != 0 );

    xb.rewind();
    unsigned int sz;
    assert( xb.get_data(ptr2,sz,204808) == true );
    assert( xb.position() == 204804 );
    assert( sz == zf.get_size() );
    assert( sz == 204800 );
    assert( ::memcmp(ptr,ptr2,sz) == 0 );
  }

  /** @test pbuf de/serialization */
  void test_pbuf()
  {
    zfile zf;
    assert( zf.read_file("random.2048") == true );
    assert( zf.get_size() == 2048 );
    pbuf ptr;
    assert( zf.get_data(ptr) == true );
    assert( ptr.size() == 2048 );

    pbuf pb;
    xdrbuf xb(pb);

    xb << ptr;

    assert( pb.size() == 2052 );

    xb.rewind();

    pbuf ptr2;
    xb >> ptr2;

    assert( ptr2.size() == 2048 );
    assert( ptr == ptr2 );
  }

  /** @test reading 2048 bytes of garbage integer */
  void garbage_int_small()
  {
    zfile zf;
    assert( zf.read_file("random.2048") == true );
    pbuf ptr;
    assert( zf.get_data(ptr) == true );
    assert( ptr.size() == 2048 );
    xdrbuf xb(ptr);

    int exc_caught = -2;

    try
    {
      xb.rewind();

      while( true )
      {
        unsigned int i;
        xb >> i;
      };
    }
    catch( csl::common::exc e )
    {
      exc_caught = e.reason_;
    }
    /* integer garbage cannot be validated, thus eof condition is checked */
    assert( exc_caught == csl::common::exc::rs_xdr_eof );
  }

  /** @test reading 204800 bytes of garbage integer */
  void garbage_int_large()
  {
    zfile zf;
    assert( zf.read_file("random.204800") == true );
    pbuf ptr;
    assert( zf.get_data(ptr) == true );
    assert( ptr.size() == 204800 );
    xdrbuf xb(ptr);

    int exc_caught = -2;

    try
    {
      xb.rewind();
      unsigned long j = 0;

      while( true )
      {
        unsigned int i;
        xb >> i;
        j += sizeof(int32_t);
        assert( xb.position() == j );
      };
    }
    catch( csl::common::exc e )
    {
      exc_caught = e.reason_;
    }
    /* integer garbage cannot be validated, thus eof condition is checked */
    assert( exc_caught == csl::common::exc::rs_xdr_eof );
  }

  /** @test reading 2048 bytes of garbage string */
  void garbage_string_small()
  {
    zfile zf;
    assert( zf.read_file("random.2048") == true );
    pbuf ptr;
    assert( zf.get_data(ptr) == true );
    assert( ptr.size() == 2048 );
    xdrbuf xb(ptr);

    int exc_caught = -2;

    try
    {
      xb.rewind();

      while( true )
      {
        str i;
        xb >> i;
      };
    }
    catch( csl::common::exc e )
    {
      exc_caught = e.reason_;
    }
    /* garbage string does not match the expected size */
    assert( exc_caught == csl::common::exc::rs_xdr_invalid );
  }

  /** @test reading 204800 bytes of garbage string */
  void garbage_string_large()
  {
    zfile zf;
    assert( zf.read_file("random.204800") == true );
    pbuf ptr;
    assert( zf.get_data(ptr) == true );
    assert( ptr.size() == 204800 );
    xdrbuf xb(ptr);

    int exc_caught = -2;

    try
    {
      xb.rewind();

      while( true )
      {
        str i;
        xb >> i;
      };
    }
    catch( csl::common::exc e )
    {
      exc_caught = e.reason_;
    }
    /* garbage string does not match the expected size */
    assert( exc_caught == csl::common::exc::rs_xdr_invalid );
  }

  /** @test reading 2048 bytes of garbage binary data to pbuf */
  void garbage_pbuf_small()
  {
    zfile zf;
    assert( zf.read_file("random.2048") == true );
    pbuf ptr;
    assert( zf.get_data(ptr) == true );
    assert( ptr.size() == 2048 );
    xdrbuf xb(ptr);

    int exc_caught = -2;

    try
    {
      xb.rewind();

      while( true )
      {
        pbuf i;
        xb >> i;
      };
    }
    catch( csl::common::exc e )
    {
      exc_caught = e.reason_;
    }
    /* garbage binary data does not match the expected size */
    assert( exc_caught == csl::common::exc::rs_xdr_invalid );
  }

  /** @test reading 204800 bytes of garbage binary data to pbuf */
  void garbage_pbuf_large()
  {
    zfile zf;
    assert( zf.read_file("random.204800") == true );
    pbuf ptr;
    assert( zf.get_data(ptr) == true );
    assert( ptr.size() == 204800 );
    xdrbuf xb(ptr);

    int exc_caught = -2;

    try
    {
      xb.rewind();

      while( true )
      {
        pbuf i;
        xb >> i;
        assert( i.size() == xb.position() );
      };
    }
    catch( csl::common::exc e )
    {
      exc_caught = e.reason_;
    }
    /* garbage binary data does not match the expected size */
    assert( exc_caught == csl::common::exc::rs_xdr_invalid );
  }

} // end of test_xdrbuf

using namespace test_xdrbuf;

int main()
{
  csl_common_print_results( "baseline             ", csl_common_test_timer_v0(baseline),"" );
  csl_common_print_results( "test_copy            ", csl_common_test_timer_v0(test_copy),"" );
  csl_common_print_results( "test_int             ", csl_common_test_timer_v0(test_int),"" );
  csl_common_print_results( "test_string          ", csl_common_test_timer_v0(test_string),"" );
  csl_common_print_results( "test_ustring         ", csl_common_test_timer_v0(test_ustring),"" );
  csl_common_print_results( "test_bin             ", csl_common_test_timer_v0(test_bin),"" );
  csl_common_print_results( "test_pbuf            ", csl_common_test_timer_v0(test_pbuf),"" );
  csl_common_print_results( "garbage_int_small    ", csl_common_test_timer_v0(garbage_int_small),"" );
  csl_common_print_results( "garbage_int_large    ", csl_common_test_timer_v0(garbage_int_large),"" );
  csl_common_print_results( "garbage_string_small ", csl_common_test_timer_v0(garbage_string_small),"" );
  csl_common_print_results( "garbage_string_large ", csl_common_test_timer_v0(garbage_string_large),"" );
  csl_common_print_results( "garbage_pbuf_small   ", csl_common_test_timer_v0(garbage_pbuf_small),"" );
  csl_common_print_results( "garbage_pbuf_large   ", csl_common_test_timer_v0(garbage_pbuf_large),"" );

  return 0;
}

/* EOF */
