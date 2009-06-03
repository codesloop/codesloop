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

#ifndef _csl_slt3_exc_hh_included_
#define _csl_slt3_exc_hh_included_

/**
   @file csl_slt3/src/exc.hh
   @brief common exception class for slt3
 */

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
    class exc
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

        enum {
          cm_unknown, ///<Unknown component
          cm_conn,    ///<conn component
          cm_tran,    ///<tran component
          cm_synqry,  ///<synqry component
          cm_param,   ///<param component
          cm_reg      ///<reg component
        };

        /** @brief converts reason code to string */
        static const char * reason_string(int rc);

        /** @brief converts component code to string */
        static const char * component_string(int cm);

        /** @brief converts exception to string */
        void to_string(common::str & res);

        /** @brief constructor 
            @param component that caused the exception */
        exc(int component)
        : reason_(rs_unknown), component_(component), line_(0) {}

        /** @brief constructor
            @param reason is to tell why
            @param component that cause the exception */
        exc(int reason, int component)
        : reason_(reason), component_(component), line_(0) {}

        /** @brief constructor
            @param reason is to tell why
            @param component that cause the exception
            @param txt provides some explanation */
        exc(int reason, int component, const wchar_t * txt)
         : reason_(reason), component_(component), text_(txt), line_(0) {}

        /** @brief constructor
            @param reason is to tell why
            @param component that cause the exception
            @param txt provides some explanation
            @param file tells which source file caused the error
            @param lin tells which line cause the error */
        exc(int reason, int component, const wchar_t * txt, const wchar_t * file, unsigned int line)
        : reason_(reason), component_(component), text_(txt), file_(file), line_(line) {}

        ~exc();

        int reason_;        ///<reason code: one of rs_*
        int component_;     ///<component code: one of cm_*
        common::str text_;  ///<error explanation
        common::str file_;  ///<error source file
        unsigned int line_; ///<error posintion in source file

      private:
        exc();
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_slt3_exc_hh_included_ */
