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
    const char * exc::reason_string(int rc)
    {
      switch( rc )
      {
        case rs_notopened:       return "Not opened";
        case rs_nullparam:       return "Param is null";
        case rs_nulltran:        return "Tran is null";
        case rs_nullconn:        return "Conn is null";
        case rs_nulldb:          return "DB is null";
        case rs_nullstmnt:       return "Statement is null";
        case rs_transaction_already_started: return "Transaction already started";
        case rs_internal:        return "Internal error";
        case rs_permission:      return "Permission denied";
        case rs_abort:           return "Abort";
        case rs_busy:            return "Busy";
        case rs_locked:          return "Locked";
        case rs_nomem:           return "No mem";
        case rs_readonly:        return "Readonly";
        case rs_ioerr:           return "I/O error";
        case rs_corrupt:         return "Corrupt database";
        case rs_notfound:        return "Not found";
        case rs_cantopen:        return "Cannot open";
        case rs_full:            return "Full";
        case rs_protocol:        return "Protocol error";
        case rs_empty:           return "Empty";
        case rs_schema:          return "Schema";
        case rs_toobig:          return "Too big";
        case rs_constraint:      return "Constraint violation";
        case rs_mismatch:        return "Mismatch";
        case rs_misuse:          return "Misuse";
        case rs_auth:            return "Auth";
        case rs_format:          return "Format";
        case rs_range:           return "Range";
        case rs_notadb:          return "Not a DB";
        case rs_cannot_reg:      return "Cannot register database";
        case rs_unknown:
          default:               return "Unknown reason";
      };
      return "unkonwn reason";
    }

    const char * exc::component_string(int cm)
    {
      switch( cm )
      {
        case cm_conn:      return "slt3::conn";
        case cm_tran:      return "slt3::tran";
        case cm_synqry:    return "slt3::synqry";
        case cm_param:     return "slt3::param";
        case cm_reg:       return "slt3::reg";
        case cm_unknown:
        default:           return "unknown";
      };
      return "unknown component";
    }

    void exc::to_string(common::str & res)
    {
      common::str t("Exception");
      if( file_.size() > 0 && line_ > 0 )
      {
        char tx[200];
        SNPRINTF(tx,199,"(%s:%d): ",file_.c_str(),line_);
        t += tx;
      }
      t += " [";
      t += component_string(component_);
      t += "] [";
      t += reason_string(reason_);
      t += "] ";
      if( text_.size() > 0 ) t+= text_;
      res.swap(t);
    }

    exc::exc() : reason_(rs_unknown), component_(cm_unknown) {}
    exc::~exc() {}
    /* public interface */
  };
};

/* EOF */
