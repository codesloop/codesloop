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
#include "reg.hh"
#include "common.h"
#include "str.hh"
#include "ustr.hh"
#include "int64.hh"

/**
  @file slt3/src/reg.cc
  @brief implementation of slt3::reg
 */

using csl::common::str;
using csl::common::ustr;
using csl::common::int64;

namespace csl
{
  namespace slt3
  {
    namespace
    {
      bool check_path(const char * path)
      {
        if( !path ) return false;

        FILE * fp = fopen(path,"a+");
        if( !fp ) return false;
        fclose( fp );
        return true;
      }
    }

    reg::helper::helper(const char * default_db_name, const char * default_db_path)
      : name_(default_db_name), default_path_(default_db_path), use_exc_(true) { }

    const char * reg::helper::path()
    {
      reg & r(reg::instance());
      reg::pool_t p;
      reg::item i;

      /* lookup path */
      if( path_ != 0 ) { return path_; }
      else if( r.get(name(),i,pool_) == false )
      {
        i.name_ = p.strdup(name_);
        i.path_ = p.strdup(default_path_);

        /* should be able to register the peer db */
        if( r.set(i) == false )
        {
          str nm(name_);

          THRR( slt3::exc::rs_cannot_reg, slt3::exc::cm_reg, nm.c_str(), NULL );
        }
        return default_path_;
      }
      else
      {
        if( path_ == 0 ) { path_ = i.path_; }
        return path_;
      }
    }

    conn & reg::helper::db()
    {
      if( conn_.name().size() == 0 )
      {
        reg & r(reg::instance());

        if( r.get(name(),conn_) == false )
        {
          /* retry once */
          (void)path();
          if( r.get(name(),conn_) == false )
          {
            str nm(name_);

            THRR( slt3::exc::rs_cannot_reg, slt3::exc::cm_reg, nm.c_str(), conn_ );
          }
        }
      }
      return conn_;
    }

    reg & reg::instance()
    {
      if( check_path(SLT3_REGISTRY_PATH1) )
      {
        return instance(SLT3_REGISTRY_PATH1);
      }
      else if( check_path(SLT3_REGISTRY_PATH2) )
      {
        return instance(SLT3_REGISTRY_PATH2);
      }
      else if( check_path(SLT3_REGISTRY_PATH3) )
      {
        return instance(SLT3_REGISTRY_PATH3);
      }
      else
      {
        return instance(0);
      }
    }

    reg & reg::instance(const char * p)
    {
      static reg * instance_ = 0;
      if( !instance_ ) instance_ = new reg();

      if( p )
      {
        instance_->path(p);
      }
      return *instance_;
    }

    reg & reg::instance(const ustr & path)
    {
      if( path.size() ) return instance(path.c_str());
      else              return instance(0);
    }

    namespace
    {
      bool init_db(conn & c,const ustr & path)
      {
        c.use_exc(true);
        if( c.open(path.c_str()) )
        {
          tran t(c);
          query q(t);
          return q.execute(
               "CREATE TABLE IF NOT EXISTS registry ( "
               " id INTEGER PRIMARY KEY ASC AUTOINCREMENT, "
               " name string UNIQUE NOT NULL, "
               " path string NOT NULL ); " );
        }
        return false;
      }
    }

    bool reg::get( const ustr & name, item & i, pool_t & pool )
    {
      return get( name.c_str(),i,pool );
    }

    bool reg::get( const char * name, item & i, pool_t & pool )
    {
      if( !name ) return false;
      try
      {
        conn c;
        if( !init_db(c,path_) ) return false;

        query::columns_t ch;
        query::fields_t  fd;

        c.use_exc(true);

        tran t(c);
        query q(t);

        ustr & p(q.ustr_param(1));
        p = name;

        if( !q.prepare("SELECT id,name,path FROM registry where name=?;") ) return false;

        q.next(ch,fd);

        if( fd.size() > 0 )
        {
          i.id_   = ((int64 *)fd.get_at(0))->value();
          i.name_ = pool.strdup( ((ustr *)fd.get_at(1))->c_str() );
          i.path_ = pool.strdup( ((ustr *)fd.get_at(2))->c_str() );
          return true;
        }
        else
        {
          return false;
        }
      }
      catch( slt3::exc e )
      {
        return false;
      }
    }

    bool reg::get( const char * name, conn & cn )
    {
      if( !name ) return false;
      try
      {
        conn c;
        if( !init_db(c,path_) ) return false;

        query::columns_t ch;
        query::fields_t  fd;

        tran t(c);
        query q(t);

        ustr & p(q.ustr_param(1));
        p = name;

        if( !q.prepare("SELECT path FROM registry WHERE name=? limit 1;") ) return false;

        if( q.next(ch,fd) )
        {
          ustr s;
          fd.get_at(0)->to_string(s);
          return cn.open( s.c_str() );
        }
        return false;
      }
      catch( slt3::exc e )
      {
        return false;
      }
    }

    bool reg::names( strlist_t & nms, pool_t & pool )
    {
      try
      {
        conn c;
        if( !init_db(c,path_) ) return false;

        query::columns_t ch;
        query::fields_t  fd;

        tran t(c);
        query q(t);

        if( !q.prepare("SELECT name FROM registry;") ) return false;

        bool ret = false;

        while( q.next(ch,fd) )
        {
          ustr s;
          fd.get_at(0)->to_string(s);
          nms.push_back( pool.strdup(s.c_str()) );
          ret = true;
        }

        return ret;
      }
      catch( slt3::exc e )
      {
        return false;
      }
    }

    bool reg::dbs( itemlist_t & itms, pool_t & pool )
    {
      try
      {
        conn c;
        if( !init_db(c,path_) ) return false;

        query::columns_t ch;
        query::fields_t  fd;

        tran t(c);
        query q(t);

        if( !q.prepare("SELECT id,name,path FROM registry;") ) return false;

        bool ret = false;

        while( q.next(ch,fd) )
        {
          item * p = (item *)pool.allocate( sizeof(item) );
          p->id_   = ((int64 *)fd.get_at(0))->value();
          p->name_ = pool.strdup( ((ustr *)fd.get_at(1))->c_str() );
          p->path_ = pool.strdup( ((ustr *)fd.get_at(2))->c_str() );
          itms.push_back( p );
          ret = true;
        }

        return ret;
      }
      catch( slt3::exc e )
      {
        return false;
      }
    }

    bool reg::set( const item & it )
    {
      try
      {
        conn c;
        if( !init_db(c,path_) ) return false;

        tran t(c);
        query q(t);

        q.use_exc(true);

        ustr & p1(q.ustr_param(1));
        ustr & p2(q.ustr_param(2));
        p1 = it.name_;
        p2 = it.path_;

        if( !q.prepare("INSERT INTO registry (name,path) VALUES(?,?);") ) return false;

        q.next(); // should throw an exception if failed

        return true;
      }
      catch( slt3::exc e )
      {
        return false;
      }
    }

    bool reg::get( const ustr & name, conn & cn )
    {
      return get( name.c_str(),cn );
    }
  }
}

/* EOF */
