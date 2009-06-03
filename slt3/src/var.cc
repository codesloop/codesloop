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

#include "common.h"
#include "obj.hh"
#include "var.hh"
#include "sql.hh"
#include "conn.hh"
#include "tran.hh"
#include "synqry.hh"
#include "param.hh"
#include "str.hh"

/**
  @file var.cc
  @brief implementation of slt3::var
 */

namespace csl
{
  namespace slt3
  {
    void var::helper::set_id(long long id)
    {
      datalist_t::iterator it(dtalst_.begin());
      datalist_t::iterator end(dtalst_.end());
      if( it != end )
      {
        data * dx = *it;

        synqry::colhead ch;
        synqry::field fd;

        fd.intval_ = id;
        fd.size_   = sizeof(fd.intval_);
        ch.type_   = synqry::colhead::t_integer;

        dx->var_->set_value(&ch,&fd);
      }
    }

    /* 'CREATE TABLE IF NOT EXISTS Xtable ( id INTEGER PRIMARY KEY ASC AUTOINCREMENT , 
                                            name TEXT NOT NULL ,
                                            height REAL DEFAULT (0.1) ,
                                            pk BLOB  ); */
    bool var::helper::init(tran & t, const wchar_t * sql_query)
    {
      synqry q(t);
      if( q.prepare(sql_query) == false ) return false;
      q.next();
      return true;
    }

    /* 'INSERT INTO Xtable ( name,height,pk ) VALUES ( ?,?,? );' */
    bool var::helper::create(tran & t, const wchar_t * sql_query)
    {
      synqry q(t);

      datalist_t::iterator it(dtalst_.begin());
      datalist_t::iterator end(dtalst_.end());

      unsigned int i = 0;

      for( ;it!=end;++it )
      {
        /* skip the first parameter, assuming it is the primary key ... */
        if( i > 0 )
        {
          param & p(q.get_param(i));
          data * d = *it;
          d->var_->set_param(p);
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
    bool var::helper::save(tran & t, const wchar_t * sql_query)
    {
      synqry q(t);

      datalist_t::iterator it(dtalst_.begin());
      datalist_t::iterator end(dtalst_.end());

      unsigned int i = 0;

      for( ;it!=end;++it )
      {
        /* set parameters while skipping id */
        if( i > 0 )
        {
          param & p(q.get_param(i));
          data * d = *it;
          d->var_->set_param(p);
        }
        ++i;
      }

      /* set id at last */
      datalist_t::iterator ix(dtalst_.begin());
      param & p_id(q.get_param(i));
      data * dx = *ix;
      dx->var_->set_param(p_id);
      ++i;

      if( !q.prepare(sql_query) ) return false;

      q.next();

      /* check if succeed */
      if( q.change_count() > 0 ) return true;
      else                       return false;
    }

    /* 'DELETE FROM Xtable WHERE id=?;' */
    bool var::helper::remove(tran & t, const wchar_t * sql_query)
    {
      synqry q(t);

      datalist_t::iterator it(dtalst_.begin());
      param & p_id(q.get_param(1));
      data * dx = *it;
      dx->var_->set_param(p_id);

      if( !q.prepare(sql_query) ) return false;

      q.next();

      /* check if succeed */
      if( q.change_count() > 0 ) return true;
      else                       return false;
    }

    /* 'SELECT id,name,height,pk FROM Xtable WHERE id=? LIMIT 1;' */
    bool var::helper::find_by_id(tran & t, const wchar_t * sql_query)
    {
      synqry q(t);

      /* set id */
      datalist_t::iterator ix(dtalst_.begin());
      param & p_id(q.get_param(1));
      data * dx = *ix;
      dx->var_->set_param(p_id);

      if( !q.prepare(sql_query) ) return false;

      synqry::columns_t ch;
      synqry::fields_t  fd;

      q.next(ch,fd);

      if( ch.size() > 0 && fd.size() > 0 )
      {
        synqry::columns_t::iterator chit(ch.begin());
        synqry::columns_t::iterator chend(ch.end());

        synqry::fields_t::iterator fdit(fd.begin());
        synqry::fields_t::iterator fdend(fd.end());

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

    bool var::helper::find_by(tran & t,
                              const wchar_t * sql_query,
                              int field1,
                              int field2,
                              int field3,
                              int field4,
                              int field5)
    {
      int tmpi[5] = { field1, field2, field3, field4, field5 };

      synqry q(t);

      for( int k=0;k<5 && tmpi[k] != -1; ++k )
      {
        data * d = dtalst_.get_at(tmpi[k]);
        if( d )
        {
          param & p(q.get_param(k+1));
          d->var_->set_param(p);
        }
      }

      if( !q.prepare(sql_query) ) return false;

      synqry::columns_t ch;
      synqry::fields_t  fd;

      q.next(ch,fd);

      if( ch.size() > 0 && fd.size() > 0 )
      {
        synqry::columns_t::iterator chit(ch.begin());
        synqry::columns_t::iterator chend(ch.end());

        synqry::fields_t::iterator fdit(fd.begin());
        synqry::fields_t::iterator fdend(fd.end());

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


    bool var::helper::add_field(const wchar_t * name, var & v)
    {
      data * d = new data(name,v);
      dtalst_.push_back(d);
      return true;
    }

    void intvar::set_param(param & p)    { p.set(value_); }
    void strvar::set_param(param & p)    { p.set((const wchar_t *)value_.data()); }
    void doublevar::set_param(param & p) { p.set(value_); }
    void blobvar::set_param(param & p)   { p.set(value_.data(),value_.size()); }

    bool intvar::set_value(synqry::colhead * ch,synqry::field * fd)
    {
      if( !ch || !fd ) return false;

      wchar_t * endp = 0;

      switch( ch->type_ )
      {
        case synqry::colhead::t_integer:
          if( fd->size_ != sizeof(long long) ) return false;
          value_ = fd->intval_;
          break;

        case synqry::colhead::t_string:
          if( !fd->stringval_ || !fd->size_ ) return false;
          value_ = WCSTOLL(fd->stringval_,&endp,10);
          break;

        case synqry::colhead::t_double:
          if( fd->size_ != sizeof(double) ) return false;
          value_ = (long long)(fd->doubleval_);
          break;

        case synqry::colhead::t_blob:
          if( fd->size_ != sizeof(long long) ) return false;
          value_ = fd->intval_;
          break;

        case synqry::colhead::t_null:
          value_ = 0;
          break;

        default:
          return false;
      };
      parent()->on_change();
      return true;
    }

    bool doublevar::set_value(synqry::colhead * ch,synqry::field * fd)
    {
      if( !ch || !fd ) return false;

      wchar_t * endp = 0;

      switch( ch->type_ )
      {
        case synqry::colhead::t_integer:
          if( fd->size_ != sizeof(long long) ) return false;
          value_ = (double)fd->intval_;
          break;

        case synqry::colhead::t_string:
          if( !fd->stringval_ || !fd->size_ ) return false;
          value_ = WCSTOLD(fd->stringval_,&endp);
          break;

        case synqry::colhead::t_double:
          if( fd->size_ != sizeof(double) ) return false;
          value_ = fd->doubleval_;
          break;

        case synqry::colhead::t_blob:
          if( fd->size_ != sizeof(double) ) return false;
          value_ = fd->doubleval_;
          break;

        case synqry::colhead::t_null:
          value_ = 0.0;
          break;

        default:
          return false;
      };
      parent()->on_change();
      return true;
    }

    bool strvar::set_value(synqry::colhead * ch,synqry::field * fd)
    {
      if( !ch || !fd ) return false;

      wchar_t tmp[200];

      switch( ch->type_ )
      {
        case synqry::colhead::t_integer:
          SNPRINTF( tmp, sizeof(tmp)-1,L"%lld",fd->intval_ );
          value_ = tmp;
          break;

        case synqry::colhead::t_double:
          SNPRINTF( tmp, sizeof(tmp)-1,L"%.10f", fd->doubleval_ );
          value_ = tmp;
          break;

        case synqry::colhead::t_string:
          if( fd->size_ == 0 ) { value_.reset(); break; }
          // TODO carefully check results
          value_.assign(fd->stringval_,fd->stringval_+fd->size_);
          break;

        case synqry::colhead::t_blob:
          if( fd->size_ == 0 ) { value_.reset(); break; }
          value_.assign(fd->stringval_,fd->stringval_+fd->size_);
          // TODO carefully check results
          break;

        case synqry::colhead::t_null:
          value_.reset();
          break;

        default:
          return false;
      };
      parent()->on_change();
      return true;
    }

    bool blobvar::set_value(synqry::colhead * ch,synqry::field * fd)
    {
      if( !ch || !fd ) return false;
      if( ch->type_ == synqry::colhead::t_null || fd->size_ == 0 || fd->blobval_ == 0 )
      {
        value_.reset();
      }
      else
      {
        value_.set(fd->blobval_,fd->size_);
      }
      parent()->on_change();
      return true;
    }

    intvar::intvar(const wchar_t * name, obj & parent,const wchar_t * flags) : var(parent), value_(0)
    {
      sql::helper & h(parent.sql_helper());
      var::helper & v(parent.var_helper());
      h.add_field(name,L"INTEGER",flags);
      v.add_field(name,*this);
    }

    strvar::strvar(const wchar_t * name, obj & parent,const wchar_t * flags) : var(parent)
    {
      value_.ensure_trailing_zero();
      sql::helper & h(parent.sql_helper());
      var::helper & v(parent.var_helper());
      h.add_field(name,L"TEXT",flags);
      v.add_field(name,*this);
    }

    doublevar::doublevar(const wchar_t * name, obj & parent,const wchar_t * flags) : var(parent), value_(0.0)
    {
      sql::helper & h(parent.sql_helper());
      var::helper & v(parent.var_helper());
      h.add_field(name,L"REAL",flags);
      v.add_field(name,*this);
    }

    blobvar::blobvar(const wchar_t * name, obj & parent,const wchar_t * flags) : var(parent)
    {
      sql::helper & h(parent.sql_helper());
      var::helper & v(parent.var_helper());
      h.add_field(name,L"BLOB",flags);
      v.add_field(name,*this);
    }

    /* operators */
    intvar & intvar::operator=(const intvar & other)
    {
      value_ = other.value_;
      parent()->on_change();
      return *this;
    }

    intvar & intvar::operator=(long long v)
    {
      value_ = v;
      parent()->on_change();
      return *this;
    }

    long long intvar::operator*() const { return value_; }
    long long intvar::get() const { return value_; }

    strvar & strvar::operator=(const wchar_t * other)
    {
      if( !other )
      {
        value_.reset();
        value_.ensure_trailing_zero();
      }
      else
      {
        size_t sz = sizeof(wchar_t)*(wcslen(other)+1);
        value_ = other; //.assign( other,other+sz );
      }
      parent()->on_change();
      return *this;
    }

    strvar & strvar::operator=(const common::str & other)
    {
      if( other.size() )
      {
        value_ = other;
      }
      else
      {
        value_.reset();
        value_.ensure_trailing_zero();
      }
      parent()->on_change();
      return *this;
    }

    strvar & strvar::operator=(const strvar & other)
    {
      value_ = other.value_;
      parent()->on_change();
      return *this;
    }

    strvar & strvar::operator=(const common::pbuf & other)
    {
      value_ = other;
      parent()->on_change();
      return *this;
    }

    const strvar::value_t & strvar::operator*() const { return value_; }
    const strvar::value_t & strvar::get() const { return value_; }

    const wchar_t * strvar::c_str()
    {
      value_.ensure_trailing_zero();
      return value_.data();
    }

    doublevar & doublevar::operator=(const doublevar & other)
    {
      value_ = other.value_;
      parent()->on_change();
      return *this;
    }

    doublevar & doublevar::operator=(value_t other)
    {
      value_ = other;
      parent()->on_change();
      return *this;
    }

    double doublevar::operator*() const { return value_; }
    double doublevar::get() const { return value_; }

    blobvar & blobvar::operator=(const blobvar & other)
    {
      value_ = other.value_;
      parent()->on_change();
      return *this;
    }

    blobvar & blobvar::operator=(const value_t & other)
    {
      value_ = other;
      parent()->on_change();
      return *this;
    }

    blobvar & blobvar::operator=(const std::vector<unsigned char> & other)
    {
      if( other.size() > 0 ) value_.set( &(other[0]), other.size());
      else                   value_.reset();
      parent()->on_change();
      return *this;
    }

    blobvar & blobvar::operator=(const common::pbuf & other)
    {
      value_ = other;
      parent()->on_change();
      return *this;
    }

    blobvar & blobvar::operator=(const common::str & other)
    {
      if( other.size() > 0 ) value_.set( (const unsigned char *)other.data(), other.size() );
      else                   value_.reset();
      parent()->on_change();
      return *this;
    }

    const blobvar::value_t & blobvar::operator*() const { return value_; }
    const blobvar::value_t & blobvar::get() const  { return value_; }

    unsigned int blobvar::size() { return value_.size(); }
  };
};

/* EOF */
