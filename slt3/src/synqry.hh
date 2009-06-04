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

#ifndef _csl_slt3_synqry_hh_included_
#define _csl_slt3_synqry_hh_included_

#include "pvlist.hh"
#include "str.hh"
#include "ustr.hh"
#ifdef __cplusplus
#include <memory>

/**
   @file synqry.hh
   @brief Represents a SQLite3 query object
 */

namespace csl
{
  namespace slt3
  {
    class tran;
    class param;

    /**
    @brief synqry represents a SQLite3 query

    synqry supports one step, iterative and parametrized queries

    synqry only runs as a child of tran. this is a design decision to enforce the
    usage of transactions.

    integer, double, string and blob datatypes are supported

    to specify query parameters one needs to obtain a param reference first and then
    set its value. the memory of the param values are managed by synqry which allows
    them to be reused if needed

    synqry manages the memory needed to return the result set and the used parameters, but
    the user does have influence on that. for more information see clear_params() and
    reset_data()

    at the latest synqry frees all allocated memory on destruction

    there is a manual and an automatic mode of managing the memory allocated for the
    returned result set. if autoreset_data() was set then each next(), prepare(), execute() call
    will first free the already allocated result data. otherwise it may be used up to the
    destruction of synqry object. (the default is true)

    synqry object copies the use_exc value of the parent object at construction time.
    use_exc tells wether to throw slt3::exc exceptions on errors. if they are not used
    then the return value of the given function tells that it is failed. the error message
    is sent to stderr.
    */
    class synqry
    {
      public:
        synqry(tran & t);
        ~synqry();

        /** @brief describes a column of the result set */
        struct colhead
        {
          enum {
            t_null,      ///<Null column
            t_integer,   ///<64 bit integer column
            t_double,    ///<standard 8 byte double precision column
            t_string,    ///<string column
            t_blob       ///<blob column
          };
          //
          int    type_;    ///<the column type: t_null,t_integer,t_double,t_string or t_blob
          char * name_;    ///<the column name as given in the SQL query
          char * table_;   ///<the table name
          char * db_;      ///<the db name
          char * origin_;  ///<the original column name
          //
          colhead() : type_(t_null), name_(0), table_(0), db_(0), origin_(0) {}

          void debug();
        };

        /** @brief used as a pointer vector to column data

            the underlying memory is managed by the synqry object */
        typedef common::pvlist< 32,colhead,common::nop_destructor<colhead> > columns_t;

        /** @brief field is used to return the field data

            the underlying memory is managed by the synqry object */
        struct field
        {
          union {
            long long         intval_;    ///<the 64 bit integer value
            double            doubleval_; ///<the 8 byte double precision value
            char *            stringval_; ///<the string value
            unsigned char *   blobval_;   ///<pointer to the blob data
          };
          unsigned int size_; ///<the size of the returned value
        };

        /** @brief used as a pointer vector to the returned fields

           the underlying memory is managed by the synqry object 
           see reset_data() for more information */
        typedef common::pvlist< 32,field,common::nop_destructor<field> > fields_t;

        /** @brief returns a reference to the given parameter
            @param pos tells what parameter should be returned
            @return a reference to the desired param
            @throw slt3::exc if pos > 2000

            parameters are allocated on demand and the allocated memory will be freed
            either at destruction or as a result of clear_params()

            set() and get() operations on the returned param reference may be used
            to change its value

            param references are indexed by number and the first index starts at 1 */
        param & get_param(unsigned int pos);

        /** @brief clears all allocated parameter data */
        void clear_params();

        /** @brief prepares a parametrized or iterative query
            @param sql is the query to be prepared
            @return true if successful
            @throw slt3::exc on error */
        bool prepare(const char * sql);

        /** @brief resets an iterative query
            @return true if successful
            @throw slt3::exc

            if schema changed or the database is busy reset may be neccessary
            to restart the query. there is no need to set parameters again. */
        bool reset();

        /** @brief frees memory allocated to the result set

            calling this function is only needed if autoreset_data() is set to false.
            if so the returned result set is kept until reset_data() or the ~synqry was
            called. */
        void reset_data();

        /** @brief sets automatic or manual data allocation mode
            @param yesno if true than automatic otherwise manual

            if autoreset_data was set then each prepare(), next(), execute() call
            will check and free the already allocated result set. otherwise these
            data will remain valid until object destruction.

           the default is true. */
        void autoreset_data(bool yesno);

        /** @brief returns the current setting of autoreset_data */
        bool autoreset_data();


        /** @brief step of iterative or parametrized query
            @param cols is used to return the result column data
            @param fields is used to return the current row
            @return true if more next() may be called
            @throw slt3::exc

            both cols and fields are managed by synqry. cols are kept valid
            until next prepare() call, while fields may or may not be reallocated
            based on the autoreset_data() value or a reset_data() call.

            if parameters are given or changed before or between the iterative query
            the next() call will update them as expected.

            all synqry allocated memory will be freed at destruction */
        bool next(columns_t & cols, fields_t & fields);


        /** @brief step of iterative or parametrized query
            @return true if more next() may be called
            @throw slt3::exc

            if parameters are given or changed before or between the iterative query
            the next() call will update them as expected.

            all synqry allocated memory will be freed at destruction */
        bool next();

        /** @brief returns the last insert rowid of the last insert query
            @return the insert id or -1 on error
            @throw slt3::exc */
        long long last_insert_id();

        /** @brief returns the number of changed rows of the last query
            @return the number or -1 on error 
            @throw slt3::exc */
        long long change_count();

        /** @brief executes a onestep query without returning any result
            @param sql is the query to be executed
            @return true if successful
            @throw slt3::exc
            @note that no parameters are used even if given before or the query itself is a
            parametrized query. for those queries use prepare() and next() */
        bool execute(const char * sql);

        /** @brief executes a onestep query with returning a single string
            @param sql is the query to be executed
            @return true if successful
            @throw slt3::exc
            @note that no parameters are used even if given before or the query itself is a
            parametrized query. for those queries use prepare() and next()

            if there are return values of the execute query, only the first column of the
            first row is returned in the result reference. for returning more than one row
            or column use the prepare(), next() interface instead. */
        bool execute(const char * sql, common::ustr & result);

        void debug();

        /** @brief Specifies whether synqry should throw slt3::exc exceptions
            @param yesno is the desired value to be set

            the default value for use_exc() is true, so it throws exceptions by default */
        void use_exc(bool yesno);

        /** @brief Returns the current value of use_exc
            @return true if exc exceptions are used */
        bool use_exc();

      private:
        /* types */
        struct impl;
        typedef std::auto_ptr<impl> impl_t;
        /* private data */
        friend class param;
        impl_t impl_;
        /* copying not allowed */
        synqry(const synqry & other);
        synqry & operator=(const synqry & other);
        /* no default construction */
        synqry();
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_slt3_synqry_hh_included_ */
