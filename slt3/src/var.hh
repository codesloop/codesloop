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

#ifndef _csl_slt3_var_hh_included_
#define _csl_slt3_var_hh_included_

/**
  @file var.hh
  @brief slt3 var
  @todo document me
 */

#include "pvlist.hh"
#include "mpool.hh"
#include "tbuf.hh"
#include "pbuf.hh"
#include "synqry.hh"
#include "str.hh"
#ifdef __cplusplus
#include <vector>

namespace csl
{
  namespace slt3
  {
    class obj;
    class intvar;
    class strvar;
    class doublevar;
    class blobvar;
    class param;

    /** @todo document me */
    class var
    {
      public:
        virtual void set_param(param & p) = 0;
        virtual bool set_value(synqry::colhead * ch,synqry::field * fd) = 0;
        virtual int type() = 0;
        virtual ~var() {}

        /** @todo document me */
        class helper
        {
          public:
            bool add_field(const char * name, var & v);

            struct data
            {
              const char * name_;
              var  * var_;
              data(const char * nm,var & v) : name_(nm), var_(&v) {}
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

        inline var(obj & parent) : parent_(&parent) {}

      private:
        var() {}
        var(const var & other) {}
        var & operator=(const var & other) { return *this; }

      protected:
        inline obj * parent() { return parent_; }

        obj * parent_;
    };

    /** @todo document me */
    class intvar : public var
    {
      public:
        enum { typ = synqry::colhead::t_integer };

        virtual inline int type() { return typ; }

        virtual void set_param(param & p);
        virtual bool set_value(synqry::colhead * ch,synqry::field * fd);
        intvar(const char * name, obj & parent,const char * flags="");

        virtual intvar & operator=(const intvar & other);
        virtual intvar & operator=(long long v);
        virtual long long operator*() const;
        virtual long long get() const;

      private:
        long long value_;
    };

    /** @todo document me */
    class strvar : public var
    {
      public:
        enum { typ = synqry::colhead::t_string };
        typedef common::tbuf<128> value_t;

        virtual inline int type() { return typ; }

        virtual void set_param(param & p);
        virtual bool set_value(synqry::colhead * ch,synqry::field * fd);
        strvar(const char * name, obj & parent,const char * flags="");

        virtual strvar & operator=(const strvar & other);
        virtual strvar & operator=(const char * other);
        virtual strvar & operator=(const common::str & other);
        virtual strvar & operator=(const value_t & other);
        virtual strvar & operator=(const common::pbuf & other);
        virtual const value_t & operator*() const;
        virtual const value_t & get() const;
        virtual const char * c_str();

      private:
        value_t value_;
    };

    /** @todo document me */
    class doublevar : public var
    {
      public:
        enum { typ = synqry::colhead::t_double };
        typedef double value_t;

        virtual inline int type() { return typ; }

        virtual void set_param(param & p);
        virtual bool set_value(synqry::colhead * ch,synqry::field * fd);
        doublevar(const char * name, obj & parent,const char * flags="");

        virtual doublevar & operator=(const doublevar & other);
        virtual doublevar & operator=(value_t other);
        virtual double operator*() const;
        virtual double get() const;

      private:
        value_t value_;
    };

    /** @todo document me */
    class blobvar : public var
    {
      public:
        enum { typ = synqry::colhead::t_blob };
        typedef common::tbuf<128> value_t;

        virtual inline int type() { return typ; }

        virtual void set_param(param & p);
        virtual bool set_value(synqry::colhead * ch,synqry::field * fd);
        blobvar(const char * name, obj & parent,const char * flags="");

        virtual blobvar & operator=(const blobvar & other);
        virtual blobvar & operator=(const value_t & other);
        virtual blobvar & operator=(const std::vector<unsigned char> & other);
        virtual blobvar & operator=(const common::pbuf & other);
        virtual const value_t & operator*() const;
        virtual const value_t & get() const;
        virtual unsigned int size();

      private:
        value_t value_;
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_slt3_var_hh_included_ */
