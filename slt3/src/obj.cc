/*
Copyright (c) 2008,2009, David Beck, Tamas Foldi

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

#include "obj.hh"

/**
  @file obj.cc
  @brief implementation of slt3::obj
 */

namespace csl
{
  namespace slt3
  {
    bool obj::init(tran & t)
    {
      return var_helper().init(t,sql_helper().init_sql());
    }

    bool obj::create(tran & t)
    {
      return var_helper().create(t,sql_helper().create_sql());
    }

    bool obj::save(tran & t)
    {
      return var_helper().save(t,sql_helper().save_sql());
    }

    bool obj::remove(tran & t)
    {
      return var_helper().remove(t,sql_helper().remove_sql());
    }

    bool obj::find_by_id(tran & t)
    {
      bool r = var_helper().find_by_id(t,sql_helper().find_by_id_sql());
      if( r ) this->on_load();
      return r;
    }

    bool obj::find_by(tran & t,
                      int field1,
                      int field2,
                      int field3,
                      int field4,
                      int field5)
    {
      bool r =
          var_helper().find_by(
                     t,
                     sql_helper().find_by(field1, field2, field3, field4, field5),
                     field1, field2, field3, field4, field5);
      if( r ) this->on_load();
      return r;
    }

    bool obj::init()
    {
      conn & dbr(db());
      tran t(dbr);
      t.use_exc(true);
      bool ret = init(t);
      return ret;
    }

    bool obj::create()
    {
      conn & dbr(db());
      tran t(dbr);
      t.use_exc(true);
      bool ret = create(t);
      return ret;
    }

    bool obj::save()
    {
      conn & dbr(db());
      tran t(dbr);
      t.use_exc(true);
      bool ret = save(t);
      return ret;
    }

    bool obj::remove()
    {
      conn & dbr(db());
      tran t(dbr);
      t.use_exc(true);
      bool ret = remove(t);
      return ret;
    }

    bool obj::find_by_id()
    {
      conn & dbr(db());
      tran t(dbr);
      t.use_exc(true);
      bool ret = find_by_id(t);
      if( ret ) this->on_load();
      return ret;
    }

    bool obj::find_by(int field1,
                      int field2,
                      int field3,
                      int field4,
                      int field5)
    {
      conn & dbr(db());
      tran t(dbr);
      t.use_exc(true);
      bool ret = find_by(field1,field2,field3,field4,field5);
      if( ret ) this->on_load();
      return ret;
    }

    void obj::set_id(long long id)
    {
      var_helper().set_id(id);
    }
  };
};

/* EOF */
