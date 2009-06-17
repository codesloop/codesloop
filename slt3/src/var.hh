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

#ifndef _csl_slt3_var_hh_included_
#define _csl_slt3_var_hh_included_

/**
  @file var.hh
  @brief slt3 var
  
  implementation of slt3 variables that helps 'instrumenting' classes to easily use the
  simple object relational mapping provided by slt3. these variables bind member variables to
  database fields.
 */

#include "pvlist.hh"
#include "pbuf.hh"
#include "query.hh"
#include "int64.hh"
#include "dbl.hh"
#include "binry.hh"
#include "ustr.hh"
#ifdef __cplusplus
#include <vector>

namespace csl
{
  namespace slt3
  {
    class obj;

    /**
    @brief slt3::var is the base class of other slt3 variable mappers
    
    slt3::var variables represents the mapping between member variables of classes and database fields.
    this is part of the object relational mapping facilities of slt3. the mapping is done with the help of
    member variables that are registered by var::helper.
    */
    class var_base
    {
    public:
      /* abstract functions */
      //virtual void set_param(common::var & p) = 0;
      virtual bool set_value(query::colhead * ch,query::field * fd) = 0;
      virtual common::var * get_value() = 0;
      virtual int type() = 0;

      inline virtual ~var_base() {}
      inline var_base(obj & parent) : parent_(&parent) {}

    protected:
      /* helpers */
      virtual void register_variable(var_base * v,const char * name,const char * coltype,slt3::obj & parent,const char * flags); 

      inline virtual obj * parent() { return parent_; }

      obj * parent_;
        
    public:
      /** @todo document me */
      class helper
      {
      public:
        bool add_field(const char * name, var_base & v);
      
          struct data
          {
            const char * name_;
            var_base  * var_;
            data(const char * nm,var_base & v) : name_(nm), var_(&v) {}
          };

        typedef common::pvlist< 32,data,common::delete_destructor<data> > datalist_t;

        bool init(tran & t, const char * sql_query);
        bool create(tran & t, const char * sql_query);
        bool save(tran & t, const char * sql_query);
        bool remove(tran & t, const char * sql_query);
        bool find_by_id(tran & t, const char * sql_query);
        bool find_by(tran & t,
                      const char * sql_query,
                      int field1,
                      int field2=-1,
                      int field3=-1,
                      int field4=-1,
                      int field5=-1);

        void set_id(long long id);

      private:
        datalist_t dtalst_;
      };

    private:
      /* not allowed to call these */
      var_base() {}
      var_base(const var_base & other) {}
      var_base & operator=(const var_base & other) { return *this; }
    };
    
    template <typename T> struct var_col_type {};
    template <> struct var_col_type<common::int64> { static const char * coltype_s; };
    template <> struct var_col_type<common::dbl> { static const char * coltype_s; };
    template <> struct var_col_type<common::ustr> { static const char * coltype_s; };
    template <> struct var_col_type<common::binry> { static const char * coltype_s; };
    
    template <typename T> class varT : public var_base
    {
    public:
      inline varT(const char * name, slt3::obj & parent,const char * flags="") : var_base(parent)
      {
        register_variable(this,name,var_col_type<T>::coltype_s,parent,flags);
      }
      
      enum { type_v = T::var_type_v };

      inline int type() { return type_v; }
      
      inline bool set_value(query::colhead * ch,query::field * fd)
      {
        if( !ch || !fd ) return false;
        bool ret = value_.from_var(*fd);
        parent()->on_change();
        return ret;
      }
      
      typedef T tval_t;
      typedef typename T::value_t value_t;
      
      inline value_t get() const { value_.value(); }

      template <typename V> inline bool get(V & v) const { return value_.get(v); }
      template <typename V> inline bool set(V & v) { return value_.set(v); }

      inline varT & operator=(const char * other)
      {
        value_.from_string(other);
        return *this;
      }
      
      inline varT & operator=(const wchar_t * other)
      {
        value_.from_string(other);
        return *this;
      }
      
      inline varT & operator=(const T & other)
      {
        value_ = other;
        return *this;
      }
      
      inline varT & operator=(const varT & other)
      {
        value_ = other.value_;
        return *this;
      }
      
      inline varT & operator=(const common::binry::buf_t & other)
      {
        value_.from_binary(other.data(),other.size());
        return *this;
      }

      inline varT & operator=(const std::vector<unsigned char> & vref)
      {
        value_.from_binary( &(vref[0]), vref.size() );
        return *this;
      }
      
      inline common::var * get_value() { return &value_; }
      
    private:
      T value_;
    };
    
    typedef varT<common::int64> intvar;
    typedef varT<common::dbl>   doublevar;
    typedef varT<common::ustr>  strvar;
    typedef varT<common::binry> blobvar;

  } /* end of slt3 namespace */
} /* end of csl namespace */

#endif /* __cplusplus */
#endif /* _csl_slt3_var_hh_included_ */
