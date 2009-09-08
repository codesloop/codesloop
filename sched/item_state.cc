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
#include "codesloop/sched/item_state.hh"
#include "codesloop/sched/exc.hh"
#include "codesloop/common/mpool.hh"
#include "codesloop/common/common.h"
#include <memory>

/**
  @file item_state.cc
  @brief implementation of sched::item_state
 */

namespace csl
{
  namespace sched
  {
    using namespace csl::slt3;

    reg::helper item_state::reg_(SCHED_ITEM_STATE_DB_NAME,SCHED_ITEM_STATE_DB_DEFAULT_PATH);
    sql::helper item_state::sql_helper_(SCHED_ITEM_STATE_TABLE_NAME);

    /* copy */
    item_state & item_state::operator=(const item_state & x)
    {
      id_             = x.id_;
      item_id_        = x.item_id_;
      peer_id_        = x.peer_id_;
      peer_state_     = x.peer_state_;
      use_exc(x.use_exc());
      return *this;
    }

    item_state::item_state(const item_state & x)
      : id_(0,*this), item_id_(0,*this), peer_id_(0,*this), peer_state_(0,*this)
      {
        throw exc(exc::rs_private_fun,get_class_name());
      }

  } /*sched*/
} /*csl*/

/* EOF */
