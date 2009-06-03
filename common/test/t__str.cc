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
   @file t__logger.cc
   @brief Tests to verify logger
 */

#define DEBUG 

#include "logger.hh"
#include "exc.hh"
#include "common.h"
#include "str.hh"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <string>
#include <sys/stat.h>

using csl::common::str;

int main()
{

  str s( L"HELLO" );
  assert( s.size() == 5 );
  assert( wcscmp(s.c_str(), L"HELLO") == 0 );

  assert( s[0] == L'H' );
  assert( s[1] == L'E' );

  str s2 = s;
  assert( s2.size() == 5 );
  assert( wcscmp(s2.c_str(), L"HELLO") == 0 );

  str s3(s);
  assert( s3.size() == 5 );
  assert( wcscmp(s3.c_str(), L"HELLO") == 0 );

  s2 += L" WORLD";
  assert( s2.size() == 11 );
  assert( wcscmp(s2.c_str(), L"HELLO WORLD") == 0 );

  s2 = s.substr(1,3) + s2.substr(6,999);
  assert( s2.size() == 8 );
  assert( wcscmp(s2.c_str(), L"ELLWORLD") == 0 );

  str cs( "HELLO" );
  assert( cs.size() == 5 );
  assert( wcscmp(cs.c_str(), L"HELLO") == 0 );

  cs = "HELLO";
  assert( cs.size() == 5 );
  assert( wcscmp(cs.c_str(), L"HELLO") == 0 );

  assert( cs.find( L'H') == 0 );
  assert( cs.find( L'E') == 1 );
  assert( cs.find( L"LL") == 2 );


  return 0;
}

/* EOF */
