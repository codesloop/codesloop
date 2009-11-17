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
   @file t__sqlite.cc
   @brief Tests to check and measure various sqlite features
 */

#include "codesloop/common/common.h"
#include "codesloop/common/test_timer.h"
#include "codesloop/db/slt3/sqlite3.h"
#include "codesloop/db/slt3/conn.hh"
#include "codesloop/db/slt3/tran.hh"
#include "codesloop/db/slt3/query.hh"
#include <math.h>
#include <assert.h>

using namespace csl::db;

using csl::common::str;
using csl::common::ustr;
using csl::common::int64;
using csl::common::dbl;
using csl::common::binry;

/** @brief contains tests related to sqlite */
namespace test_sqlite {

  /** @test open and close with sqlite3 api */
  void open_close()
  {
    sqlite3 * db = 0;
    assert( sqlite3_open( "test.db", &db ) == SQLITE_OK );
    assert( sqlite3_close( db ) == SQLITE_OK );
  }

  /** @test baseline for performance comparison */
  void conn_baseline()
  {
    slt3::conn c;
    assert( c.open("test.db") == true );
  }

  /** @test tran test */
  void tran_query()
  {
    slt3::conn c;
    assert( c.open("test.db") == true );
    {
      slt3::tran t(c);
      {
        slt3::tran st(t);
      }
    }
  }

  /** @test create and drop table */
  void crdrp_table()
  {
    slt3::conn c;
    assert( c.open("test.db") == true );
    {
      slt3::tran t(c);
      {
        slt3::tran st(t);
        slt3::query q(st);
        assert( q.execute("CREATE TABLE T(i INT);") == true );
        assert( q.execute("DROP TABLE T;;") == true );
      }
    }
  }

  /** @test insert value and query count() */
  void single_return()
  {
    slt3::conn c;
    assert( c.open("test.db") == true );
    {
      slt3::tran t(c);
      {
        slt3::tran st(t);
        slt3::query q(st);
        ustr v;
        assert( q.execute("CREATE TABLE t(i INT);") == true );
        assert( q.execute("INSERT INTO t (i) VALUES(1);") == true );
        assert( q.execute("SELECT COUNT(*) FROM t;",v) == true );
        assert( q.execute("DROP TABLE t;") == true );
        assert( v == "1" );
        t.commit();
      }
    }
  }

  /** @test insert value with parametrized query and query count() and sum() */
  void test_params()
  {
    slt3::conn c;
    assert( c.open("test.db") == true );
    {
      slt3::tran t(c);
      {
        slt3::tran st(t);
        slt3::query q(st);
        ustr v;
        assert( q.execute("CREATE TABLE tint(i INT NOT NULL);") == true );
        assert( q.prepare("INSERT INTO tint (i) VALUES(?);") == true );
        int64 & p(q.int64_param(1));

        for( int i=0; i<10; ++i )
        {
          p.from_integer(100ll+i);
          assert( q.next() == false );
          assert( q.reset() == true );
        }

        assert( q.execute("SELECT COUNT(*) FROM tint;",v) == true );
        assert( v == "10" );
        assert( q.execute("SELECT SUM(i) FROM tint;",v) == true );
        assert( v == "1045" );
        assert( q.execute("DROP TABLE tint;") == true );
      }
    }
  }

  /** @test insert multiple integer values w/ parametrized query */
  void int_param()
  {
    slt3::conn c;
    assert( c.open("test.db") == true );
    slt3::tran t(c);
    slt3::query q(t);
    assert( q.execute("CREATE TABLE IF NOT EXISTS intp (i1 INT,i2 INT,i3 INT,i4 INT);") == true );
    assert( q.prepare("INSERT INTO intp (i1,i2,i3,i4) VALUES(?,?,?,?);") == true );
    //
    int64 & p1(q.int64_param(1));
    int64 & p2(q.int64_param(2));
    int64 & p3(q.int64_param(3));
    int64 & p4(q.int64_param(4));
    //
    for( int i=0; i<10; ++i )
    {
      p1.from_integer(100ll+i);
      p2.from_integer(200ll+i);
      p3.from_integer(300ll+i);
      p4.from_integer(400ll+i);
      assert( q.next() == false );
      assert( q.reset() == true );
    }
    //
    assert( q.prepare("SELECT * FROM intp WHERE i1=? AND i2=? AND i3=? AND i4=?;") == true );
    //
    slt3::query::columns_t cols;
    slt3::query::fields_t  flds;
    //
    for( int i=0; i<10; ++i )
    {
      p1.from_integer(100ll+i);
      p2.from_integer(200ll+i);
      p3.from_integer(300ll+i);
      p4.from_integer(400ll+i);

      assert( q.next(cols,flds) == true );

      {
        assert( cols.get_at(0)->type_ == slt3::query::colhead::t_integer );
        assert( cols.get_at(1)->type_ == slt3::query::colhead::t_integer );
        assert( cols.get_at(2)->type_ == slt3::query::colhead::t_integer );
        assert( cols.get_at(3)->type_ == slt3::query::colhead::t_integer );

        assert( str("i1") == cols.get_at(0)->name_ );
        assert( str("i2") == cols.get_at(1)->name_ );
        assert( str("i3") == cols.get_at(2)->name_ );
        assert( str("i4") == cols.get_at(3)->name_ );

        assert( str("intp") == cols.get_at(0)->table_ );
        assert( str("intp") == cols.get_at(1)->table_ );
        assert( str("intp") == cols.get_at(2)->table_ );
        assert( str("intp") == cols.get_at(3)->table_ );

        assert( str("main") == cols.get_at(0)->db_ );
        assert( str("main") == cols.get_at(1)->db_ );
        assert( str("main") == cols.get_at(2)->db_ );
        assert( str("main") == cols.get_at(3)->db_ );
      }

      {
        assert( (reinterpret_cast<int64 *>(flds.get_at(0)))->value() == 100ll+i );
        assert( (reinterpret_cast<int64 *>(flds.get_at(1)))->value() == 200ll+i );
        assert( (reinterpret_cast<int64 *>(flds.get_at(2)))->value() == 300ll+i );
        assert( (reinterpret_cast<int64 *>(flds.get_at(3)))->value() == 400ll+i );
      }

      assert( q.next(cols,flds) == false );
      assert( q.reset() );
      q.reset_data();
    }
    assert( q.execute("DROP TABLE intp;") == true );
  }

  /** @test insert multiple double values w/ parametrized query */
  void double_param()
  {
    slt3::conn c;
    assert( c.open("test.db") == true );
    slt3::tran t(c);
    slt3::query q(t);
    assert( q.execute("CREATE TABLE IF NOT EXISTS doublep (d1 REAL,d2 REAL,d3 REAL,d4 REAL);") == true );
    assert( q.prepare("INSERT INTO doublep (d1,d2,d3,d4) VALUES(?,?,?,?);") == true );
    //
    dbl & p1(q.dbl_param(1));
    dbl & p2(q.dbl_param(2));
    dbl & p3(q.dbl_param(3));
    dbl & p4(q.dbl_param(4));
    //
    for( int i=0; i<10; ++i )
    {
      p1.from_double(0.11+i);
      p2.from_double(0.12+i);
      p3.from_double(0.13+i);
      p4.from_double(0.14+i);
      assert( q.next() == false );
      assert( q.reset() == true );
    }
    //
    assert( q.prepare("SELECT * FROM doublep WHERE d1=? AND d2=? AND d3=? AND d4=?;") == true );
    //
    slt3::query::columns_t cols;
    slt3::query::fields_t  flds;
    //
    for( int i=0; i<10; ++i )
    {
      p1.from_double(0.11+i);
      p2.from_double(0.12+i);
      p3.from_double(0.13+i);
      p4.from_double(0.14+i);

      assert( q.next(cols,flds) == true );

      {
        assert( cols.get_at(0)->type_ == slt3::query::colhead::t_double );
        assert( cols.get_at(1)->type_ == slt3::query::colhead::t_double );
        assert( cols.get_at(2)->type_ == slt3::query::colhead::t_double );
        assert( cols.get_at(3)->type_ == slt3::query::colhead::t_double );

        assert( str("d1") == cols.get_at(0)->name_ );
        assert( str("d2") == cols.get_at(1)->name_ );
        assert( str("d3") == cols.get_at(2)->name_ );
        assert( str("d4") == cols.get_at(3)->name_ );

        assert( str("doublep") == cols.get_at(0)->table_ );
        assert( str("doublep") == cols.get_at(1)->table_ );
        assert( str("doublep") == cols.get_at(2)->table_ );
        assert( str("doublep") == cols.get_at(3)->table_ );

        assert( str("main") == cols.get_at(0)->db_ );
        assert( str("main") == cols.get_at(1)->db_ );
        assert( str("main") == cols.get_at(2)->db_ );
        assert( str("main") == cols.get_at(3)->db_ );
      }

      {
        assert( fabs( (reinterpret_cast<dbl *>(flds.get_at(0)))->value() - (0.11+i)) < 0.00000000001 );
        assert( fabs( (reinterpret_cast<dbl *>(flds.get_at(1)))->value() - (0.12+i)) < 0.00000000001 );
        assert( fabs( (reinterpret_cast<dbl *>(flds.get_at(2)))->value() - (0.13+i)) < 0.00000000001 );
        assert( fabs( (reinterpret_cast<dbl *>(flds.get_at(3)))->value() - (0.14+i)) < 0.00000000001 );
      }
      assert( q.next(cols,flds) == false );
      assert( q.reset() );
      q.reset_data();
    }
    assert( q.execute("DROP TABLE doublep;") == true );
  }

  /** @test insert multiple string values w/ parametrized query */
  void string_param()
  {
    slt3::conn c;
    assert( c.open("test.db") == true );
    slt3::tran t(c);
    slt3::query q(t);
    assert( q.execute("CREATE TABLE IF NOT EXISTS textp (t1 TEXT,t2 TEXT);") == true );
    assert( q.prepare("INSERT INTO textp (t1,t2) VALUES(?,?);") == true );
    //
    ustr & p1(q.ustr_param(1));
    ustr & p2(q.ustr_param(2));
    //
    p1.from_string("'_.\"");
    p2.from_string("0123.'\"");
    assert( q.next() == false );
    assert( q.reset() == true );
    //
    assert( q.prepare("SELECT * FROM textp WHERE t1=? AND t2=?;") == true );
    //
    slt3::query::columns_t cols;
    slt3::query::fields_t  flds;
    //
    p1.from_string("'_.\"");
    p2.from_string("0123.'\"");

    assert( q.next(cols,flds) == true );

    {
      assert( cols.get_at(0)->type_ == slt3::query::colhead::t_string );
      assert( cols.get_at(1)->type_ == slt3::query::colhead::t_string );

      assert( ustr("t1") == cols.get_at(0)->name_ );
      assert( ustr("t2") == cols.get_at(1)->name_ );

      assert( ustr("textp") == cols.get_at(0)->table_ );
      assert( ustr("textp") == cols.get_at(1)->table_ );

      assert( ustr("main") == cols.get_at(0)->db_ );
      assert( ustr("main") == cols.get_at(1)->db_ );
    }

    {
      ustr * f0 = reinterpret_cast<ustr *>(flds.get_at(0));
      assert( (f0)->nchars() == 4 );

      ustr * f1 = reinterpret_cast<ustr *>(flds.get_at(1));
      assert( (f1)->nchars() == 7 );

      assert( *f0 == "'_.\"" );
      assert( *f1 == "0123.'\"" );
    }

    assert( q.next(cols,flds) == false );
    assert( q.reset() );
    q.reset_data();
    assert( q.execute("DROP TABLE textp;") == true );
  }

  /** @test insert multiple blob values w/ parametrized query */
  void blob_param()
  {
    slt3::conn c;
    assert( c.open("test.db") == true );
    slt3::tran t(c);
    slt3::query q(t);
    assert( q.execute("CREATE TABLE IF NOT EXISTS blobp (b1 BLOB,b2 BLOB);") == true );
    assert( q.prepare("INSERT INTO blobp (b1,b2) VALUES(?,?);") == true );
    //
    unsigned char tx[127];
    unsigned char qx[3890];

    for( unsigned int i=0;i<sizeof(tx);++i ) { tx[i] = static_cast<unsigned char>(i%99); }
    for( unsigned int i=0;i<sizeof(qx);++i ) { qx[i] = static_cast<unsigned char>(i%111); }
    //
    binry & p1(q.binry_param(1));
    binry & p2(q.binry_param(2));
    //
    p1.from_binary(tx,sizeof(tx));
    p2.from_binary(qx,sizeof(qx));
    //
    assert( q.next() == false );
    assert( q.reset() == true );
    //
    assert( q.prepare("SELECT * FROM blobp WHERE b1=? AND b2=?;") == true );
    //
    slt3::query::columns_t cols;
    slt3::query::fields_t  flds;
    //
    p1.from_binary(tx,sizeof(tx));
    p2.from_binary(qx,sizeof(qx));

    assert( q.next(cols,flds) == true );

    {
      assert( cols.get_at(0)->type_ == slt3::query::colhead::t_blob );
      assert( cols.get_at(1)->type_ == slt3::query::colhead::t_blob );

      assert( str("b1") == cols.get_at(0)->name_ );
      assert( str("b2") == cols.get_at(1)->name_ );

      assert( str("blobp") == cols.get_at(0)->table_ );
      assert( str("blobp") == cols.get_at(1)->table_ );

      assert( str("main") == cols.get_at(0)->db_ );
      assert( str("main") == cols.get_at(1)->db_ );
    }

    {
      binry * f0 = reinterpret_cast<binry *>(flds.get_at(0));
      binry * f1 = reinterpret_cast<binry *>(flds.get_at(1));

      assert( memcmp(f0->value().data(),tx,sizeof(tx)) == 0 );
      assert( memcmp(f1->value().data(),qx,sizeof(qx)) == 0 );
    }

    assert( q.next(cols,flds) == false );
    assert( q.reset() );
    q.reset_data();
    assert( q.execute("DROP TABLE blobp;") == true );
  }

} // end of test_sqlite

using namespace test_sqlite;

int main()
{
  UNLINK( "test.db" );
  csl_common_print_results( "test_params    ", csl_common_test_timer_v0(test_params),"" );
  csl_common_print_results( "int_param      ", csl_common_test_timer_v0(int_param),"" );
  csl_common_print_results( "double_param   ", csl_common_test_timer_v0(double_param),"" );
  csl_common_print_results( "string_param   ", csl_common_test_timer_v0(string_param),"" );
  csl_common_print_results( "blob_param     ", csl_common_test_timer_v0(blob_param),"" );
  csl_common_print_results( "open_close     ", csl_common_test_timer_v0(open_close),"" );
  csl_common_print_results( "conn_baseline  ", csl_common_test_timer_v0(conn_baseline),"" );
  csl_common_print_results( "tran_query     ", csl_common_test_timer_v0(tran_query),"" );
  csl_common_print_results( "crdrp_table    ", csl_common_test_timer_v0(crdrp_table),"" );
  csl_common_print_results( "single_return  ", csl_common_test_timer_v0(single_return),"" );
  UNLINK( "test.db" );
  return 0;
}

/* EOF */
