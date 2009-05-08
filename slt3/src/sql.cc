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

#include "pbuf.hh"
#include "sql.hh"
#include <string.h>

/**
  @file sqhelpr.cc
  @brief implementation of slt3::sql
 */

namespace csl
{
  namespace slt3
  {
    namespace
    {
      int find_by_fields_initializer_[5] = { -1, -1, -1, -1, -1 };
    }

    sql::helper::helper(const char * tablename) : table_name_(tablename), done_(false)
    {
      ::memcpy( find_by_fields_ ,find_by_fields_initializer_,sizeof(find_by_fields_initializer_));
    }

    bool sql::helper::add_field(const char * name,const char * typ, const char * flags)
    {
      if( done_ || !name || ::strlen(name) == 0 || typ == 0 || ::strlen(typ) == 0 ) return false;

      fieldlist_t::iterator it(fields_.begin());
      fieldlist_t::iterator end(fields_.end());

      for( ;it!=end;++it )
      {
        if( ::strcmp(name,(*it)->name_) == 0 )
        {
          done_ = true;
          return false;
        }
      }
      data * d = new data(name,typ,flags);
      fields_.push_back(d);
      return true;
    }

    const char * sql::helper::init_sql()
    {
      if( init_sql_.size() > 0 ) { return (const char *)init_sql_.data(); }
      common::pbuf pb;
      pb << "CREATE TABLE IF NOT EXISTS " << table_name_ << " ( ";

      fieldlist_t::iterator it(fields_.begin());
      fieldlist_t::iterator end(fields_.end());

      unsigned int i = 0;

      for( ;it!=end;++it )
      {
        if( i ) pb << " , ";
        pb << (*it)->name_ << " " << (*it)->type_ << " ";
        if( ::strlen((*it)->flags_) > 0 ) pb << (*it)->flags_;
        ++i;
      }
      pb.append((const unsigned char *)" );\0",4);
      pb.copy_to( (unsigned char *)init_sql_.allocate(pb.size()) );
      return (const char *)init_sql_.data();
    }

    const char * sql::helper::create_sql()
    {
      if( create_sql_.size() > 0 ) { return (const char *)create_sql_.data(); }
      common::pbuf pb;
      pb << "INSERT INTO " << table_name_ << " ( ";

      fieldlist_t::iterator it(fields_.begin());
      fieldlist_t::iterator end(fields_.end());

      unsigned int i = 0;

      for( ;it!=end;++it )
      {
        if( i == 0 ) {}
        else
        {
          if( i > 1 )  pb << ",";
          pb << (*it)->name_;
        }
        ++i;
      }
      pb << " ) VALUES ( ";

      for( unsigned int j=1;j<i;++j )
      {
        if( j>1 ) pb << ",";
        pb << "?";
      }

      pb.append((const unsigned char *)" );\0",4);
      pb.copy_to( (unsigned char *)create_sql_.allocate(pb.size()) );
      return (const char *)create_sql_.data();
    }

    const char * sql::helper::save_sql()
    {
      if( save_sql_.size() > 0 ) { return (const char *)save_sql_.data(); }
      common::pbuf pb;

      pb << "UPDATE " << table_name_ << " SET ";

      fieldlist_t::iterator it(fields_.begin());
      fieldlist_t::iterator end(fields_.end());

      unsigned int i = 0;

      for( ;it!=end;++it )
      {
        if( i == 0 ) {}
        else
        {
          if( i > 1 )  pb << ", ";
          pb << (*it)->name_ << "=? ";
        }
        ++i;
      }

      if( i > 0 )
      {
        it = fields_.begin();
        pb << " WHERE " << (*it)->name_ << "=? ;";
      }

      pb.copy_to( (unsigned char *)save_sql_.allocate(pb.size()) );
      return (const char *)save_sql_.data();
    }

    const char * sql::helper::remove_sql()
    {
      if( remove_sql_.size() > 0 ) { return (const char *)remove_sql_.data(); }
      common::pbuf pb;

      fieldlist_t::iterator it(fields_.begin());

      pb << "DELETE FROM " << table_name_ << " WHERE " << (*it)->name_ << "=?;";

      pb.copy_to( (unsigned char *)remove_sql_.allocate(pb.size()) );
      return (const char *)remove_sql_.data();
    }

    const char * sql::helper::find_by_id_sql()
    {
      if( find_by_id_sql_.size() > 0 ) { return (const char *)find_by_id_sql_.data(); }
      common::pbuf pb;

      pb << "SELECT ";

      fieldlist_t::iterator it(fields_.begin());
      fieldlist_t::iterator end(fields_.end());

      unsigned int i = 0;

      for( ;it!=end;++it )
      {
        if( i > 0 )  pb << ",";
        pb << (*it)->name_;
        ++i;
      }

      pb << " FROM " << table_name_ << " WHERE ";

      if( i > 0 )
      {
        it = fields_.begin();
        pb << (*it)->name_ << "=? LIMIT 1;";
      }

      pb.copy_to( (unsigned char *)find_by_id_sql_.allocate(pb.size()) );
      return (const char *)find_by_id_sql_.data();
    }

    const char * sql::helper::find_by(int field1, int field2, int field3, int field4, int field5)
    {
      int tmpi[5] = { field1, field2, field3, field4, field5 };
      if( memcmp( tmpi,find_by_fields_,sizeof(tmpi) ) == 0 )
      {
        if( find_by_sql_.size() > 0 ) { return (const char *)find_by_sql_.data(); }
      }

      common::pbuf pb;
      pb << "SELECT ";

      fieldlist_t::iterator it(fields_.begin());
      fieldlist_t::iterator end(fields_.end());

      unsigned int i = 0;

      for( ;it!=end;++it )
      {
        if( i > 0 )  pb << ",";
        pb << (*it)->name_;
        ++i;
      }

      pb << " FROM " << table_name_ << " WHERE ";

      i = 0;

      for( int k=0;k<5 && tmpi[k]!=-1;++k )
      {
        if( tmpi[k] != -1 && tmpi[k] < (int)fields_.size() )
        {
          data * d = fields_.get_at(tmpi[k]);
          if( d )
          {
            if( i > 0 ) pb << " AND ";
            pb << d->name_ << "=? ";
            ++i;
          }
        }
      }

      pb << "LIMIT 1; ";
      pb.copy_to( (unsigned char *)find_by_sql_.allocate(pb.size()) );
      return (const char *)find_by_sql_.data();
    }
  };
};

/* EOF */
