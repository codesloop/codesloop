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
   @file t__str.cc
   @brief Tests to verify wide character string
 */

#ifndef DEBUG
#define DEBUG
#endif /* DEBUG */

#include "logger.hh"
#include "exc.hh"
#include "common.h"
#include "str.hh"
#include "ustr.hh"
#include "int64.hh"
#include "dbl.hh"
#include "binry.hh"
#include "pbuf.hh"
#include "xdrbuf.hh"
#include "test_timer.h"
#include <assert.h>
#include <sys/stat.h>

using namespace csl::common;

/** @brief contains tests related to str */
namespace test_str {

  /** @test baseline for performance comparison */
  void string_baseline()
  {
    std::string b;
  }

  /** @test baseline for performance comparison */
  inline void str_baseline()
  {
    str b;
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
    str b;
    b = L"Hello";
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
    str b;
    b = L"Hello" + str(L"world");
    assert( b.size() == 10 );
    assert( b.nchars() == 10 );
    assert( b.nbytes() == 11*sizeof(wchar_t) );
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
    str b;
    b += L"Hello";
    b += L" ";
    b += L"world!";
    assert( b.size() == 12 );
    assert( b.nchars() == 12 );
    assert( b.nbytes() == 13*sizeof(wchar_t) );
  }

  /** @test @todo */
  void str_opeq()
  {
    str b("Hello world");
    assert( b.size() == 11 );
    assert( b.empty() == false );
    assert( b.c_str() != 0 );
    assert( b.nchars() == 11 );
    assert( b.nbytes() == 12*sizeof(wchar_t) );
  }

  void test_empty_constr()
  {
    str s;
    assert( s.nbytes() == sizeof(wchar_t) );
    assert( s.size() == 0 );
    assert( s.nchars() == 0 );
  }

  void test_opeq_pbuf()
  {
    str s;
    pbuf pb;
    pb.append(L"Hello");
    s = pb;
    assert( pb.size() == 6*sizeof(wchar_t) );
    assert( s.size() == 5 );
    assert( s.nbytes() == 6*sizeof(wchar_t) );
    assert( s.nchars() == 5 );
    assert( s == L"Hello" );
    assert( memcmp( s.data(),L"Hello",6*sizeof(wchar_t) ) == 0 );
  }

  void test_cpyconstr()
  {
    str s1(L"Hello");

    assert( s1.size() == 5 );
    assert( s1.nbytes() == 6*sizeof(wchar_t) );
    assert( s1.nchars() == 5 );

    str s2(s1);

    assert( s2.size() == 5 );
    assert( s2.nbytes() == 6*sizeof(wchar_t) );
    assert( s2.nchars() == 5 );

    assert( s2 == L"Hello" );
    assert( memcmp( s2.data(),L"Hello",6*sizeof(wchar_t) ) == 0 );
  }

  void test_cpy0()
  {
    str s1(L"Hello");
  }

  void test_cpyop()
  {
    str s;
    s = L"Hello";
    assert( s.size() == 5 );
    assert( s.nbytes() == 6*sizeof(wchar_t) );
    assert( s.nchars() == 5 );

    str s2;
    s2 = s;

    assert( s2.size() == 5 );
    assert( s2.nbytes() == 6*sizeof(wchar_t) );
    assert( s2.nchars() == 5 );
  }

  void test_pluseq()
  {
    str s;
    s += L"Hello";

    assert( s.size() == 5 );
    assert( s.nbytes() == 6*sizeof(wchar_t) );
    assert( s.nchars() == 5 );

    s += L" world";

    assert( s.size() == 11 );
    assert( s.nbytes() == 12*sizeof(wchar_t) );
    assert( s.nchars() == 11 );

    assert( s == L"Hello world" );
    assert( memcmp( s.data(),L"Hello world",12*sizeof(wchar_t) ) == 0 );
  }

  void test_find0()
  {
    str s(L"Hello World");
    assert( s.find(L"l") == 2 );
    assert( s.find('l') == 2 );
    assert( s.rfind('l') == 9 );
  }

  void test_substr0()
  {
    str s(L"Hello World");
    str s2 = s.substr(2,4);
    assert( s2 == L"llo " );
  }

  void test_trim0()
  {
    str s(L" hello world ");
    assert( s.trim()  == L"hello world" );
    str s2(L"hello world");
    assert( s2.trim()  == L"hello world" );
  }

  void to_integer_o()
  {
    str b(L"-111000999");
    int64 o;
    assert( b.to_integer(o) == true );
    assert( o.value() == -111000999LL );
  }

  void to_integer_l()
  {
    str b("-111000999");
    long long o;
    assert( b.to_integer(o) == true );
    assert( o == -111000999LL );
  }

  void to_double_o()
  {
    str b(L"1234.9876");
    dbl o;
    assert( b.to_double(o) == true );
    assert( o.value() == 1234.9876 );
  }

  void to_double_d()
  {
    str b("-1234.9876");
    dbl o;
    assert( b.to_double(o) == true );
    assert( o.value() == -1234.9876 );
  }

  void to_string_so()
  {
    str b("árvíztűrő tükörfúrógép ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP");
    b.use_exc(true);
    str o;
    o.use_exc(true);
    assert( b.to_string(o) == true );
    assert( b == o );
    assert( o == L"árvíztűrő tükörfúrógép ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP" );
  }

  void to_string_su()
  {
    str b(L"árvíztűrő tükörfúrógép ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP");
    b.use_exc(true);
    ustr o;
    o.use_exc(true);
    assert( b.to_string(o) == true );
    assert( o == "árvíztűrő tükörfúrógép ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP" );
    assert( b == o );
  }

  void to_string_ss()
  {
    str b(L"árvíztűrő tükörfúrógép ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP");
    b.use_exc(true);
    std::string o;
    assert( b.to_string(o) == true );
    assert( o == "árvíztűrő tükörfúrógép ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP" );
    assert( b == o.c_str() );
  }

  void to_binary_o()
  {
    str b("árvíztűrő tükörfúrógép ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP");
    b.use_exc(true);
    binry o;
    assert( b.to_binary(o) == true );
    assert( b == reinterpret_cast<const wchar_t *>(o.value().data()) );
  }

  void to_binary_u()
  {
    str b("árvíztűrő tükörfúrógép ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP");
    b.use_exc(true);
    unsigned char o[300];
    size_t sz = 0;
    assert( b.to_binary(o,sz) == true );
    assert( sz == b.nbytes() );
    assert( sz > 10 );
    assert( b == reinterpret_cast<wchar_t *>(o) );
  }

  void to_binary_v()
  {
    str b("árvíztűrő tükörfúrógép ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP");
    b.use_exc(true);
    unsigned char o[300];
    void * vp = o;
    size_t sz = 0;
    assert( b.to_binary(vp,sz) == true );
    assert( sz == b.nbytes() );
    assert( sz > 10 );
    assert( b == reinterpret_cast<wchar_t *>(o) );
  }

  void to_xdr()
  {
    str b;
    // TODO
  }

  void to_var()
  {
    str b;
    // TODO
  }

  void from_integer_o()
  {
    str b;
    int64 o;
    assert( o.from_string("12984788") == true );
    assert( b.from_integer(o) == true );
    assert( b == "12984788" );
  }

  void from_integer_l()
  {
    str b;
    long long o = 12984788;
    assert( b.from_integer(o) == true );
    assert( b == "12984788" );
  }

  void from_double_o()
  {
    str b;
    dbl o;
    assert( o.from_string(L"-12984.78800") == true );
    assert( b.from_double(o) == true );
    assert( b == "-12984.788000000000" );
  }

  void from_double_d()
  {
    str b;
    double o = 12984.788;
    assert( b.from_double(o) == true );
    assert( b == "12984.788000000000" );
  }

  void from_string_so()
  {
    str b;
    str o;
    assert( o.from_double(111.222333444) == true );
    assert( b.from_string(o) == true );
    assert( b == "111.222333444000" );
  }

  void from_string_uo()
  {
    str b;
    ustr o;
    assert( o.from_double(111.222333444) == true );
    assert( b.from_string(o) == true );
    assert( b == "111.222333444000" );
  }

  void from_string_ss()
  {
    str b;
    std::string o("-111.222333444");
    assert( b.from_string(o) == true );
    assert( b == "-111.222333444" );
  }

  void from_string_sc()
  {
    str b;
    b.use_exc(true);
    assert( b.from_string("árvíztűrő tükörfúrógép ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP") == true );
    assert( b == "árvíztűrő tükörfúrógép ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP" );
  }

  void from_string_sw()
  {
    str b;
    b.use_exc(true);
    assert( b.from_string(L"árvíztűrő tükörfúrógép ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP") == true );
    assert( b == "árvíztűrő tükörfúrógép ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP" );
  }

  void from_binary_o()
  {
    str b;
    b.use_exc(true);
    binry o;
    o.use_exc(true);
    assert( o.from_string(L"árvíztűrő tükörfúrógép ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP") == true );
    assert( b.from_binary(o) == true );
    assert( b == "árvíztűrő tükörfúrógép ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP" );
  }

  void from_binary_u()
  {
    str b;
    b.use_exc(true);
    str o(L"árvíztűrő tükörfúrógép ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP");
    o.use_exc(true);
    assert( b.from_binary( o.buffer().data(), o.buffer().size() ) == true );
    assert( b == "árvíztűrő tükörfúrógép ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP" );
  }

  void from_binary_v()
  {
    str b;
    b.use_exc(true);
    str o(L"árvíztűrő tükörfúrógép ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP");
    o.use_exc(true);
    assert( b.from_binary( reinterpret_cast<const void *>(o.buffer().data()), o.buffer().size() ) == true );
    assert( b == "árvíztűrő tükörfúrógép ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP" );
  }

  void from_xdr()
  {
    str b;
    // TODO
  }

  void from_var()
  {
    str b;
    // TODO
  }


} // namespace test_str


using namespace test_str;

int main()
{
  const char * strs[] = { "LC_ALL", "LANG", "LC_CTYPE", "LC_MESSAGES", 0 };
  const char ** strp = strs;

  while( *strp )
  {
    if( getenv(*strp) == 0 )
    {
      fprintf(stderr,"Warning '%s' has not been set\n",*strp);
    }
    ++strp;
  }

  if (!setlocale(LC_CTYPE, ""))
  {
    fprintf(stderr, "Can't set the specified locale! Check LANG, LC_CTYPE, LC_ALL.\n");
    exit(-1);
  }

  str s( L"HELLO" );
  s.use_exc(true);

  assert( s.nbytes() == 6*sizeof(wchar_t) );
  assert( s.empty() == false );
  assert( s.size() == 5 );
  assert( wcscmp(s.c_str(), L"HELLO") == 0 );

  assert( s[0] == L'H' );
  assert( s[1] == L'E' );

  str s2 = s;
  s2.use_exc(true);

  assert( s2.size() == 5 );
  assert( wcscmp(s2.c_str(), L"HELLO") == 0 );

  str s3(s);
  s3.use_exc(true);

  assert( s3.size() == 5 );
  assert( wcscmp(s3.c_str(), L"HELLO") == 0 );

  s2 += L" WORLD";
  assert( s2.size() == 11 );
  assert( wcscmp(s2.c_str(), L"HELLO WORLD") == 0 );

  s2 = s.substr(1,3) + s2.substr(6,999);
  assert( s2.size() == 8 );
  assert( wcscmp(s2.c_str(), L"ELLWORLD") == 0 );

  str cs( "HELLO" );
  cs.use_exc(true);

  assert( cs.size() == 5 );
  assert( wcscmp(cs.c_str(), L"HELLO") == 0 );

  cs = "HELLO";
  assert( cs.size() == 5 );
  assert( wcscmp(cs.c_str(), L"HELLO") == 0 );

  assert( cs.find( L'H') == 0 );
  assert( cs.find( L'E') == 1 );
  assert( cs.find( L"LL") == 2 );

  cs = std::string("HELLO");
  assert( cs.size() == 5 );
  assert( wcscmp(cs.c_str(), L"HELLO") == 0 );

  cs = L"Árvíztűrő tükörfúrógép";
  assert( cs.size() == 22 );
  assert( cs.buffer().size() == sizeof(wchar_t) * (cs.size() + 1) );
  assert( wcscmp(cs.c_str(), L"Árvíztűrő tükörfúrógép" ) == 0 );

  /* conversions */
  csl_common_print_results( "to_integer_o     ", csl_common_test_timer_v0(to_integer_o),"" );
  csl_common_print_results( "to_integer_l     ", csl_common_test_timer_v0(to_integer_l),"" );
  csl_common_print_results( "to_double_o      ", csl_common_test_timer_v0(to_double_o),"" );
  csl_common_print_results( "to_double_d      ", csl_common_test_timer_v0(to_double_d),"" );
  csl_common_print_results( "to_string_so     ", csl_common_test_timer_v0(to_string_so),"" );
  csl_common_print_results( "to_string_su     ", csl_common_test_timer_v0(to_string_su),"" );
  csl_common_print_results( "to_string_ss     ", csl_common_test_timer_v0(to_string_ss),"" );
  csl_common_print_results( "to_binary_o      ", csl_common_test_timer_v0(to_binary_o),"" );
  csl_common_print_results( "to_binary_u      ", csl_common_test_timer_v0(to_binary_u),"" );
  csl_common_print_results( "to_binary_v      ", csl_common_test_timer_v0(to_binary_v),"" );
  csl_common_print_results( "to_xdr           ", csl_common_test_timer_v0(to_xdr),"" );
  csl_common_print_results( "to_var           ", csl_common_test_timer_v0(to_var),"" );
  csl_common_print_results( "from_integer_o   ", csl_common_test_timer_v0(from_integer_o),"" );
  csl_common_print_results( "from_integer_l   ", csl_common_test_timer_v0(from_integer_l),"" );
  csl_common_print_results( "from_double_o    ", csl_common_test_timer_v0(from_double_o),"" );
  csl_common_print_results( "from_double_d    ", csl_common_test_timer_v0(from_double_d),"" );
  csl_common_print_results( "from_string_so   ", csl_common_test_timer_v0(from_string_so),"" );
  csl_common_print_results( "from_string_uo   ", csl_common_test_timer_v0(from_string_uo),"" );
  csl_common_print_results( "from_string_ss   ", csl_common_test_timer_v0(from_string_ss),"" );
  csl_common_print_results( "from_string_sc   ", csl_common_test_timer_v0(from_string_sc),"" );
  csl_common_print_results( "from_string_sw   ", csl_common_test_timer_v0(from_string_sw),"" );
  csl_common_print_results( "from_binary_o    ", csl_common_test_timer_v0(from_binary_o),"" );
  csl_common_print_results( "from_binary_u    ", csl_common_test_timer_v0(from_binary_u),"" );
  csl_common_print_results( "from_binary_v    ", csl_common_test_timer_v0(from_binary_v),"" );
  csl_common_print_results( "from_xdr         ", csl_common_test_timer_v0(from_xdr),"" );
  csl_common_print_results( "from_var         ", csl_common_test_timer_v0(from_var),"" );

  /* functional tests */
  csl_common_print_results( "empty_constr       ", csl_common_test_timer_v0(test_empty_constr),"" );
  csl_common_print_results( "opeq_pbuf          ", csl_common_test_timer_v0(test_opeq_pbuf),"" );
  csl_common_print_results( "cpyconstr          ", csl_common_test_timer_v0(test_cpyconstr),"" );
  csl_common_print_results( "cpy0               ", csl_common_test_timer_v0(test_cpy0),"" );
  csl_common_print_results( "cpyop              ", csl_common_test_timer_v0(test_cpyop),"" );
  csl_common_print_results( "pluseq             ", csl_common_test_timer_v0(test_pluseq),"" );
  csl_common_print_results( "find0              ", csl_common_test_timer_v0(test_find0),"" );
  csl_common_print_results( "substr0            ", csl_common_test_timer_v0(test_substr0),"" );
  csl_common_print_results( "trim0              ", csl_common_test_timer_v0(test_trim0),"" );

  /* performance */
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
