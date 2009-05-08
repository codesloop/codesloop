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

#ifndef _csl_sched_head_hh_included_
#define _csl_sched_head_hh_included_

/**
   @file head.hh
   @brief head @todo head
 */

#include "conn.hh"
#include "reg.hh"
#include "obj.hh"
#include "var.hh"
#ifdef __cplusplus

#ifndef SCHED_HEAD_DB_NAME
#define SCHED_HEAD_DB_NAME "sched_head"
#endif /*SCHED_HEAD_DB_NAME*/

#ifndef SCHED_HEAD_TABLE_NAME
#define SCHED_HEAD_TABLE_NAME "head"
#endif /*SCHED_HEAD_TABLE_NAME*/

#ifndef SCHED_HEAD_DB_DEFAULT_PATH
#define SCHED_HEAD_DB_DEFAULT_PATH "head.db"
#endif /*SCHED_HEAD_DB_DEFAULT_PATH*/

namespace csl
{
  namespace sched
  {
    using slt3::conn;
    using slt3::reg;
    using slt3::sql;

    /** @todo document me */
    class head : public slt3::obj
    {
      public:
        inline head():
          /* database fields */
          id_(         "id",        *this,"PRIMARY KEY ASC AUTOINCREMENT"),
          item_id_(    "item_id",   *this,"NOT NULL"),
          name_(       "name",      *this,"NOT NULL"),
          value_(      "value",     *this,"NOT NULL")
        {}

        inline ~head() { }

        /* ORM mapper's stuff */
        static reg::helper reg_;

        static sql::helper  sql_helper_;
        virtual conn & db() { return reg_.db(); }
        virtual sql::helper & sql_helper() const { return sql_helper_; }

        /* copy */
        virtual head & operator=(const head & x);

        /* interface */

      private:
        /* private copy constructor */
        head(const head & x);

      public:
        /* db fields */
        slt3::intvar  id_;
        slt3::intvar  item_id_;
        slt3::strvar  name_;
        slt3::strvar  value_;
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_sched_head_hh_included_ */
