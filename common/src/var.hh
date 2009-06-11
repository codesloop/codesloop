/*
Copyright (c) 2008,2009, Tamas Foldi, David Beck

Rediobjibution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Rediobjibutions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Rediobjibutions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the diobjibution.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, objICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _csl_common_var_hh_included_
#define _csl_common_var_hh_included_

/**
   @file var.hh
   @brief super class of all variable types
 */

#include "obj.hh"
#include "common.h"
#ifdef __cplusplus
#include <string>

namespace csl
{
  namespace common
  {
    class str;
    class ustr;
    class int64;
    class dbl;
    class binry;
    class xdrbuf;

    /**
    @brief abstract base class of all variable types

    this class defines the abstract functions to be implemented by descendant.
    these functions are mainly about conversion to and from various variable types.
    */
    class var : public obj
    {
      private:
        bool use_exc_; ///<use exceptions?

      public:
        /** @brief default constructor */
        inline var() : obj(), use_exc_(false) {}

        inline bool use_exc() const     { return use_exc_;  } ///<checks exception usage
        inline void use_exc(bool yesno) { use_exc_ = yesno; } ///<adjust exception usage

        virtual inline ~var() {} ///<destructor

        /**
        @brief returns the variable type

        subclasses should change this
         */
        inline virtual int var_type() { return CSL_TYPE_UNKNOWN; }

        /** @brief abstract function to reset the variable's internal state */
        virtual void reset() = 0;

        /* conversions to other types */

        /** @brief abstract function: convert to common::int64 */
        virtual bool to_integer(int64 & v) const = 0;

        /** @brief abstract function: convert to long long */
        virtual bool to_integer(long long & v) const = 0;

        /** @brief abstract function: convert to common::dbl */
        virtual bool to_double(dbl & v) const = 0;

        /** @brief abstract function: convert to double */
        virtual bool to_double(double & v) const = 0;

        /** @brief abstract function: convert to common::str */
        virtual bool to_string(str & v) const = 0;

        /** @brief abstract function: convert to common::ustr */
        virtual bool to_string(ustr & v) const = 0;

        /** @brief abstract function: convert to std::string */
        virtual bool to_string(std::string & v) const = 0;

        /** @brief abstract function: convert to common::binry */
        virtual bool to_binary(binry & v) const = 0;

        /** @brief abstract function: convert to (ptr+size) */
        virtual bool to_binary(unsigned char * v, size_t & sz) const = 0;

        /** @brief abstract function: convert to (ptr+size) */
        virtual bool to_binary(void * v, size_t & sz) const = 0;

        /** @brief abstract function: convert XDR */
        virtual bool to_xdr(xdrbuf & b) const = 0;

        /** @brief abstract function: convert to other variable */
        virtual bool to_var(var & v) const = 0;

        /* conversions from other types */

        /** @brief abstract function: convert from common::int64 */
        virtual bool from_integer(const int64 & v) = 0;

        /** @brief abstract function: convert from long long */
        virtual bool from_integer(long long v) = 0;

        /** @brief abstract function: convert from common::dbl */
        virtual bool from_double(const dbl & v) = 0;

        /** @brief abstract function: convert from double */
        virtual bool from_double(double v) = 0;

        /** @brief abstract function: convert from common::str */
        virtual bool from_string(const str & v) = 0;

        /** @brief abstract function: convert from common::ustr */
        virtual bool from_string(const ustr & v) = 0;

        /** @brief abstract function: convert from std::string */
        virtual bool from_string(const std::string & v) = 0;

        /** @brief abstract function: convert from char string */
        virtual bool from_string(const char * v) = 0;

        /** @brief abstract function: convert from wide char string */
        virtual bool from_string(const wchar_t * v) = 0;

        /** @brief abstract function: convert from common::binry */
        virtual bool from_binary(const binry & v) = 0;

        /** @brief abstract function: convert from (ptr+size) */
        virtual bool from_binary(const unsigned char * v,size_t sz) = 0;

        /** @brief abstract function: convert from (ptr+size) */
        virtual bool from_binary(const void * v,size_t sz) = 0;

        /** @brief abstract function: convert from XDR */
        virtual bool from_xdr(xdrbuf & v) = 0;

        /** @brief abstract function: convert from other variable */
        virtual bool from_var(const var & v) = 0;
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_common_var_hh_included_ */
