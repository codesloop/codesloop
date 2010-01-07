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

#ifndef _csl_db_conn_hh_included_
#define _csl_db_conn_hh_included_

#include "codesloop/common/ustr.hh"
#include "codesloop/common/str.hh"
#include "codesloop/common/common.h"
#include "codesloop/db/driver.hh"
#ifdef __cplusplus
#include <memory>

namespace csl
{
  using common::str;
  using common::ustr;

  namespace db
  {
    /* only forwarding functionality, everythin meaningful should be done
       in the driver */

    class conn
    {
      public:
        conn( int driver_type ) : driver_(driver::instance(driver_type)) { }

        bool open(const char * connect_string)
        {
          if(!connect_string) return false;
          else
          {
            ustr tmp(connect_string);
            return this->open(tmp);
          }
        }

        bool open(const ustr & connect_string)
        {
          return driver_->open(connect_string);
        }

        bool open(const  str & connect_string)
        {
          ustr tmp(connect_string);
          return this->open(tmp);
        }

        uint64_t last_insert_id() { return driver_->last_insert_id(); }
        uint64_t change_count()   { return driver_->change_count();   }
        void reset_change_count() { driver_->reset_change_count();    }
        bool close()              { return driver_->close();          }

      private:
        /* no default construction */
        conn() {}

        /* no copy */
        conn(const conn & other) {}
        conn & operator=(const conn & other) { return *this; }

        /* variables */
        std::auto_ptr<driver> driver_;
    };
  }; // end of ns:csl::db
}; // end of ns:csl

#endif // __cplusplus
#endif // _csl_db_conn_hh_included_
