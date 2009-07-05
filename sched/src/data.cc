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

#include "csl_slt3.hh"
#include "data.hh"
#include "exc.hh"
#include "mpool.hh"
#include "common.h"
#include <memory>

/**
  @file data.cc
  @brief implementation of sched::data
 */

namespace csl
{
  namespace sched
  {
    using namespace csl::slt3;

    reg::helper data::reg_(SCHED_DATA_DB_NAME,SCHED_DATA_DB_DEFAULT_PATH);
    sql::helper data::sql_helper_(SCHED_DATA_TABLE_NAME);

    /* copy */
    data & data::operator=(const data & x)
    {
      id_             = x.id_;
      item_id_        = x.item_id_;
      is_inline_      = x.is_inline_;
      value_          = x.value_;
      use_exc(x.use_exc());
      return *this;
    }

    data::data(const data & x)
      : id_(0,*this), item_id_(0,*this), is_inline_(0,*this), value_(0,*this)
      {
        throw exc(exc::rs_private_fun,get_class_name());
      }

  } /*sched*/
} /*csl*/

/* EOF */
