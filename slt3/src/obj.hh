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

#ifndef _csl_slt3_obj_hh_included_
#define _csl_slt3_obj_hh_included_

/**
  @file obj.hh
  @brief slt3 obj
  @todo document me
 */

#include "var.hh"
#include "conn.hh"
#include "tran.hh"
#include "sql.hh"
#ifdef __cplusplus

namespace csl
{
  namespace slt3
  {
    /** @todo document me */
    class obj
    {
      public:
        inline obj() : changed_(false), use_exc_(true) {}
        virtual ~obj() {}

        /* abstract functions */
        virtual conn & db() = 0;
        virtual sql::helper & sql_helper() const = 0;
        virtual void set_id(long long id);

        virtual bool init(tran & t);
        virtual bool create(tran & t);
        virtual bool save(tran & t);
        virtual bool remove(tran & t);
        virtual bool find_by_id(tran & t);
        virtual bool find_by(tran & t,
                             int field1,
                             int field2=-1,
                             int field3=-1,
                             int field4=-1,
                             int field5=-1);

        virtual bool init();
        virtual bool create();
        virtual bool save();
        virtual bool remove();
        virtual bool find_by_id();
        virtual bool find_by(int field1,
                             int field2=-1,
                             int field3=-1,
                             int field4=-1,
                             int field5=-1);

        virtual void on_load() {}
        virtual void on_change() { changed_ = true; }

        virtual const wchar_t * table_name() const { return sql_helper().table_name(); }
        virtual var::helper & var_helper() { return var_helper_; }

        bool changed() { return changed_; }
        void changed(bool c) { changed_ = c; }

        /** @brief Specifies whether param should throw sched::exc exceptions
        @param yesno is the desired value to be set

        the default value for use_exc() is true, so it throws exceptions by default */
        inline void use_exc(bool yesno) { use_exc_ = yesno; }

        /** @brief Returns the current value of use_exc
        @return true if exc exceptions are used */
        inline bool use_exc() const { return use_exc_; }
      private:
        var::helper var_helper_;
        bool changed_;
        bool use_exc_;
        inline obj(const obj & other) {}
        inline obj & operator=(const obj & other) { return *this; }
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_slt3_obj_hh_included_ */
