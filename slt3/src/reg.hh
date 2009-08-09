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

#ifndef _csl_slt3_reg_hh_included_
#define _csl_slt3_reg_hh_included_

/**
   @file reg.hh
   @brief slt3::reg is a database registry

   SQLite3 databases are either files or located in memory. slt3::reg helps locating 
   database files by registering their paths in a "registry" database. this centralized
   registry helps the ORM mapper.
 */

#include "pvlist.hh"
#include "conn.hh"
#include "mpool.hh"
#include "str.hh"
#include "ustr.hh"
#ifdef __cplusplus
#include <memory>

/** @brief SLT3_REGISTRY_PATH1 is the first where to check for database registry */
#ifndef SLT3_REGISTRY_PATH1
#define SLT3_REGISTRY_PATH1 "/var/db/csl/slt3/registry.db"
#endif /*SLT3_REGISTRY_PATH1*/

/** @brief SLT3_REGISTRY_PATH1 is the second where to check for database registry */
#ifndef SLT3_REGISTRY_PATH2
#define SLT3_REGISTRY_PATH2 "/etc/csl/slt3/registry.db"
#endif /*SLT3_REGISTRY_PATH2*/

/** @brief SLT3_REGISTRY_PATH1 is the third where to check for database registry */
#ifndef SLT3_REGISTRY_PATH3
#define SLT3_REGISTRY_PATH3 "./registry.db"
#endif /*SLT3_REGISTRY_PATH3*/

namespace csl
{
  namespace slt3
  {
    /**
    @brief slt3::reg is used for registering (SQLite3 DB,Path) pairs

    slt3::reg is a singleton that is dynamically allocated on request. 
     */
    class reg
    {
      public:
        /**
        @brief returns the singleton instance
        @return the registry instance (singleton)

        if an already initialized instance is available then it will be returned. if not then
        SLT3_REGISTRY_PATH1, SLT3_REGISTRY_PATH2 and SLT3_REGISTRY_PATH3 will be tried as database
        path for initialization
         */
        static reg & instance();

        /**
        @brief returns the singleton instance
        @param path is the database path to be used as the registry database
        @return the registry instance (singleton)

        if an already initialized instance is available then it will be returned. if not then
        path will be tried as database path for initialization.
         */
        static reg & instance(const char * path);

        /**
        @brief returns the singleton instance
        @param path is the database path to be used as the registry database
        @return the registry instance (singleton)

        if an already initialized instance is available then it will be returned. if not then
        path will be tried as database path for initialization.
         */
        static reg & instance(const common::ustr & path);

        typedef common::mpool<> pool_t;

        /**
        @brief reg::helper is to be used in slt3::obj derived classes to help ORM mapping

        reg::helper stores the database connection object along with the database path and database name
        that the ORM mapper will use.

        @li the name_ property tells what is the database name associated with the given class
        @li the path_ propery tells where to look for the database

        the (name,path) properties are associated with a given class. that database will store the
        serialized instances of the given class.

        if (name,path) pair cannot be located in the registry the helper will register in the registry.
        the catch here is that the registry database may be changed (off-line) when the application is
        not running, thus the database <-> class mapping is configurable without recompilation.
         */
        class helper
        {
          public:
            /**
            @brief initializing constructor
            @param name is used to locate class specific registry entry
            @param default_db_path is used when registry entry cannot be located and this will be registered

            the constructor initializes the internal variables but will not touch the registry. the actual database
            call will only happen when path() or db() was called.
             */
            helper(const char * name, const char * default_db_path);

            /** @brief access name property */
            inline const char * name() { return name_; }

            /**
            @brief returns the path associated with name()
            @return the path

            this call will check the registry for the path associated with the database name().
            if the required entry cannot be found in the database the it will register it with the default path
            given at construction time.
             */
            const char * path();

            /**
            @brief returns the database connection instance associated with the given class (name+path)
            @return the connection reference to be used

            this call will try to retrieve the database path from the registry. if not found then it will
            store the (name,default_db_path) pair in the registry and. finally it opens the database
            at the given database path and returns the connection refernce to that database.
             */
            conn & db();

            inline virtual ~helper() {} ///<destructor

            inline void use_exc(bool yesno) { use_exc_ = yesno; }  ///<sets exception usage
            inline bool use_exc() const     { return use_exc_; }  ///<should throw exceptions?

          private:
            helper() {}                                                    ///<default construction is not allowed
            helper(const helper & other) {}                               ///<copy constructor is not allowed
            helper & operator=(const helper & other) { return *this; } ///<copy operator is not allowed

            const char *   name_;              ///<the database name
            const char *   default_path_;      ///<the default database path (used when not registered)
            const char *   path_;              ///<the path as registered
            conn           conn_;               ///<the internal database connection handle
            bool           use_exc_;           ///<use exceptions?
            pool_t         pool_;               ///<memory pool

            CSL_OBJ(csl::slt3,reg::helper);
        };

        /** @brief item represents a database registry entry */
        struct item
        {
          long long       id_;
          char *          name_;
          char *          path_;
        };

        /** @brief this helper used for returning string lists */
        typedef common::pvlist< 64,char,common::nop_destructor<char> > strlist_t;

        /** @brief this helper used for returning list of items */
        typedef common::pvlist< 64,item,common::nop_destructor<item> > itemlist_t;

        /**
        @brief returns a database handle associated with 'name'
        @param name is used to locate the connection path
        @param c is the connection reference to be used
        @return true if successful
         */
        bool get( const char * name, conn & c );

        /**
        @brief returns a database handle associated with 'name'
        @param name is used to locate the connection path
        @param c is the connection reference to be used
        @return true if successful
         */
        bool get( const common::ustr & name, conn & c );

        /**
        @brief returns the registry item associated with'name'
        @param name is the name of the item to be returned
        @param i is the item reference to be filled
        @param pool is a client supplied pool to be used for memory allocation
        @return true if successful
         */
        bool get( const char * name, item & i, pool_t & pool );

        /**
        @brief returns the registry item associated with'name'
        @param name is the name of the item to be returned
        @param i is the item reference to be filled
        @param pool is a client supplied pool to be used for memory allocation
        @return true if successful
         */
        bool get( const common::ustr & name, item & i, pool_t & pool );

        /**
        @brief stores it (item) in the registry database
        @param it is the item to be stored
        @return true if successful
        */
        bool set( const item & it ); 

        /**
        @brief returns all names in the registry
        @param nms is the list of names
        @param pool is a user supplied memory pool to be used for memory pooling
        @return true if successful
         */
        bool names( strlist_t & nms, pool_t & pool );

        /**
        @brief returns all items in the registry
        @param itms is the list of items
        @param pool is a user supplied memory pool to be used for memory pooling
        @return true if successful
         */
        bool dbs( itemlist_t & itms, pool_t & pool );

        /* inline functions */
        inline const char * path() const           { return path_.c_str(); } ///<access path propery
        inline void path(const char * p)           { path_ = p; }             ///<sets path propery
        inline void path(const common::ustr & p)   { path_ = p; }             ///<sets path propery

        inline void use_exc(bool yesno) { use_exc_ = yesno; }  ///<sets exception usage
        inline bool use_exc() const     { return use_exc_; }  ///<should throw exceptions?

        inline ~reg() { } ///<destructor

      private:
        /* no default construction or copy */

        reg() { }                                              ///<default construction is not allowed
        reg & operator=(const reg & x) { return *this; }   ///<copy construction is not allowed
        reg(const reg & x) { }                                ///<copy operator is not allowed

        /* variables */
        common::ustr  path_;     ///<path propery
        bool          use_exc_;  ///<use exceptions?

        /* singleton instance */
        static std::auto_ptr<reg> instance_;  ///<static singleton instance

        CSL_OBJ(csl::slt3,reg);
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_slt3_reg_hh_included_ */
