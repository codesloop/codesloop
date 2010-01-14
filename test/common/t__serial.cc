/*
   Copyright (c) 2008,2009,2010, CodeSLoop Team

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
   @file t__obj.cc
   @brief Tests to verify serialization/deserialization routines
   @todo add more test case for each vars
 */

#ifndef DEBUG
#define DEBUG
#endif /* DEBUG */

#include "codesloop/common/common.h"
#include "codesloop/common/str.hh"
#include "exc.hh"
#include "codesloop/common/arch.hh"
#include "codesloop/common/logger.hh"
#include "codesloop/common/xdrbuf.hh"
#include "codesloop/common/pbuf.hh"
#include <assert.h>
#include <sys/stat.h>

using namespace csl::common;

int main()
{
  bool caught = false;

  try {
    str src = L"This is a test";
    str dst;
    pbuf * buf;

    // Class types - direct
    arch sar( arch::SERIALIZE );
    src.serialize( sar );

    buf = sar.get_pbuf();

    arch dar( arch::DESERIALIZE );
    dar.set_pbuf( (*buf) );
    dst.serialize( dar);

    assert( src == dst );

    // Elementary types
    unsigned int elementary = 0xDeadBabe;
    arch sar2( arch::SERIALIZE );
    sar2.serialize( elementary );
    buf = sar2.get_pbuf();

    elementary = 0;

    arch dar2( arch::DESERIALIZE );
    dar2.set_pbuf( (*buf) );
    dar2.serialize( elementary );

    assert( elementary == 0xDeadBabe );


  } catch ( exc e ) {
    FPRINTF(stderr,L"Exception caught: %ls\n",e.to_string().c_str());
    caught = true;
  }
  /* this should not throw an exception, will add as zero length string */
  assert( caught == false );
}

/* EOF */
