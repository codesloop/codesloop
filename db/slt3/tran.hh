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

#ifndef _csl_db_slt3_tran_hh_included_
#define _csl_db_slt3_tran_hh_included_

/**
   @file tran.hh
   @brief SQLite3 database transactions

   The transaction object is used to enforce transaction usage and also to provide nested
   transaction semantics.
 */

#ifdef __cplusplus
#include <memory>

namespace csl
{
  namespace db
  {
    namespace slt3
    {
      class conn;

      /**
      @brief sqlite3 transaction object

      the pupose of this class is to enforce transaction usage and also help managing them.
      it supports nested transactions that are implemented as SQLite3 savepoints.

      transaction objects may automatically commit or rollback transactions at destruction.
      the default is to commit started transaction on destruction. This may be changed.

      tran object is used with synqry objects to start user specified queries.

      tran object copies the use_exc value of the parent object at construction time
      use_exc tells wether to throw slt3::exc exceptions on errors. if they are not used
      then the return value of the given function tells that it is failed. the error message
      is sent to stderr.
       */
      class tran
      {
      public:
        /**
        @brief this constructor starts a main transaction
        @param c is the database connection
          */
        tran(conn & c);

        /**
        @brief this constructor starts a nested transaction of t
        @param t is the parent transaction
          */
        tran(tran & t);

        ~tran();

        /**
        @brief sets the destructor behaviour
        @param yesno tells wether to commit on destruction
          */
        void commit_on_destruct(bool yesno=true);

        /**
        @brief sets the destructor behaviour
        @param yesno tells wether to rollback on destruction
          */
        void rollback_on_destruct(bool yesno=true);

        /** @brief commit the current transaction */
        void commit();

        /** @brief rollback the current transaction */
        void rollback();

        /**
        @brief Specifies whether tran should throw slt3::exc exceptions
        @param yesno is the desired value to be set

        the default value for use_exc() is true, so it throws exceptions by default
         */
        void use_exc(bool yesno);

        /**
        @brief Returns the current value of use_exc
        @return true if exc exceptions are used
         */
        bool use_exc();

        /* private implementation hidden in impl */
        struct impl;
        typedef std::auto_ptr<impl> impl_t;

      private:

        friend class query;
        /* private data */
        impl_t impl_;

        /* copying not allowed */
        tran & operator=(const tran & other);

        /* transactions should only be created in conn, or tran context */
        tran();
      };
    } /* end of ns: csl::db::slt3 */
  } /* end of ns: csl::db */
} /* end of ns: csl */

#endif /* __cplusplus */
#endif /* _csl_db_slt3_tran_hh_included_ */
