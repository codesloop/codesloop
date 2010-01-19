/*
Copyright (c) 2008,2009,2010, CodeSLoop Team

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
#include "codesloop/sched/item.hh"
#include "codesloop/sched/exc.hh"
#include "codesloop/common/mpool.hh"
#include "codesloop/common/common.h"
#include <memory>

/**
  @file item.cc
  @brief implementation of sched::item
 */

namespace csl
{
  namespace sched
  {
    using namespace csl::slt3;

    reg::helper item::reg_(SCHED_ITEM_DB_NAME,SCHED_ITEM_DB_DEFAULT_PATH);
    sql::helper item::sql_helper_(SCHED_ITEM_TABLE_NAME);

    /* copy */
    item & item::operator=(const item & x)
    {
      id_             = x.id_;
      schedule_id_    = x.schedule_id_;
      use_exc(x.use_exc());
      return *this;
    }

    item::item(const item & x)
      : id_(0,*this), schedule_id_(0,*this)
      {
        throw exc(exc::rs_private_fun,get_class_name());
      }

  } /*sched*/
} /*csl*/

/* EOF */
