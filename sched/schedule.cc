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

#include "codesloop/db/csl_slt3.hh"
#include "codesloop/sched/schedule.hh"
#include "codesloop/sched/exc.hh"
#include "codesloop/common/mpool.hh"
#include "codesloop/common/common.h"
#include <memory>

/**
  @file schedule.cc
  @brief implementation of sched::schedule
 */

namespace csl
{
  namespace sched
  {
    using namespace csl::slt3;

    reg::helper schedule::reg_(SCHED_SCHEDULE_DB_NAME,SCHED_SCHEDULE_DB_DEFAULT_PATH);
    sql::helper schedule::sql_helper_(SCHED_SCHEDULE_TABLE_NAME);

    /* copy */
    schedule & schedule::operator=(const schedule & x)
    {
      id_             = x.id_;
      common_name_    = x.common_name_;
      owner_peer_id_  = x.owner_peer_id_;
      use_exc(x.use_exc());
      return *this;
    }

    schedule::schedule(const schedule & x)
      : id_(0,*this), common_name_(0,*this), owner_peer_id_(0,*this)
    {
      throw exc(exc::rs_private_fun,get_class_name());
    }

  } /*sched*/
} /*csl*/

/* EOF */
