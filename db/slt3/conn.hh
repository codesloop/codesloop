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

#ifndef _csl_db_slt3_conn_hh_included_
#define _csl_db_slt3_conn_hh_included_

/**
   @file conn.hh
   @brief Contains slt3::conn which represents a sqlite3 connection

   The implementation is based on SQLite3
 */

#include "codesloop/common/ustr.hh"
#ifdef __cplusplus
#include <memory>

namespace csl
{
  namespace db
  {
    namespace slt3
    {
      class tran;

      /**
      @brief sqlite3 connection object

      The conn class represents a SQLite3 connection. This is not to be confused
      w/ a database. Connection is a main database and possibly attached databases.

      conn may throw slt3::exc exceptions, depending on the setting of use_exc

      if use_exc is true it throws exception on error, it it is false it sends
      an error message to stderr

      derived objects like slt3::tran inherits the current value of use_exc()

      to query the database you need a tran (transaction object) and a synqry (query object as well)
      */
      class conn
      {
        public:

          conn();
          ~conn();

          /**
          @brief Opens an SQLite3 (main) database
          @param db is the filename of the database
          @return true if successful
          @throw slt3::exc error description

          depending on the use_exc() value it may throw an slt3::exc exception

          if use_exc is true it throws otherwise it displays an error message on stderr
           */
          bool open(const char * db);

          /**
          @brief Returns the name used for opening the database
          @return the pathname of the database opened

          if the open() was not successful than name().empty() == true
           */
          const common::ustr & name() const;

          /**
          @brief Closes an SQLite3 connection
          @return true if successful
          @throw slt3::exc error description

          depending on the use_exc() value it may throw an slt3::exc exception

          if use_exc is true it throws otherwise it displays an error message on stderr
           */
          bool close();

          /**
          @brief returns the last insert rowid of the last insert query
          @return the insert id or -1 on error
          @throw slt3::exc
           */
          long long last_insert_id();

          /**
          @brief returns the number of changed rows of the last query
          @return the number or -1 on error
          @throw slt3::exc
           */
          long long change_count();

          /**
          @brief Specifies whether conn should throw slt3::exc exceptions
          @param yesno is the desired value to be set

          the default value for use_exc() is true, so it throws exceptions by default
           */
          void use_exc(bool yesno);

          /**
          @brief Returns the current value of use_exc
          @return true if exc exceptions are used
          */
          bool use_exc();

          /* types */
          struct impl;
          typedef std::auto_ptr<impl> impl_t;

        private:
          /* private data */
          friend class tran;
          impl_t impl_;
          /* copying not allowed */
          conn(const conn & other);
          conn & operator=(const conn & other);
      };
    } /* end of ns: csl::db::slt3 */
  } /* end of ns: csl::db */
} /* end of ns: csl */


#endif /* __cplusplus */
#endif /* _csl_db_slt3_conn_hh_included_ */
