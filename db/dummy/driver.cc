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

// #if 0
#ifndef DEBUG
#define DEBUG
#define DEBUG_ENABLE_INDENT
#endif /* DEBUG */
// #endif

#include "codesloop/common/logger.hh"
#include "codesloop/common/common.h"
#include "codesloop/db/dummy/driver.hh"

namespace csl
{
  namespace db
  {
    namespace dummy
    {
      /* static */ driver * driver::instance()
      {
        return new driver();
      }

      // connection related
      bool driver::open(const ustr & connect_string) { return false; }
      bool driver::close() { return false; }

      // transactions
      bool driver::begin(ustr & id) { return false; }
      bool driver::commit(const ustr & id) { return false; }
      bool driver::rollback(const ustr & id) { return false; }

      // subtransactions
      bool driver::savepoint(ustr & id) { return false; }
      bool driver::release_savepoint(const ustr & id) { return false; }
      bool driver::rollback_savepoint(const ustr & id) { return false; }

      // infos
      uint64_t driver::last_insert_id() { return 0; }
      uint64_t driver::change_count() { return 0; }
      void driver::reset_change_count() { }

      driver::driver() { }
      driver::~driver() { }
    } // end of ns:csl::db::dummy
  } // end of ns:csl::db
} // end of ns:csl

/* EOF */
