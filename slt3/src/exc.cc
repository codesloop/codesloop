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

#include "exc.hh"
#include "str.hh"
#include "common.h"

/**
  @file csl_slt3/src/exc.cc
  @brief implementation of slt3::exc
 */

namespace csl
{
  namespace slt3
  {
    const wchar_t * exc::reason_string(int rc)
    {
      switch( rc )
      {
        case rs_notopened:       return L"Not opened";
        case rs_nullparam:       return L"Param is null";
        case rs_nulltran:        return L"Tran is null";
        case rs_nullconn:        return L"Conn is null";
        case rs_nulldb:          return L"DB is null";
        case rs_nullstmnt:       return L"Statement is null";
        case rs_transaction_already_started: return L"Transaction already started";
        case rs_internal:        return L"Internal error";
        case rs_permission:      return L"Permission denied";
        case rs_abort:           return L"Abort";
        case rs_busy:            return L"Busy";
        case rs_locked:          return L"Locked";
        case rs_nomem:           return L"No mem";
        case rs_readonly:        return L"Readonly";
        case rs_ioerr:           return L"I/O error";
        case rs_corrupt:         return L"Corrupt database";
        case rs_notfound:        return L"Not found";
        case rs_cantopen:        return L"Cannot open";
        case rs_full:            return L"Full";
        case rs_protocol:        return L"Protocol error";
        case rs_empty:           return L"Empty";
        case rs_schema:          return L"Schema";
        case rs_toobig:          return L"Too big";
        case rs_constraint:      return L"Constraint violation";
        case rs_mismatch:        return L"Mismatch";
        case rs_misuse:          return L"Misuse";
        case rs_auth:            return L"Auth";
        case rs_format:          return L"Format";
        case rs_range:           return L"Range";
        case rs_notadb:          return L"Not a DB";
        case rs_cannot_reg:      return L"Cannot register database";
        case rs_unknown:
          default:               return L"Unknown reason";
      };
      return L"unkonwn reason";
    }
  };
};

/* EOF */
