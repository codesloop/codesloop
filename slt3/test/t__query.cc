/*
Copyright (c) 2008,2009, David Beck, Tamas Foldi

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
   @file t__query.cc
   @brief Tests to check and measure various slt3::query features
 */

#include "test_timer.h"
#include "query.hh"
#include "tran.hh"
#include "conn.hh"
#include "exc.hh"
#include "str.hh"
#include "common.h"
#include <assert.h>

using namespace csl::slt3;
using csl::common::str;
using csl::common::ustr;
using csl::common::int64;
using csl::common::binry;
using csl::common::dbl;

/** @brief contains tests related to slt3::query */
namespace test_query {

  /** @test baseline for performance comparison */
  void baseline()
  {
    conn c;
    c.use_exc(false);
    tran t(c);
    query q(t);

    assert( c.use_exc() == false );
    assert( t.use_exc() == false );
    assert( q.use_exc() == false );
  }

  /** @test querying a database that was not opened w/o throwing exception */
  void noopen_nothrow()
  {
    conn c;
    c.use_exc(false);
    tran t(c);
    query q(t);
    FPRINTF(stderr,L"Error message should follow here:\n");
    assert( q.execute("CREATE TABLE nothrow (i INT);") == false );
  }

  /** @test querying a database that was not opened w/ throwing exception */
  void noopen_throw()
  {
    bool caught = false;
    conn c;
    c.use_exc(true);
    try
    {
      tran t(c);
      query q(t);
      assert( q.execute("CREATE TABLE nothrow (i INT);") == false );
    }
    catch( exc e )
    {
      caught = true;
    }
    assert( caught == true );
  }

  /** @test declare colhead */
  void test_colhead() { query::colhead ch; }

  /** @test parameter handling */
  void test_param()
  {
    conn c;
    c.use_exc(false);
    tran t(c);
    query q(t);

    dbl & pa1(q.dbl_param(1));
    ustr & pa2(q.ustr_param(2));
    int64 & pa3(q.int64_param(3));

    pa1.set(1.0);
    pa2.set("10000.001");
    pa3.set(100ll);

    dbl & pb1(q.dbl_param(1));
    ustr & pb2(q.ustr_param(2));
    int64 & pb3(q.int64_param(3));

    assert( pa1.get_long() == pb1.get_long() );
    assert( pa1.get_double() == pb1.get_double() );

    ustr lhs,rhs;
    assert( pa1.get(lhs) == true );
    assert( pb1.get(rhs) == true );
    assert( lhs == rhs );

    assert( pb1.get_long() == 1ll );
    assert( pb1.get_double() == 1.0 );

    assert( pb1.get(lhs) == true );
    assert( lhs == "1.000000000000" );

    assert( pa2.get_long() == pb2.get_long() );
    assert( pa2.get_double() == pb2.get_double() );

    assert( pa2.get(lhs) == true );
    assert( pb2.get(rhs) == true );

    assert( lhs == rhs );
    assert( pb2.get_long() == 10000ll );
    assert( pb2.get_double() == 10000.001 );

    assert( pb2.get(lhs) == true );
    assert( lhs == "10000.001" );

    assert( pa3.get_long() == pb3.get_long() );
    assert( pa3.get_double() == pb3.get_double() );

    assert( pa3.get(lhs) == true );
    assert( pb3.get(rhs) == true );

    assert( lhs == rhs );
    assert( pb3.get_long() == 100ll );
    assert( pb3.get_double() == 100.0 );


    assert( pb3.get(lhs) == true );
    assert( lhs == "100" );

    q.clear_params();
    ustr & pc1(q.ustr_param(1));
    assert( pc1.empty() == true );
  }

  /** @test iterative query w/o retval (no automatic reset) */
  void stepw_noret_noaut()
  {
    conn c;
    assert( c.open("test.db") == true );
    try
    {
      tran t(c);
      query q(t);
      q.autoreset_data(false);
      assert( q.execute("CREATE TABLE stepw_noret_noaut (o string);") == true );
      ustr & p(q.ustr_param(1));
      assert( q.prepare("INSERT INTO stepw_noret_noaut (o) VALUES (?);") == true );
      for( unsigned int i=0;i<100;++i )
      {
        p.set("Hello world");
        assert( q.next() == false );
        assert( q.reset() == true );
        assert( q.last_insert_id() > i );
        assert( q.change_count() == 1 );
        assert( q.change_count() == c.change_count() );
      }
      assert( q.execute("DELETE FROM stepw_noret_noaut;") == true );
      assert( q.change_count() == 100 );
      assert( c.change_count() == 100 );
      assert( q.execute("DROP TABLE stepw_noret_noaut;") == true );
    }
    catch(exc e)
    {
      str s; e.to_string(s);
      FPRINTF(stderr,L"ERROR: %ls\n",s.c_str());
    }
  }

  /** @test iterative query w/o retval (automatic reset) */
  void stepw_noret_aut()
  {
    conn c;
    assert( c.open("test.db") == true );
    try
    {
      tran t(c);
      query q(t);
      q.autoreset_data(true);
      assert( q.execute("CREATE TABLE stepw_noret_aut (o string);") == true );
      ustr & p(q.ustr_param(1));
      assert( q.prepare("INSERT INTO stepw_noret_aut (o) VALUES (?);") == true );
      for( unsigned int i=0;i<100;++i )
      {
        p.set("Hello world");
        assert( q.next() == false );
        assert( q.reset() == true );
        assert( q.last_insert_id() > i );
      }
      assert( q.execute("DROP TABLE stepw_noret_aut;") == true );
    }
    catch(exc e)
    {
      str s; e.to_string(s);
      FPRINTF(stderr,L"ERROR: %ls\n",s.c_str());
    }
  }

  /** @test iterative query w/ retval (automatic reset) */
  void stepw_ret_noaut()
  {
    conn c;
    assert( c.open("test.db") == true );
    try
    {
      tran t(c);
      query q(t);
      q.autoreset_data(false);
      assert( q.execute("CREATE TABLE stepw_ret_noaut (o string);") == true );
      assert( q.execute("INSERT INTO stepw_ret_noaut (o) VALUES ('Hello');") == true );

      ustr & p(q.ustr_param(1));
      assert( q.prepare("SELECT o FROM stepw_ret_noaut WHERE o=?;") == true );

      query::columns_t ch;
      query::fields_t  fd;

      for( unsigned int i=0;i<100;++i )
      {
        p.set("Hello");
        assert( q.next(ch,fd) == true );
        assert( q.reset() == true );
        ustr * s0 = reinterpret_cast<ustr *>(fd.get_at(0));
        assert( *s0 == "Hello" );
        assert( s0->nchars() == 5 );
      }

      assert( q.execute("DROP TABLE stepw_ret_noaut;") == true );
    }
    catch(exc e)
    {
      str s; e.to_string(s);
      FPRINTF(stderr,L"ERROR: %ls\n",s.c_str());
    }
  }

  /** @test iterative query w/ retval (automatic reset) */
  void stepw_ret_aut()
  {
    conn c;
    assert( c.open("test.db") == true );
    try
    {
      tran t(c);
      query q(t);
      q.autoreset_data(true);
      assert( q.execute("CREATE TABLE stepw_ret_aut (o string);") == true );
      assert( q.execute("INSERT INTO stepw_ret_aut (o) VALUES ('Hello');") == true );
      assert( q.last_insert_id() > 0 );

      ustr & p(q.ustr_param(1));
      assert( q.prepare("SELECT o FROM stepw_ret_aut WHERE o=?;") == true );

      query::columns_t ch;
      query::fields_t  fd;

      for( unsigned int i=0;i<100;++i )
      {
        p.set("Hello");
        assert( q.next(ch,fd) == true );
        assert( q.reset() == true );
        ustr * s0 = reinterpret_cast<ustr *>(fd.get_at(0));
        assert( *s0 == "Hello" );
        assert( s0->size() == 5 );
      }

      assert( q.execute("DROP TABLE stepw_ret_aut;") == true );
    }
    catch(exc e)
    {
      str s; e.to_string(s);
      FPRINTF(stderr,L"ERROR: %ls\n",s.c_str());
    }
  }

  /** @test onestep query w/o retval (no automatic reset) */
  void onesht_noret_noaut()
  {
    conn c;
    assert( c.open("test.db") == true );
    try
    {
      tran t(c);
      query q(t);
      q.autoreset_data(false);
      assert( q.execute("CREATE TABLE onesht_noret_noaut (o string);") == true );
      assert( q.execute("INSERT INTO onesht_noret_noaut (o) VALUES ('Hello');") == true );
      assert( q.execute("SELECT o FROM onesht_noret_noaut WHERE o='Hello';") == true );
      assert( q.execute("DROP TABLE onesht_noret_noaut;") == true );
    }
    catch(exc e)
    {
      str s; e.to_string(s);
      FPRINTF(stderr,L"ERROR: %ls\n",s.c_str());
    }
  }

  /** @test onestep query w/o retval (automatic reset) */
  void onesht_noret_aut()
  {
    conn c;
    assert( c.open("test.db") == true );
    try
    {
      tran t(c);
      query q(t);
      q.autoreset_data(true);
      assert( q.execute("CREATE TABLE onesht_noret_aut (o string);") == true );
      assert( q.execute("INSERT INTO onesht_noret_aut (o) VALUES ('Hello');") == true );
      assert( q.execute("SELECT o FROM onesht_noret_aut WHERE o='Hello';") == true );
      assert( q.execute("DROP TABLE onesht_noret_aut;") == true );
    }
    catch(exc e)
    {
      str s; e.to_string(s);
      FPRINTF(stderr,L"ERROR: %ls\n",s.c_str());
    }
  }

  /** @test onestep query w/ retval (no automatic reset) */
  void onesht_ret_noaut()
  {
    conn c;
    assert( c.open("test.db") == true );
    try
    {
      tran t(c);
      query q(t);
      q.autoreset_data(false);
      assert( q.execute("CREATE TABLE onesht_ret_noaut (o string);") == true );
      assert( q.execute("INSERT INTO onesht_ret_noaut (o) VALUES ('Hello');") == true );
      ustr s;
      assert( q.execute("SELECT o FROM onesht_ret_noaut WHERE o='Hello';",s) == true );
      assert( s == "Hello" );
      assert( q.execute("DROP TABLE onesht_ret_noaut;") == true );
    }
    catch(exc e)
    {
      str s; e.to_string(s);
      FPRINTF(stderr,L"ERROR: %ls\n",s.c_str());
    }
  }

  /** @test onestep query w/ retval (utomatic reset) */
  void onesht_ret_aut()
  {
    conn c;
    assert( c.open("test.db") == true );
    try
    {
      tran t(c);
      query q(t);
      q.autoreset_data(true);
      assert( q.execute("CREATE TABLE onesht_ret_aut (o string);") == true );
      assert( q.execute("INSERT INTO onesht_ret_aut (o) VALUES ('Hello');") == true );
      ustr s;
      assert( q.execute("SELECT o FROM onesht_ret_aut WHERE o='Hello';",s) == true );
      assert( s == "Hello" );
      assert( q.execute("DROP TABLE onesht_ret_aut;") == true );
    }
    catch(exc e)
    {
      str s; e.to_string(s);
      FPRINTF(stderr,L"ERROR: %ls\n",s.c_str());
    }
  }

  static conn * perf_conn_ = 0;
  static tran * perf_tran_ = 0;

  /** @test insert and delete integer */
  void ins_del_int()
  {
    tran t(*perf_conn_);
    query q(t);
    assert( q.execute("INSERT INTO perftest (i) VALUES(1);") == true );
    assert( q.last_insert_id() > 0 );
    assert( q.execute("DELETE FROM perftest WHERE i=1;") == true );
  }

  /** @test insert and delete double */
  void ins_del_double()
  {
    tran t(*perf_conn_);
    query q(t);
    assert( q.execute("INSERT INTO perftest (d) VALUES('3.14123');") == true );
    assert( q.last_insert_id() > 0 );
    assert( q.execute("DELETE FROM perftest WHERE d='3.14123';") == true );
  }

  /** @test insert and delete string */
  void ins_del_str()
  {
    tran t(*perf_conn_);
    query q(t);
    assert( q.execute("INSERT INTO perftest (s) VALUES('3.14123');") == true );
    assert( q.last_insert_id() > 0 );
    assert( q.execute("DELETE FROM perftest WHERE s='3.14123';") == true );
  }

  /** @test insert and delete blob */
  void ins_del_blob()
  {
    tran t(*perf_conn_);
    query q(t);
    assert( q.execute("INSERT INTO perftest (b) VALUES('3.14123');") == true );
    assert( q.last_insert_id() > 0 );
    assert( q.execute("DELETE FROM perftest WHERE b='3.14123';") == true );
  }

  /** @test insert and delete integer */
  void insdel_int_notran()
  {
    query q(*perf_tran_);
    assert( q.execute("INSERT INTO perftest (i) VALUES(1);") == true );
    assert( q.last_insert_id() > 0 );
    assert( q.execute("DELETE FROM perftest WHERE i=1;") == true );
  }

} // end of test_query

using namespace test_query;

int main()
{
  noopen_nothrow();
  noopen_throw();

  UNLINK( "test.db" );
  csl_common_print_results( "baseline           ", csl_common_test_timer_v0(baseline),"" );
  csl_common_print_results( "test_colhead       ", csl_common_test_timer_v0(test_colhead),"" );
  csl_common_print_results( "test_param         ", csl_common_test_timer_v0(test_param),"" );
  csl_common_print_results( "stepw_noret_noaut  ", csl_common_test_timer_v0(stepw_noret_noaut),"" );
  csl_common_print_results( "stepw_noret_aut    ", csl_common_test_timer_v0(stepw_noret_aut),"" );
  csl_common_print_results( "stepw_ret_noaut    ", csl_common_test_timer_v0(stepw_ret_noaut),"" );
  csl_common_print_results( "stepw_ret_aut      ", csl_common_test_timer_v0(stepw_ret_aut),"" );
  csl_common_print_results( "onesht_noret_noaut ", csl_common_test_timer_v0(onesht_noret_noaut),"" );
  csl_common_print_results( "onesht_noret_aut   ", csl_common_test_timer_v0(onesht_noret_aut),"" );
  csl_common_print_results( "onesht_ret_noaut   ", csl_common_test_timer_v0(onesht_ret_noaut),"" );
  csl_common_print_results( "onesht_ret_aut     ", csl_common_test_timer_v0(onesht_ret_aut),"" );
  UNLINK( "test.db" );

  UNLINK( "test2.db" );
  {
    conn c;
    perf_conn_ = &c;
    assert( c.open("test2.db") == true );
    {
      tran t(c); query q(t);
      assert( q.execute("CREATE TABLE perftest (i int, d real, s string, b blob);") == true );
    }
    csl_common_print_results( "ins_del_int        ", csl_common_test_timer_v0(ins_del_int),"" );
    csl_common_print_results( "ins_del_double     ", csl_common_test_timer_v0(ins_del_double),"" );
    csl_common_print_results( "ins_del_str        ", csl_common_test_timer_v0(ins_del_str),"" );
    csl_common_print_results( "ins_del_blob       ", csl_common_test_timer_v0(ins_del_blob),"" );
    {
      tran t(c);
      perf_tran_ = &t;
      csl_common_print_results( "insdel_int_notran  ", csl_common_test_timer_v0(insdel_int_notran),"" );
    }
    assert( c.close() == true );
  }
  UNLINK( "test2.db" );
  return 0;
}

/* EOF */
