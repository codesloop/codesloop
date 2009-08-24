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
   @file t__qpid_sess.cc
   @brief Tests to verify qpid session management
 */

#define DEBUG 

#include "common.h"
#include "obj.hh"
#include "qpid_sess.hh"
#include <assert.h>
#include <sys/stat.h>

using namespace csl::common;
using namespace csl::mq;

int main()
{
  qpid_sess s;
  s.connect("qpid://localhost:5672");

  s.add_q("qpid_sess.q1");
  s.add_q("qpid_sess.q2");
  s.add_xchg("qpid_sess.xchg");

  // xchg based route
  s.add_route("qpid_sess.xchg", "qpid_sess.q1", "route1");
  s.add_route("qpid_sess.xchg", "qpid_sess.q2", "route2");

  // use direct transport 
  s.add_route("amq.direct", "qpid_sess.q1", "route1");
  s.add_route("amq.direct", "qpid_sess.q2", "route2");


  s.disconnect();
}

/* EOF */
