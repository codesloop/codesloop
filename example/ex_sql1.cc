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
  @file ex_sql1.cc
  @brief basic slt3 demonstration using exceptions
 */

#include "str.hh"
#include "csl_slt3.hh"
#include "common.h"

using namespace csl::slt3;
using namespace csl::common;

int main()
{
  conn c;

  try
  {
    c.open(L"testme.db");

    /* start a transaction */
    tran t(c);

    /* create a query object */
    synqry q(t);

    q.execute(L"create table test(i int);");
  }
  catch( exc e )
  {
    str s;
    e.to_string(s);

    FPRINTF(stderr,L"Exception caught: %sl\n",s.c_str());
  }

  return 0;
}

/* EOF */

