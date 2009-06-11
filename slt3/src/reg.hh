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

#ifndef _csl_slt3_reg_hh_included_
#define _csl_slt3_reg_hh_included_

/**
   @file db.hh
   @brief db @todo db
 */

#include "pvlist.hh"
#include "conn.hh"
#include "mpool.hh"
#include "str.hh"
#include "ustr.hh"
#ifdef __cplusplus

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
    /** @todo document me */
    class reg
    {
      public:
        static reg & instance();
        static reg & instance(const char * path);
        static reg & instance(const common::ustr & path);

        typedef common::mpool<> pool_t;

        class helper
        {
          public:
            helper(const char * default_db_name, const char * default_db_path);

            const char * name() { return name_; }
            const char * path();
            conn & db();

            virtual ~helper() {}

            inline void use_exc(bool yesno) { use_exc_ = yesno; }
            inline bool use_exc() const     { return use_exc_; }

          private:
            helper() {}
            helper(const helper & other) {}
            helper & operator=(const helper & other) { return *this; }

            const char *   name_;
            const char *   default_path_;
            const char *   path_;
            conn           conn_;
            bool           use_exc_;
            pool_t         pool_;
        };

        struct item
        {
          long long       id_;
          char *          name_;
          char *          path_;
        };

        typedef common::pvlist< 64,char,common::nop_destructor<char> > strlist_t;
        typedef common::pvlist< 64,item,common::nop_destructor<item> > itemlist_t;

        bool get( const char * name, conn & c );
        bool get( const common::ustr & name, conn & c );
        bool get( const char * name, item & i, pool_t & pool );
        bool get( const common::ustr & name, item & i, pool_t & pool );

        bool set( const item & it );

        bool names( strlist_t & nms, pool_t & pool );
        bool dbs( itemlist_t & itms, pool_t & pool );

        /* inline functions */
        const char * path() const           { return path_.c_str(); }
        void path(const char * p)           { path_ = p; }
        void path(const common::ustr & p)   { path_ = p; }

        inline void use_exc(bool yesno) { use_exc_ = yesno; }
        inline bool use_exc() const     { return use_exc_; }

      private:
        /* no default construction or copy */
        reg() { }
        inline ~reg() { }

        inline reg & operator=(const reg & x) { return *this; }
        inline reg(const reg & x) { }

        /* variables */
        common::ustr  path_;
        bool          use_exc_;
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_slt3_reg_hh_included_ */
