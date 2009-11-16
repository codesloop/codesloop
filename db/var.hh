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

#ifndef _csl_db_var_hh_included_
#define _csl_db_var_hh_included_

/**
  @file var.hh
  @brief slt3 var

  implementation of slt3 variables that helps 'instrumenting' classes to easily use the
  simple object relational mapping provided by slt3. these variables bind member variables to
  database fields.
 */

#include "codesloop/common/pvlist.hh"
#include "codesloop/common/pbuf.hh"
#include "codesloop/db/query.hh"
#include "codesloop/common/int64.hh"
#include "codesloop/common/dbl.hh"
#include "codesloop/common/binry.hh"
#include "codesloop/common/ustr.hh"
#ifdef __cplusplus
#include <vector>

namespace csl
{
  namespace db
  {
    namespace slt3
    {
      class obj;

      /**
      @brief slt3::var is the base class of other slt3 variable mappers

      slt3::var variables represents the mapping between member variables of classes and database fields.
      this is part of the object relational mapping facilities of slt3. the mapping is done with the help of
      member variables that are registered by var::helper.

      the ORM mapping facilities are: sql::helper , reg::helper and var_base::helper they register ORM
      variables, generate SQL strings and maps database columns to variables.
       */
      class var_base
      {
      public:
        /* abstract functions */

        /**
        @brief abtract function for setting the ORM-property variable from the result of an SQL query
        @param ch is a pointer to column header data as returned from the database query
        @param fd is a pointer to field data as returned from the database query
        @return true if successful
         */
        virtual bool set_value(query::colhead * ch,query::field * fd) = 0;

        /** @brief abstract function to get the internal variable (common::var) */
        virtual const common::var * get_value() const = 0;

        /** @brief returns the variable's type as specified in common variables or query::colhead enumeration (same) */
        virtual int type() = 0;

        /** @brief destructor */
        inline virtual ~var_base() {}

        /** @brief initializing constructor */
        inline var_base(obj & prn) : parent_(&prn) {}

      protected:

        /**
        @brief register a variable with both sql::helper and var_base::helper
        @param v is a pointer to the variable to be initialized
        @param name is the database column name associated with the variable
        @param coltype is the database column type like (INTEGER, REAL, BLOB, ...)
        @param parent is a pointer to the actual class instance
        @param flags are the misc properties of the database column like (UNIQUE, DEFAULT, PRIMARY KEY, etc...)
         */
        virtual void register_variable(var_base * v, const char * name, const char * coltype, slt3::obj & parent, const char * flags);

        /** @brief returns a pointer to the parent object */
        inline virtual obj * parent() { return parent_; }

        obj * parent_;

      public:

        /**
        @brief var_base::helper keeps track of variables and helps the ORM functionality
         */
        class helper
        {
          public:
            /**
            @brief registers a database variable
            @param name is the variable's database column name
            @param v is a reference to the given variable
             */
            bool add_field(const char * name, var_base & v);

            /** @brief represents a data field */
            struct data
            {
              const char * name_;
              var_base  * var_;
              data(const char * nm,var_base & v) : name_(nm), var_(&v) {}
            };

            typedef common::pvlist< 32,data,common::delete_destructor<data> > datalist_t;

            /**
            @brief initializes the database table associated with the object
            @param t is the transaction context to be used
            @param sql_query as generated by sql::helper
            @return true if successful

            the net result of this call is a CREATE TABLE ... SQL call
             */
            bool init(tran & t, const char * sql_query);

            /**
            @brief creates an instance of the given object in the database
            @param t is the transaction context to be used
            @param sql_query as generated by sql::helper
            @return true if successful

            the net result of this call is a INSERT ... SQL call
            it will also call the objects set_id() call to register the
            identifier of the given instance.
             */
            bool create(tran & t, const char * sql_query);

            /**
            @brief updates an instance of the given object in the database
            @param t is the transaction context to be used
            @param sql_query as generated by sql::helper
            @return true if successful

            the net result of this call is a UPDATE ... SQL call
             */
            bool save(tran & t, const char * sql_query);

            /**
            @brief deletes an instance of the given object from the database
            @param t is the transaction context to be used
            @param sql_query as generated by sql::helper
            @return true if successful

            the net result of this call is a DELETE ... SQL call
             */
            bool remove(tran & t, const char * sql_query);

            /**
            @brief finds an instance of the given object in the database
            @param t is the transaction context to be used
            @param sql_query as generated by sql::helper
            @return true if successful

            the net result of this call is a SELECT ... SQL call

            the object's database variables will be filled with the returned data
             */
            bool find_by_id(tran & t, const char * sql_query);

            /**
            @brief finds an instance of the given object in the database
            @param t is the transaction context to be used
            @param sql_query as generated by sql::helper
            @param field1 to be put into the where conditions
            @param field2 to be put into the where conditions (if not -1)
            @param field3 to be put into the where conditions (if not -1)
            @param field4 to be put into the where conditions (if not -1)
            @param field5 to be put into the where conditions (if not -1)
            @return true if successful

            the net result of this call is a SELECT ... SQL call

            the object's database variables will be filled with the returned data. the
            fieldx variables will be used as condition variables. the fields with non-default
            values represent a variable by index. indexing start from zero. the variables
            given are logically AND'ed together.

            to select on the 3rd and 4th variable of the given object one must first set those
            variables to the desired values to be searched for, and then call this function
            with field1=2 and field2=3.
             */
            bool find_by(tran & t,
                        const char * sql_query,
                        int field1,
                        int field2=-1,
                        int field3=-1,
                        int field4=-1,
                        int field5=-1);


            /**
            @brief helper that sets the instance id
            @param id is the id to be set

            please note that this function assumes that the first variable is the id to be set
             */
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

      /** @brief helper to statically determine the SQL column type string based on the template parameter */
      template <typename T> struct var_col_type {};
      template <> struct var_col_type<common::int64> { static const char * coltype_s; };
      template <> struct var_col_type<common::dbl> { static const char * coltype_s; };
      template <> struct var_col_type<common::ustr> { static const char * coltype_s; };
      template <> struct var_col_type<common::binry> { static const char * coltype_s; };

      /**
      @brief templated variable type that respresents a variable in the ORM user object

      the varT type is basically a common::var family type plus a name and flags. these
      together represent a column in the database. the ORM mapper maps classes to tables and
      member variables to table-columns. the mapping is done through various helper classes
      like:

      @li @em sql::helper that generates the SQL query strings
      @li @em reg::helper that helps in locating the database for the given class
      @li @em var_base::helper associates the member variables with DB columns and converts between the DB and the variables

      */
      template <typename T> class varT : public var_base
      {
      public:

        /**
        @brief initializing constructor
        @param name is the column name to be used
        @param parent is a refernce to the class whose member is the variable
        @param flags is the database flags associated with the database column (like UNIQUE, PRIMARY KEY, AUTO_INCREMENT etc...)

        this constructor registers the variable instance with the sql::helper and the var_base::helper instance
        */
        inline varT(const char * name, slt3::obj & prn,const char * flags="") : var_base(prn)
        {
          register_variable(this,name,var_col_type<T>::coltype_s,prn,flags);
        }

        /** @brief the variable types are one of common::var family types */
        enum { type_v = T::var_type_v };

        /** @brief returns the variable's type */
        inline int type() { return type_v; }

        /**
        @brief sets the variable's content based on info returned from DB query
        @param ch represents a column header
        @param fd is the field's data
        */
        inline bool set_value(query::colhead * ch,query::field * fd)
        {
          if( !ch || !fd ) return false;
          bool ret = value_.from_var(*fd);
          // parent()->on_change(); TODO : check this!!!
          return ret;
        }

        typedef T tval_t;
        typedef typename T::value_t value_t;

        /** @brief returns the variable's value in the original representation

        that is one of:
        @li long long
        @li doube
        @li char *
        @li tbuf<>

        */
        inline value_t get() const { return value_.value(); }

        /**
        @brief forwards the get() operation to the value's get() operation
        @see common::var for more information
        */
        template <typename V> inline bool get(V & v) const { return value_.get(v); }

        /**
        @brief forwards the set() operation to the value's set() operation
        @see common::var for more information
        */
        template <typename V> inline bool set(V & v)
        {
          bool ret = value_.set(v);
          if( ret ) parent()->on_change();
          return ret;
        }

        /** @brief forward operator checks for equality */
        template <typename V> inline bool operator==(V & other) const
        {
          return (value_ == other);
        }

        /** @brief copy constructor (initialize from char * string) */
        inline varT & operator=(const char * other)
        {
          bool success = value_.from_string(other);
          if( success ) parent()->on_change();
          return *this;
        }

        /** @brief copy constructor (initialize from wide char string) */
        inline varT & operator=(const wchar_t * other)
        {
          bool success = value_.from_string(other);
          if( success ) parent()->on_change();
          return *this;
        }

        /** @brief copy constructor (initialize from an ther variable) */
        inline varT & operator=(const T & other)
        {
          bool success = value_.from_var(other);
          if( success ) parent()->on_change();
          return *this;
        }

        /** @brief copy constructor (initialize from an other variable - varT) */
        inline varT & operator=(const varT & other)
        {
          bool success = value_.from_var(other.value_);
          if( success ) parent()->on_change();
          return *this;
        }

        /** @brief copy constructor (initialize from binary buffer) */
        inline varT & operator=(const common::binry::buf_t & other)
        {
          bool success = value_.from_binary(other.data(),other.size());
          if( success ) parent()->on_change();
          return *this;
        }

        /** @brief copy constructor (initialize from unsigned char vector) */
        inline varT & operator=(const std::vector<unsigned char> & vref)
        {
          bool success = value_.from_binary( &(vref[0]), vref.size() );
          if( success ) parent()->on_change();
          return *this;
        }

        /** @brief returns a pointer to the internal value */
        inline const common::var * get_value() const { return &value_; }

      private:
        T value_;
      };

      typedef varT<common::int64> intvar;     ///<int64 variable (long long)
      typedef varT<common::dbl>   doublevar;  ///<dbl variable (double)
      typedef varT<common::ustr>  strvar;     ///<ustr variable (char string)
      typedef varT<common::binry> blobvar;    ///<binry variable (tbuf based binary value)

    } /* end of slt3 namespace */
  } /* end of db namespace */
} /* end of csl namespace */

#endif /* __cplusplus */
#endif /* _csl_db_var_hh_included_ */
