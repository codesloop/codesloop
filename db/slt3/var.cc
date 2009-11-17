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

#include "codesloop/common/ustr.hh"
#include "codesloop/common/common.h"
#include "codesloop/db/slt3/obj.hh"
#include "codesloop/db/slt3/var.hh"
#include "codesloop/db/slt3/sql.hh"
#include "codesloop/db/slt3/conn.hh"
#include "codesloop/db/slt3/tran.hh"
#include "codesloop/db/slt3/query.hh"

/**
  @file var.cc
  @brief implementation of slt3::var
 */

using csl::common::str;
using csl::common::ustr;
using csl::common::pbuf;

namespace csl
{
  namespace db
  {
    namespace slt3
    {
      void var_base::helper::set_id(long long id)
      {
        datalist_t::iterator it(dtalst_.begin());
        datalist_t::iterator end(dtalst_.end());

        if( it != end )
        {
          data * dx = *it;

          query::colhead ch;
          common::int64 fd(id);

          ch.type_  = query::colhead::t_integer;
          dx->var_->set_value(&ch,&fd);
        }
      }

      /* 'CREATE TABLE IF NOT EXISTS Xtable ( id INTEGER PRIMARY KEY ASC AUTOINCREMENT ,
                                              name TEXT NOT NULL ,
                                              height REAL DEFAULT (0.1) ,
                                              pk BLOB  ); */
      bool var_base::helper::init(tran & t, const char * sql_query)
      {
        query q(t);
        if( q.prepare(sql_query) == false ) return false;
        q.next();
        return true;
      }

      /* 'INSERT INTO Xtable ( name,height,pk ) VALUES ( ?,?,? );' */
      bool var_base::helper::create(tran & t, const char * sql_query)
      {
        query q(t);

        datalist_t::iterator it(dtalst_.begin());
        datalist_t::iterator end(dtalst_.end());

        unsigned int i = 0;

        for( ;it!=end;++it )
        {
          /* skip the first parameter, assuming it is the primary key ... */
          if( i > 0 )
          {
            data * d = *it;
            q.set_param( static_cast<int>(i),*(d->var_->get_value()) );
          }
          ++i;
        }

        if( !q.prepare(sql_query) ) return false;

        q.next();

        if( q.change_count() > 0 )
        {
          datalist_t::iterator ix(dtalst_.begin());
          data * d = *ix;
          d->var_->parent()->set_id(q.last_insert_id());
          return true;
        }
        else
        {
          return false;
        }
      }

      /* 'UPDATE Xtable SET name=? , height=? , pk=?  WHERE id=? ;' */
      bool var_base::helper::save(tran & t, const char * sql_query)
      {
        query q(t);

        datalist_t::iterator it(dtalst_.begin());
        datalist_t::iterator end(dtalst_.end());

        unsigned int i = 0;

        for( ;it!=end;++it )
        {
          /* set parameters while skipping id */
          if( i > 0 )
          {
            data * d = *it;
            q.set_param( static_cast<int>(i),*(d->var_->get_value()) );
          }
          ++i;
        }

        /* set id at last */
        datalist_t::iterator ix(dtalst_.begin());
        data * dx = *ix;
        q.set_param( static_cast<int>(i),*(dx->var_->get_value()) );
        ++i;

        if( !q.prepare(sql_query) ) return false;

        q.next();

        /* check if succeed */
        if( q.change_count() > 0 ) return true;
        else                       return false;
      }

      /* 'DELETE FROM Xtable WHERE id=?;' */
      bool var_base::helper::remove(tran & t, const char * sql_query)
      {
        query q(t);

        datalist_t::iterator it(dtalst_.begin());
        data * dx = *it;
        q.set_param( 1,*(dx->var_->get_value()) );

        if( !q.prepare(sql_query) ) return false;

        q.next();

        /* check if succeed */
        if( q.change_count() > 0 ) return true;
        else                       return false;
      }

      /* 'SELECT id,name,height,pk FROM Xtable WHERE id=? LIMIT 1;' */
      bool var_base::helper::find_by_id(tran & t, const char * sql_query)
      {
        query q(t);

        /* set id */
        datalist_t::iterator ix(dtalst_.begin());
        data * dx = *ix;
        q.set_param( 1,*(dx->var_->get_value()) );

        if( !q.prepare(sql_query) ) return false;

        query::columns_t ch;
        query::fields_t  fd;

        q.next(ch,fd);

        if( ch.size() > 0 && fd.size() > 0 )
        {
          query::columns_t::iterator chit(ch.begin());
          query::columns_t::iterator chend(ch.end());

          query::fields_t::iterator fdit(fd.begin());
          query::fields_t::iterator fdend(fd.end());

          datalist_t::iterator dit(dtalst_.begin());
          datalist_t::iterator dend(dtalst_.end());

          while( chit!=chend && fdit!=fdend && dit!=dend )
          {
            ///
            data * d = *dit;
            if( !d->var_->set_value( *chit, *fdit ) ) return false;
            ++chit;
            ++fdit;
            ++dit;
          }
          return (chit==chend && fdit==fdend && dit == dend);
        }
        return false;
      }

      bool var_base::helper::find_by(tran & t,
                                const char * sql_query,
                                int field1,
                                int field2,
                                int field3,
                                int field4,
                                int field5)
      {
        int tmpi[5] = { field1, field2, field3, field4, field5 };

        query q(t);

        for( int k=0;k<5 && tmpi[k] != -1; ++k )
        {
          data * d = dtalst_.get_at(tmpi[k]);
          if( d )
          {
            q.set_param( k+1,*(d->var_->get_value()) );
          }
        }

        if( !q.prepare(sql_query) ) return false;

        query::columns_t ch;
        query::fields_t  fd;

        q.next(ch,fd);

        if( ch.size() > 0 && fd.size() > 0 )
        {
          query::columns_t::iterator chit(ch.begin());
          query::columns_t::iterator chend(ch.end());

          query::fields_t::iterator fdit(fd.begin());
          query::fields_t::iterator fdend(fd.end());

          datalist_t::iterator dit(dtalst_.begin());
          datalist_t::iterator dend(dtalst_.end());

          while( chit!=chend && fdit!=fdend && dit!=dend )
          {
            ///
            data * d = *dit;
            if( !d->var_->set_value( *chit, *fdit ) ) return false;
            ++chit;
            ++fdit;
            ++dit;
          }
          return (chit==chend && fdit==fdend && dit == dend);
        }
        return false;
      }

      bool var_base::helper::add_field(const char * name, var_base & v)
      {
        data * d = new data(name,v);
        dtalst_.push_back(d);
        return true;
      }

      const char * var_col_type<common::int64>::coltype_s = "INTEGER";
      const char * var_col_type<common::dbl>::coltype_s = "REAL";
      const char * var_col_type<common::ustr>::coltype_s = "TEXT";
      const char * var_col_type<common::binry>::coltype_s = "BLOB";

      void var_base::register_variable(var_base * vb, const char * namev, const char * coltype, slt3::obj & prnt, const char * flags)
      {
        sql::helper & h(prnt.sql_helper());
        var_base::helper & v(prnt.var_helper());
        h.add_field(namev,coltype,flags);
        v.add_field(namev,*vb);
      }
    }; /* end of slt3 namespace */
  }; /* end of db namespace */
}; /* end of csl namespace */

/* EOF */
