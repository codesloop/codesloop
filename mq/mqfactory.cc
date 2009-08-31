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
THEORY OF LIABILITY, WHETHER IN CONTRACT, objICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/**
   @file qpid_msg.cc
   @brief impl_ interface for qpid based queueing
 */

#include "msg.hh"
#include "sess.hh"
#include "qpid_msg.hh"
#include "qpid_sess.hh"
#include "mqfactory.hh"
#include "logger.hh"

#include <qpid/client/Connection.h>
#include <qpid/client/Session.h>


using namespace qpid::client;
using namespace qpid::framing;

namespace csl 
{
  namespace mq 
  {

      msg * mqfactory::build_msg(char * provider)
      {
        return new qpid_msg;
      }

      sess * mqfactory::build_sess(char * provider)
      {
        sess * ret = new qpid_sess;
        return ret;
      }


      void mqfactory::free_msg(msg *& p)
      {
        if ( p )
          free(p);
        p = NULL;
      }

      void mqfactory::free_sess(sess *& p )
      {
        if ( p )
          free(p);
        p = NULL;
      }

  }
}


