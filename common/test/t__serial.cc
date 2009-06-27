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
   @file t__obj.cc
   @brief Tests to verify serialization/deserialization routines
   @todo add more test case for each vars
 */

#define DEBUG 

#include "common.h"
#include "str.hh"
#include "exc.hh"
#include "arch.hh"
#include "logger.hh"
#include "xdrbuf.hh"
#include "pbuf.hh"
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

    arch sar( arch::SERIALIZE );
    src.serialize( sar );

    buf = sar.get_pbuf();

    arch dar( arch::DESERIALIZE );
    dar.set_pbuf( (*buf) );
    dst.serialize( dar);

//    FPRINTF(stderr,L"src:%ls, dst:%ls\n", src.c_str(), dst.c_str() );
//    FPRINTF(stderr,L"sar:%ld, dar:%ld\n", sar.size(), dar.size() );

    assert( src == dst );

  } catch ( exc e ) {
    FPRINTF(stderr,L"Exception caught: %ls\n",e.to_string().c_str());
    caught = true;
  }
  /* this should not throw an exception, will add as zero length string */
  assert( caught == false );
}

/* EOF */
