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

#ifndef _csl_sched_schedule_hh_included_
#define _csl_sched_schedule_hh_included_

/**
   @file schedule.hh
   @brief schedule @todo schedule
 */

#include "conn.hh"
#include "reg.hh"
#include "obj.hh"
#include "var.hh"
#include "item.hh"
#ifdef __cplusplus

#ifndef SCHED_SCHEDULE_DB_NAME
#define SCHED_SCHEDULE_DB_NAME "sched_schedule"
#endif /*SCHED_SCHEDULE_DB_NAME*/

#ifndef SCHED_SCHEDULE_TABLE_NAME
#define SCHED_SCHEDULE_TABLE_NAME "schedules"
#endif /*SCHED_SCHEDULE_TABLE_NAME*/

#ifndef SCHED_SCHEDULE_DB_DEFAULT_PATH
#define SCHED_SCHEDULE_DB_DEFAULT_PATH "sched.db"
#endif /*SCHED_SCHEDULE_DB_DEFAULT_PATH*/

namespace csl
{
  namespace sched
  {
    using slt3::conn;
    using slt3::reg;
    using slt3::sql;

    /** @todo document me */
    class schedule : public slt3::obj
    {
      public:
        inline schedule() :
              /* database fields */
              id_(             "id",           *this,"PRIMARY KEY ASC AUTOINCREMENT"),
              common_name_(    "common_name",  *this,"UNIQUE NOT NULL"),
              owner_peer_id_(  "owner_peer_id",*this,"NOT NULL")
        {}

        inline ~schedule() { }

        /* ORM mapper's stuff */
        static reg::helper reg_;
        static sql::helper  sql_helper_;
        virtual conn & db() { return reg_.db(); }
        virtual sql::helper & sql_helper() const { return sql_helper_; }

        /* copy */
        schedule & operator=(const schedule & x);

        /* interface */

      private:
        /* private copy constructor */
        schedule(const schedule & x);

      public:
        /* db fields */
        slt3::intvar    id_;
        slt3::strvar    common_name_;
        slt3::intvar    owner_peer_id_;
    };
  }
}

#endif /*__cplusplus*/
#endif /*_csl_sched_schedule_hh_included_*/
