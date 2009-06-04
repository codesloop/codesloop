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
   @file t__ustr.cc
   @brief Tests to verify unicode character string
 */

#define DEBUG

#include "logger.hh"
#include "exc.hh"
#include "common.h"
#include "str.hh"
#include "ustr.hh"
#include "test_timer.h"
#include <assert.h>
#include <sys/stat.h>

using csl::common::str;
using csl::common::ustr;
using csl::common::pbuf;

/** @brief contains tests related to tbuf */
namespace test_ustr {

  /** @test baseline for performance comparison */
  void string_baseline()
  {
    std::string b;
  }

  /** @test baseline for performance comparison */
  inline void str_baseline()
  {
    ustr b;
  }

  /** @test @todo */
  void string_hello()
  {
    std::string b;
    b = "Hello";
  }

  /** @test @todo */
  void str_hello()
  {
    ustr b;
    b = "Hello";
  }

  /** @test @todo */
  void string_concat()
  {
    std::string b;
    b = "Hello" + std::string("world");
  }

  /** @test @todo */
  void str_concat()
  {
    ustr b;
    b = "Hello" + ustr("world");
    assert( b.size() == 10 );
    assert( b.nchars() == 10 );
    assert( b.nbytes() == 11 );
  }

  /** @test @todo */
  void string_append()
  {
    std::string b;
    b += "Hello";
    b += " ";
    b += "world!";
  }

  /** @test @todo */
  void str_append()
  {
    ustr b;
    b += "Hello";
    b += " ";
    b += "world!";
    assert( b.size() == 12 );
    assert( b.nchars() == 12 );
    assert( b.nbytes() == 13 );
  }

  /** @test @todo */
  void str_opeq()
  {
    ustr b("Hello world");
    assert( b.size() == 11 );
    assert( b.empty() == false );
    assert( b.c_str() != 0 );
    assert( b.nchars() == 11 );
    assert( b.nbytes() == 12 );
  }

  void test_empty_constr()
  {
    ustr s;
    assert( s.nbytes() == 1 );
    assert( s.size() == 0 );
    assert( s.nchars() == 0 );
  }

  void test_opeq_pbuf()
  {
    ustr s;
    pbuf pb;
    pb.append("Hello");
    s = pb;
    assert( pb.size() == 6 );
    assert( s.size() == 5 );
    assert( s.nbytes() == 6 );
    assert( s.nchars() == 5 );
    assert( s == "Hello" );
    assert( memcmp( s.data(),"Hello",6 ) == 0 );
  }

  void test_cpyconstr()
  {
    ustr s1("Hello");

    assert( s1.size() == 5 );
    assert( s1.nbytes() == 6 );
    assert( s1.nchars() == 5 );

    ustr s2(s1);

    assert( s2.size() == 5 );
    assert( s2.nbytes() == 6 );
    assert( s2.nchars() == 5 );

    assert( s2 == "Hello" );
    assert( memcmp( s2.data(),"Hello",6 ) == 0 );
  }

  void test_cpy0()
  {
    ustr s1("Hello");
  }

  void test_cpyop()
  {
    ustr s;
    s = "Hello";
    assert( s.size() == 5 );
    assert( s.nbytes() == 6 );
    assert( s.nchars() == 5 );

    ustr s2;
    s2 = s;

    assert( s2.size() == 5 );
    assert( s2.nbytes() == 6 );
    assert( s2.nchars() == 5 );
  }

  void test_pluseq()
  {
    ustr s;
    s += "Hello";

    assert( s.size() == 5 );
    assert( s.nbytes() == 6 );
    assert( s.nchars() == 5 );

    s += " world";

    assert( s.size() == 11 );
    assert( s.nbytes() == 12 );
    assert( s.nchars() == 11 );

    assert( s == "Hello world" );
    assert( memcmp( s.data(),"Hello world",12 ) == 0 );
  }

  void test_find0()
  {
    ustr s("Hello World");
    assert( s.find("l") == 2 );
    assert( s.find('l') == 2 );
    assert( s.rfind('l') == 9 );
  }

  void test_substr0()
  {
    ustr s("Hello World");
    ustr s2 = s.substr(2,4);
    assert( s2 == "llo " );
  }
}

using namespace test_ustr;

int main()
{
  ustr s( "HELLO" );
  assert( s.size() == 5 );
  assert( strcmp(s.c_str(), "HELLO") == 0 );

  assert( s[0] == L'H' );
  assert( s[1] == L'E' );

  ustr s2 = s;
  assert( s2.size() == 5 );
  assert( strcmp(s2.c_str(), "HELLO") == 0 );

  ustr s3(s);
  assert( s3.size() == 5 );
  assert( strcmp(s3.c_str(), "HELLO") == 0 );

  s2 += " WORLD";
  assert( s2.size() == 11 );
  assert( strcmp(s2.c_str(), "HELLO WORLD") == 0 );

  s2 = s.substr(1,3) + s2.substr(6,999);
  assert( s2.size() == 8 );
  assert( strcmp(s2.c_str(), "ELLWORLD") == 0 );

  ustr cs( "HELLO" );
  assert( cs.size() == 5 );
  assert( strcmp(cs.c_str(), "HELLO") == 0 );

  cs = "HELLO";
  assert( cs.size() == 5 );
  assert( strcmp(cs.c_str(), "HELLO") == 0 );

  assert( cs.find( L'H') == 0 );
  assert( cs.find( L'E') == 1 );
  assert( cs.find( "LL") == 2 );

  cs = std::string("HELLO");
  assert( cs.size() == 5 );
  assert( strcmp(cs.c_str(), "HELLO") == 0 );

  // functional tests
  csl_common_print_results( "empty_constr       ", csl_common_test_timer_v0(test_empty_constr),"" );
  csl_common_print_results( "opeq_pbuf          ", csl_common_test_timer_v0(test_opeq_pbuf),"" );
  csl_common_print_results( "cpyconstr          ", csl_common_test_timer_v0(test_cpyconstr),"" );
  csl_common_print_results( "cpy0               ", csl_common_test_timer_v0(test_cpy0),"" );
  csl_common_print_results( "cpyop              ", csl_common_test_timer_v0(test_cpyop),"" );
  csl_common_print_results( "pluseq             ", csl_common_test_timer_v0(test_pluseq),"" );
  csl_common_print_results( "find0              ", csl_common_test_timer_v0(test_find0),"" );
  csl_common_print_results( "substr0            ", csl_common_test_timer_v0(test_substr0),"" );

  // performance
  csl_common_print_results( "str_baseline       ", csl_common_test_timer_v0(str_baseline),"" );
  csl_common_print_results( "string_baseline    ", csl_common_test_timer_v0(string_baseline),"" );
  csl_common_print_results( "str_hello          ", csl_common_test_timer_v0(str_hello),"" );
  csl_common_print_results( "string_hello       ", csl_common_test_timer_v0(string_hello),"" );
  csl_common_print_results( "str_concat         ", csl_common_test_timer_v0(str_concat),"" );
  csl_common_print_results( "string_concat      ", csl_common_test_timer_v0(string_concat),"" );
  csl_common_print_results( "str_append         ", csl_common_test_timer_v0(str_append),"" );
  csl_common_print_results( "string_append      ", csl_common_test_timer_v0(string_append),"" );
  csl_common_print_results( "str_opeq           ", csl_common_test_timer_v0(str_opeq),"" );

  return 0;
}

/* EOF */
