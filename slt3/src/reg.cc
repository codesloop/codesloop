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

/**
  @file slt3/src/reg.cc
  @brief implementation of slt3::reg
 */

#ifndef SLT3_REGISTRY_PATH1
#define SLT3_REGISTRY_PATH1 "/var/db/csl/slt3/registry.db"
#endif /*SLT3_REGISTRY_PATH1*/

#ifndef SLT3_REGISTRY_PATH2
#define SLT3_REGISTRY_PATH2 "/etc/csl/slt3/registry.db"
#endif /*SLT3_REGISTRY_PATH2*/

#ifndef SLT3_REGISTRY_PATH3
#define SLT3_REGISTRY_PATH3 "./registry.db"
#endif /*SLT3_REGISTRY_PATH3*/

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
      : name_(default_db_name), default_path_(default_db_path) { }

    const char * reg::helper::path()
    {
      reg & r(reg::instance());
      reg::pool_t p;
      reg::item i;

      /* lookup path */
      if( path_.size() != 0 ) { return path_.c_str(); }
      else if( r.get(name(),i,p) == false )
      {
        i.name_ = p.strdup(name_.c_str());
        i.path_ = p.strdup(default_path_.c_str());

        /* should be able to register the peer db */
        if( r.set(i) == false )
        {
          throw slt3::exc( slt3::exc::rs_cannot_reg,
                           slt3::exc::cm_reg,
                           name_.c_str(),
                           __FILE__,
                           __LINE__ );
        }
        return default_path_.c_str();
      }
      else
      {
        if( path_.size() == 0 ) { path_ = i.path_; }
        return path_.c_str();
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
            throw slt3::exc( slt3::exc::rs_cannot_reg,
                             slt3::exc::cm_reg,
                             name_.c_str(),
                             __FILE__,
                             __LINE__ );
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

    reg & reg::instance(const common::str & path)
    {
      if( path.size() ) return instance(path.c_str());
      else              return instance(0);
    }

    namespace
    {
      bool init_db(conn & c,const common::str & path)
      {
        c.use_exc(true);
        if( c.open(path.c_str()) )
        {
          tran t(c);
          synqry q(t);
          return q.execute(
              "CREATE TABLE IF NOT EXISTS registry ( "
              " id INTEGER PRIMARY KEY ASC AUTOINCREMENT, "
              " name string UNIQUE NOT NULL, "
              " path string NOT NULL ); " );
        }
        return false;
      }
    }

    bool reg::get( const common::str & name, item & i, pool_t & pool )
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

        synqry::columns_t ch;
        synqry::fields_t  fd;

        c.use_exc(true);

        tran t(c);
        synqry q(t);

        param & p(q.get_param(1));
        p.set(name);

        if( !q.prepare("SELECT id,name,path FROM registry where name=?;") ) return false;

        q.next(ch,fd);

        if( fd.size() > 0 )
        {
          i.id_ = fd.get_at(0)->intval_;
          i.name_ = pool.strdup(fd.get_at(1)->stringval_);
          i.path_ = pool.strdup(fd.get_at(2)->stringval_);
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

        synqry::columns_t ch;
        synqry::fields_t  fd;

        tran t(c);
        synqry q(t);

        param & p(q.get_param(1));
        p.set(name);

        if( !q.prepare("SELECT path FROM registry WHERE name=? limit 1;") ) return false;

        if( q.next(ch,fd) )
        {
          return cn.open( fd.get_at(0)->stringval_ );
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

        synqry::columns_t ch;
        synqry::fields_t  fd;

        tran t(c);
        synqry q(t);

        if( !q.prepare("SELECT name FROM registry;") ) return false;

        bool ret = false;

        while( q.next(ch,fd) )
        {
          nms.push_back( pool.strdup(fd.get_at(0)->stringval_) );
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

        synqry::columns_t ch;
        synqry::fields_t  fd;

        tran t(c);
        synqry q(t);

        if( !q.prepare("SELECT id,name,path FROM registry;") ) return false;

        bool ret = false;

        while( q.next(ch,fd) )
        {
          item * p = (item *)pool.allocate( sizeof(item) );
          p->id_ = fd.get_at(0)->intval_;
          p->name_ = pool.strdup(fd.get_at(1)->stringval_);
          p->path_ = pool.strdup(fd.get_at(2)->stringval_);
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
        synqry q(t);

        q.use_exc(true);

        param & p1(q.get_param(1));
        param & p2(q.get_param(2));
        p1.set(it.name_);
        p2.set(it.path_);

        if( !q.prepare("INSERT INTO registry (name,path) VALUES(?,?);") ) return false;

        q.next(); // should throw an exception if failed

        return true;
      }
      catch( slt3::exc e )
      {
        return false;
      }
    }

    bool reg::get( const common::str & name, conn & cn )
    {
      return get( name.c_str(),cn );
    }
  }
}

/* EOF */
