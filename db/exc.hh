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

#ifndef _csl_slt3_exc_hh_included_
#define _csl_slt3_exc_hh_included_

/**
   @file slt3/src/exc.hh
   @brief common exception class for slt3
 */

#include "cexc.hh"
#include "str.hh"
#ifdef __cplusplus

namespace csl
{
  namespace slt3
  {
    /**
    @brief common exception class used by slt3 classes

    this class is used by the slt3 class as an exception to be thrown
    */
    class exc : public csl::common::cexc
    {
      public:
        enum {
          rs_unknown,      ///<Unknown error.
          rs_notopened,    ///<Database not opened.
          rs_nullparam,    ///<Null parameter given.
          rs_nulltran,     ///<Transaction is null.
          rs_nullconn,     ///<Connection is null.
          rs_nulldb,       ///<DB handle is null.
          rs_nullstmnt,    ///<Statement handle is null.
          rs_transaction_already_started, ///<Transaction already aborted.
          rs_internal,     ///<Internal error.
          rs_permission,   ///<Access permission denied. Cannot read or write to the database file.
          rs_abort,        ///<A callback routine caused the transaction to abort.
          rs_busy,         ///<The database file locked.
          rs_locked,       ///<Table locked.
          rs_nomem,        ///<Malloc failed.
          rs_readonly,     ///<Readonly database.
          rs_ioerr,        ///<Input/Output error.
          rs_corrupt,      ///<Corrupt database.
          rs_notfound,     ///<Not found.
          rs_cantopen,     ///<Cannot open DB.
          rs_full,         ///<Database full.
          rs_protocol,     ///<Database locked or lock protocol error.
          rs_empty,        ///<The database table is empty.
          rs_schema,       ///<Schema changed.
          rs_toobig,       ///<Too big.
          rs_constraint,   ///<Constraint violation.
          rs_mismatch,     ///<Data type mismatch.
          rs_misuse,       ///<Library misuse.
          rs_auth,         ///<Authorization error.
          rs_format,       ///<Format error.
          rs_range,        ///<Range error.
          rs_notadb,       ///<Not a database.
          rs_cannot_reg    ///<Cannot register database
        };

        /** @brief converts reason code to string */
        static const wchar_t * reason_string(int rc);

        /** @brief constructor 
         *   @param component that caused the exception
         */
        exc(const wchar_t * component)
        : csl::common::cexc(component) {}

        /** @brief constructor
         *   @param reason is to tell why
         *   @param component that cause the exception
         */
        exc(int reason, const wchar_t * component)
        : csl::common::cexc(reason,component) {}

        /** @brief constructor
         *   @param reason is to tell why
         *   @param component that cause the exception
         *   @param txt provides some explanation
         */
        exc(int reason, const wchar_t * component, const wchar_t * txt)
        : csl::common::cexc(reason,component,txt) {}

        /** @brief constructor
         *   @param reason is to tell why
         *   @param component that cause the exception
         *   @param txt provides some explanation
         *   @param file tells which source file caused the error
         *   @param lin tells which line cause the error
         */
        exc(int reason, const wchar_t * component, const wchar_t * txt, const wchar_t * file, unsigned int line)
        : csl::common::cexc(reason,component,txt,file,line) {}
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_slt3_exc_hh_included_ */
