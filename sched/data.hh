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

#ifndef _csl_sched_data_hh_included_
#define _csl_sched_data_hh_included_

/**
   @file data.hh
   @brief data @todo data
 */

#include "conn.hh"
#include "reg.hh"
#include "obj.hh"
#include "var.hh"
#ifdef __cplusplus

#ifndef SCHED_DATA_DB_NAME
#define SCHED_DATA_DB_NAME "sched_data"
#endif /*SCHED_DATA_DB_NAME*/

#ifndef SCHED_DATA_TABLE_NAME
#define SCHED_DATA_TABLE_NAME "data"
#endif /*SCHED_DATA_TABLE_NAME*/

#ifndef SCHED_DATA_DB_DEFAULT_PATH
#define SCHED_DATA_DB_DEFAULT_PATH "data.db"
#endif /*SCHED_DATA_DB_DEFAULT_PATH*/

namespace csl
{
  namespace sched
  {
    using slt3::conn;
    using slt3::reg;
    using slt3::sql;

    /** @todo document me */
    class data : public slt3::obj
    {
      public:
        inline data():
          /* database fields */
          id_(         "id",        *this, "PRIMARY KEY ASC AUTOINCREMENT"),
          item_id_(    "item_id",   *this, "NOT NULL"),
          is_inline_(  "is_inline", *this, "NOT NULL DEFAULT(0)"),
          value_(      "value",     *this)
        {}

        inline ~data() { }

        /* ORM mapper's stuff */
        static reg::helper reg_;
        static sql::helper  sql_helper_;
        virtual conn & db() { return reg_.db(); }
        virtual sql::helper & sql_helper() const { return sql_helper_; }

        /* copy */
        virtual data & operator=(const data & x);

        /* interface */

      private:
        /* private copy constructor */
        data(const data & x);

      public:
        /* db fields */
        slt3::intvar    id_;
        slt3::intvar    item_id_;
        slt3::intvar    is_inline_;
        slt3::blobvar   value_;

        CSL_OBJ(csl::sched,cfg);
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_sched_data_hh_included_ */
