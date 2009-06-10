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

#ifndef _csl_slt3_param_hh_included_
#define _csl_slt3_param_hh_included_

/**
   @file param.hh
   @brief parameter class for slt3 queries
 */

#include "tbuf.hh"
#include "synqry.hh"
#include "str.hh"
#include "ustr.hh"
#ifdef __cplusplus
#include <memory>
#include <vector>

namespace csl
{
  namespace slt3
  {
    /**
    @brief param is a convenience class for sqlite3 parametrized queries

    parametrized queries provide convenient way for doing parametrized queries.
    these saves lots of string manipulation and also automatically escapes the
    parameter values. for using blob values it is even better. blobs can easily given
    as a parameter without converting or escaping them. blobs may be given as parameters
    to insert or select queries.

    the memory allocation of the parameter values are provided by synqry. for more information
    on that see synqry::get_param() and synqry::clear_params().
    */
    class param
    {
      public:
        /** @brief blob values may be passed as unsigned char vectors */
        typedef common::binry blob_t;

        /** @brief returns the param type
            @return the type as in synqry::colhead

            types are set as a result of set() calls */
        int get_type() const;

        /** @brief return the parameter value as a 64 bit long integer */
        long long get_long() const;

        /** @brief return the parameter value as a double precision value */
        double get_double() const;

        /** @brief returns true if not set */
        bool is_empty();

        /** @brief return the parameter value as a 64 bit long integer */
        bool get(long long & val) const;

        /** @brief return the parameter value as a double precision value */
        bool get(double & val) const;

        /** @brief return the parameter value as a string object */
        bool get(common::ustr & val) const;

        /** @brief return the parameter value as a string object */
        bool get(common::str & val) const;

        /** @brief return the parameter value as a vector of unsigned characters */
        bool get(blob_t & val) const;

        /** @brief sets the value as a 64 bit long integer */
        void set(long long val);

        /** @brief sets the value as a double precision value */
        void set(double val);

        /** @brief sets the value as a string */
        void set(const common::ustr & val);

        /** @brief sets the value as a string */
        void set(const common::str & val);

        /** @brief sets the value as a string */
        void set(const wchar_t * val);

        /** @brief sets the value as a string */
        void set(const char * val);

        /** @brief sets the value as a vector of unsigned characters */
        void set(const blob_t & val);

        /** @brief sets the value as a vector of unsigned characters */
        void set(const unsigned char * ptr,unsigned int size);

        void debug();

        /** @brief Specifies whether param should throw slt3::exc exceptions
        @param yesno is the desired value to be set

        the default value for use_exc() is true, so it throws exceptions by default */
        void use_exc(bool yesno);

        /** @brief Returns the current value of use_exc
        @return true if exc exceptions are used */
        bool use_exc();
        ~param();

      private:
        /* types */
        struct impl;
        typedef std::auto_ptr<impl> impl_t;
        /* private data */
        friend class synqry;
        friend class asynqry;
        impl_t impl_;

        /* copying not allowed */
        param(const param & other);
        param & operator=(const param & other);

        /* no default construction */
        param();
        param(synqry::impl & sq);
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_slt3_param_hh_included_ */
