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

#ifndef _csl_common_var_hh_included_
#define _csl_common_var_hh_included_

/**
   @file var.hh
   @brief super class of all variable types
 */

#include "codesloop/common/obj.hh"
#include "codesloop/common/common.h"
#include "codesloop/common/serializable.hh"
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
    class var : public obj, public serializable
    {
      public:
        /** @brief default constructor */
        inline var() : obj() { }

        virtual inline ~var() {} ///<destructor

        enum { var_type_v =  CSL_TYPE_UNKNOWN };

        /**
        @brief returns the variable type

        subclasses should change this
         */
        inline virtual int var_type() const { return var_type_v; }

        /** @brief abstract function to reset the variable's internal state */
        virtual void reset() = 0;

        /* conversions to other types */

        /** @brief abstract function: convert to common::int64 */
        virtual bool to_integer(int64 & v) const = 0;

        /** @brief abstract function: convert to int64_t */
        virtual bool to_integer(int64_t & v) const = 0;

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
        virtual bool to_binary(unsigned char * v, uint64_t & sz) const = 0;

        /** @brief abstract function: convert to (ptr+size) */
        virtual bool to_binary(void * v, uint64_t & sz) const = 0;

        /** @brief abstract function: convert XDR */
        virtual bool to_xdr(xdrbuf & b) const = 0;

        /** @brief abstract function: convert to other variable */
        virtual bool to_var(var & v) const = 0;

        /* conversions from other types */

        /** @brief abstract function: convert from common::int64 */
        virtual bool from_integer(const int64 & v) = 0;

        /** @brief abstract function: convert from int64_t */
        virtual bool from_integer(int64_t v) = 0;

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
        virtual bool from_binary(const unsigned char * v,uint64_t sz) = 0;

        /** @brief abstract function: convert from (ptr+size) */
        virtual bool from_binary(const void * v,uint64_t sz) = 0;

        /** @brief abstract function: convert from XDR */
        virtual bool from_xdr(xdrbuf & v) = 0;

        /** @brief abstract function: convert from other variable */
        virtual bool from_var(const var & v) = 0;

        /** @brief returns the size of the variable data */
        virtual uint64_t var_size() const = 0;

        /** @brief returns a const pointer to internal data */
        virtual const unsigned char * ucharp_data() const = 0;

        /** @brief convert variable to signed 64 bit integer using built in conversion */
        virtual inline operator int64_t () const
        {
         int64_t ret = 0;
         this->to_integer(ret);
         return ret;
        }

        /** @brief convert variable to double precision floating point value using built in conversion */
        virtual inline operator double () const
        {
         double ret = 0.0;
         this->to_double(ret);
         return ret;
        }

        /** @brief convert variable from signed 64 bit integer using polymorphic conversion */
        virtual inline void operator=(int64_t v)
        {
         this->from_integer(v);
        }

        /** @brief convert variable from double precision floating point value using polymorphic conversion */
        virtual inline void operator=(double v)
        {
         this->from_double(v);
        }

        /** @brief returns a const pointer to internal data */
        virtual const char * charp_data() const { return reinterpret_cast<const char *>(this->ucharp_data()); };

        /** @brief initialize from C string */
        virtual inline bool set(const char * v) { return (this->from_string(v)); }

        /** @brief initialize from wchar_t string */
        virtual inline bool set(const wchar_t * v) { return (this->from_string(v)); }

        /** @brief initialize from int64_t value */
        virtual inline bool set(int64_t v) { return (this->from_integer(v)); }

        /** @brief initialize from double value */
        virtual inline bool set(double v) { return (this->from_double(v)); }

        /** @brief initialize from binary buffer */
        virtual inline bool set(const unsigned char * ptr, uint64_t sz) { return (this->from_binary(ptr,sz)); }

        /** @brief returns the value as double */
        virtual inline double get_double() const { return static_cast<double>(*this); }

        /** @brief returns the value as int64_t */
        virtual inline int64_t get_long() const { return static_cast<int64_t>(*this); }

        /** @brief converts and copies the internal value to int64_t */
        virtual inline bool get(int64_t & v) { return (this->to_integer(v)); }

        /** @brief converts and copies the internal value to double */
        virtual inline bool get(double & v) { return (this->to_double(v)); }

        /** @brief converts and copies the internal value to string */
        virtual inline bool get(ustr & v) { return (this->to_string(v)); }

        /** @brief converts and copies the internal value to string */
        virtual inline bool get(str & v) { return (this->to_string(v)); }

        /** @brief converts and copies the internal value to binary */
        virtual inline bool get(binry & v) { return (this->to_binary(v)); }
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_common_var_hh_included_ */
