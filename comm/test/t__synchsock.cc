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
   @file t__synchsock.cc
   @brief Tests to verify synchsock routines
 */

#include "thread.hh"
#include "synchsock.hh"
#include "test_timer.h"
#include "exc.hh"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

using namespace csl::nthread;
using namespace csl::comm;

/** @brief contains tests related to comm interface */
namespace test_synchsock {

  void print_hex(const char * prefix,const void * vp,size_t len)
  {
    unsigned char * hx = (unsigned char *)vp;
    printf("%s: ",prefix);
    for(size_t i=0;i<len;++i) printf("%.2X",hx[i]);
    printf("\n");
  }

  void baseline()
  {
    synchsock s;
    s.init(1);
  }

  class entry : public thread::callback
  {
    public:
      virtual void operator()(void)
      {
        while( stop_me_ == false )
        {
          assert( s_->wait_read(10000) == false );
        }
      }

      virtual ~entry() {}

      entry() : s_(0), stop_me_(false) {}

      synchsock * s_;
      bool stop_me_;
  };

  synchsock * s__  = NULL;

  void wait_write()
  {
    assert( s__->wait_write(20000) == true );
  }

} // end of test_synchsock

using namespace test_synchsock;

int main()
{
  try
  {
    synchsock s;
    s.init(1);
    thread t;
    entry e;
    e.s_ = &s;
    t.set_entry(e);
    s__ = &s;
    t.start();
    t.start_event().wait(0);

    csl_common_print_results( "baseline      ", csl_common_test_timer_v0(baseline),"" );
    csl_common_print_results( "wait_write    ", csl_common_test_timer_v0(wait_write),"" );

  }
  catch( csl::comm::exc & e )
  {
    std::string s;
    e.to_string(s);
    printf("Exception: %s\n",s.c_str());
  }

  return 0;
}

/* EOF */
