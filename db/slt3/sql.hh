/*
Copyright (c) 2008,2009,2010, CodeSLoop Team

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

#ifndef _csl_db_sql_hh_included_
#define _csl_db_sql_hh_included_

/**
  @file sql.hh
  @brief sql helper to be used to generate ORM related SQL queries
 */

#include "codesloop/common/pvlist.hh"
#include "codesloop/common/mpool.hh"
#include "codesloop/common/tbuf.hh"
#include "codesloop/common/var.hh"
#include "codesloop/common/ustr.hh"
#ifdef __cplusplus

namespace csl
{
  namespace db
  {
    namespace slt3
    {
      class obj;

      /** @brief only a container of the sql::helper class */
      class sql
      {
        public:

          /** @brief sql::helper is used for generating ORM related SQL queries */
          class helper
          {
            public:

              /** @brief helper::data contains descriptions of ORM fields */
              struct data
              {
                const char * name_;  ///<the column name
                const char * type_;  ///<the column type (INTEGER,BLOB,TEXT,etc...)
                const char * flags_; ///<misc flags like (AUTOINCREMENT, UNIQUE, PRIMARY KEY, etc...)

                /** @brief initializing constructor */
                data(const char * name, const char * typ,const char * flags)
                  : name_(name), type_(typ), flags_(flags) {}
              };

              typedef common::ustr buf_t;
              typedef common::pvlist< 32,data,common::delete_destructor<data> > fieldlist_t;

              inline const char * table_name() { return table_name_; }

              /**
              @brief initializing constructor
              @param tablename is the database table name associated with the class
               */
              helper(const char * tablename);

              /**
              @brief registers a database field with the SQL helper
              @param name is the column name
              @param typ is the column type like (INTEGER, BLOB, REAL, etc...)
              @param flags is the database column flags like (PRIMARY KEY, DEFAULT value, UNIQUE, etc...)

              registers the database field within the internal structure of the SQL helper
               */
              bool add_field(const char * name,const char * typ, const char * flags="");

              const char * init_sql();     ///<returns: CREATE TABLE...
              const char * create_sql();   ///<returns: INSERT INTO...
              const char * save_sql();     ///<returns: UPDATE ....
              const char * remove_sql();   ///<returns: DELETE FROM ...
              const char * find_by_id_sql();
              const char * find_by(int field1,
                                      int field2=-1,
                                      int field3=-1,
                                      int field4=-1,
                                      int field5=-1); ///<returns: SELECT... WHERE ...

              inline void use_exc(bool yesno) { use_exc_ = yesno; }  ///<sets exception usage
              inline bool use_exc() const     { return use_exc_; }  ///<should throw exceptions?

            private:
              helper() {} ///<destructor

              const char * table_name_;
              bool         done_;
              fieldlist_t  fields_;

              /**/
              buf_t  init_sql_;
              buf_t  create_sql_;
              buf_t  save_sql_;
              buf_t  remove_sql_;
              buf_t  find_by_id_sql_;
              buf_t  find_by_sql_;
              int    find_by_fields_[5];
              bool   use_exc_;
          };
      };
    }; /* end of csl::db::slt3 namespace */
  }; /* end of csl::db namespace */
}; /* end of csl namespace */

#endif /* __cplusplus */
#endif /* _csl_db_sql_hh_included_ */
