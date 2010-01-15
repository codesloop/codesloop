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

#ifndef _csl_db_conn_hh_included_
#define _csl_db_conn_hh_included_

#include "codesloop/common/ustr.hh"
#include "codesloop/common/str.hh"
#include "codesloop/common/common.h"
#include "codesloop/common/logger.hh"
#include "codesloop/common/obj.hh"
#include "codesloop/db/driver.hh"
#ifdef __cplusplus
#include <memory>

namespace csl
{
  using common::str;
  using common::ustr;

  namespace db
  {
    /* only forwarding functionality, everything meaningful should be done
       in the driver */

    class conn
    {
      public:
        conn( int driver_type ) : driver_(driver::instance(driver_type))
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"conn(%d)",driver_type);
          LEAVE_FUNCTION();
        }

        bool open(const char * connect_string)
        {
          ENTER_FUNCTION();
          if(!connect_string)
          {
            CSL_DEBUGF(L"open(NULL) [ERROR]");
            RETURN_FUNCTION(false);
          }
          else
          {
            CSL_DEBUGF(L"open(%s)",connect_string);
            ustr tmp(connect_string);
            bool ret = this->open(tmp);
            CSL_DEBUGF(L"open(...) => %s",(ret==true?"TRUE":"FALSE"));
            RETURN_FUNCTION(ret);
          }
        }

        bool open(const ustr & connect_string)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"open(%s)",connect_string.c_str());
          bool ret = driver_->open(connect_string);
          CSL_DEBUGF(L"open(...) => %s",(ret==true?"TRUE":"FALSE"));
          RETURN_FUNCTION(ret);
        }

        bool open(const  str & connect_string)
        {
          ENTER_FUNCTION();
          ustr tmp(connect_string);
          CSL_DEBUGF(L"open(%s)",tmp.c_str());
          bool ret = this->open(tmp);
          CSL_DEBUGF(L"open(...) => %s",(ret==true?"TRUE":"FALSE"));
          RETURN_FUNCTION(ret);
        }

        bool open(const driver::connect_desc & info)
        {
          ENTER_FUNCTION();
          CSL_DEBUGF(L"open(info[host:%s port:%lld db:%s user:%s pass:%s])",
                     info.host_.c_str(),
                     info.port_.value(),
                     info.db_name_.c_str(),
                     info.user_.c_str(),
                     info.password_.c_str() );
          bool ret = driver_->open(info);
          CSL_DEBUGF(L"open(info[...]) => %s",(ret==true?"TRUE":"FALSE"));
          RETURN_FUNCTION(ret);
        }

        template <typename HOST,  typename PORT,  typename DB_NAME,
                  typename USER,  typename PASS>
        bool open(const HOST & h, const PORT & p, const DB_NAME & d,
                  const USER & u, const PASS & pa)
        {
          ENTER_FUNCTION();
          driver::connect_desc info;
          info.host_     = h;
          info.port_     = p;
          info.db_name_  = d;
          info.user_     = u;
          info.password_ = pa;
          CSL_DEBUGF(L"open(info[host:%s port:%lld db:%s user:%s pass:%s])",
                     info.host_.c_str(),
                     info.port_.value(),
                     info.db_name_.c_str(),
                     info.user_.c_str(),
                     info.password_.c_str() );
          bool ret = driver_->open(info);
          CSL_DEBUGF(L"open(info[...]) => %s",(ret==true?"TRUE":"FALSE"));
          RETURN_FUNCTION(ret);
        }

        uint64_t last_insert_id()
        {
          ENTER_FUNCTION();
          uint64_t ret = driver_->last_insert_id()
          CSL_DEBUGF(L"last_insert_id() => %lld",ret);
          RETURN_FUNCTION(ret);
        }

        uint64_t change_count()
        {
          ENTER_FUNCTION();
          uint64_t ret = driver_->change_count()
          CSL_DEBUGF(L"change_count() => %lld",ret);
          RETURN_FUNCTION(ret);
        }

        void reset_change_count()
        {
          ENTER_FUNCTION();
          driver_->reset_change_count();
          LEAVE_FUNCTION();
        }

        bool close()
        {
          ENTER_FUNCTION();
          bool ret = driver_->close();
          CSL_DEBUGF(L"close() => %s",(ret==true?"TRUE":"FALSE"));
          RETURN_FUNCTION(ret);
        }

        driver & get_driver()
        {
          ENTER_FUNCTION();
          RETURN_FUNCTION(*(driver_.get()));
        }

      private:
        /* no default construction */
        conn() {}

        /* no copy */
        conn(const conn & other) {}
        conn & operator=(const conn & other) { return *this; }

        /* variables */
        std::auto_ptr<driver> driver_;

        CSL_OBJ(csl::db,conn);
    };
  }; // end of ns:csl::db
}; // end of ns:csl

#endif // __cplusplus
#endif // _csl_db_conn_hh_included_
