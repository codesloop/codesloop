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

        class helper
        {
          public:
            helper(const char * default_db_name, const char * default_db_path);

            const char * name() { return name_.c_str(); }
            const char * path();
            conn & db();

            virtual ~helper() {}

          private:
            helper() {}
            helper(const helper & other) {}
            helper & operator=(const helper & other) { return *this; }

            common::ustr name_;
            common::ustr default_path_;
            common::ustr path_;
            conn conn_;
        };

        struct item
        {
          long long       id_;
          char *       name_;
          char *       path_;
        };

        typedef common::pvlist< 64,char,common::nop_destructor<char> > strlist_t;
        typedef common::pvlist< 64,item,common::nop_destructor<item> > itemlist_t;
        typedef common::mpool<> pool_t;

        bool get( const char * name, conn & c );
        bool get( const common::ustr & name, conn & c );
        bool get( const char * name, item & i, pool_t & pool );
        bool get( const common::ustr & name, item & i, pool_t & pool );

        bool set( const item & it );

        bool names( strlist_t & nms, pool_t & pool );
        bool dbs( itemlist_t & itms, pool_t & pool );

        /* inline functions */
        const common::ustr & path() const { return path_; }
        void path(const common::ustr & p) { path_ = p; }

      private:
        /* no default construction or copy */
        reg() { }
        inline ~reg() { }

        inline reg & operator=(const reg & x) { return *this; }
        inline reg(const reg & x) { }

        /* variables */
        common::ustr  path_;
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_slt3_reg_hh_included_ */
