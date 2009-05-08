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

#ifndef _csl_slt3_sql_hh_included_
#define _csl_slt3_sql_hh_included_

/**
  @file sql.hh
  @brief sqhelpr
  @todo document me
 */

#include "pvlist.hh"
#include "mpool.hh"
#include "tbuf.hh"
#include "var.hh"
#ifdef __cplusplus

namespace csl
{
  namespace slt3
  {
    class obj;

    /** @todo document me */
    class sql
    {
      public:
        /** @todo document me */
        class helper
        {
          public:
            struct data
            {
              const char * name_;
              const char * type_;
              const char * flags_;

              data(const char * name, const char * typ,const char * flags)
                : name_(name), type_(typ), flags_(flags) {}
            };

            typedef common::tbuf<256> buf_t;
            typedef common::pvlist< 32,data,common::delete_destructor<data> > fieldlist_t;
            typedef common::mpool<> strpool_t;

            inline const char * table_name() { return table_name_; }

            helper(const char * tablename);
            bool add_field(const char * name,const char * typ, const char * flags="");

            const char * init_sql();
            const char * create_sql();
            const char * save_sql();
            const char * remove_sql();
            const char * find_by_id_sql();
            const char * find_by(int field1,
                                 int field2=-1,
                                 int field3=-1,
                                 int field4=-1,
                                 int field5=-1);

          private:
            helper() {}

            const char * table_name_;
            bool         done_;
            fieldlist_t  fields_;
            //strpool_t    pool_;

            /**/
            buf_t init_sql_;
            buf_t create_sql_;
            buf_t save_sql_;
            buf_t remove_sql_;
            buf_t find_by_id_sql_;
            buf_t find_by_sql_;
            int   find_by_fields_[5];
        };
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_slt3_sql_hh_included_ */
