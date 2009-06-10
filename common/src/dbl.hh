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

#ifndef _csl_common_dbl_hh_included_
#define _csl_common_dbl_hh_included_

/**
   @file dbl.hh
   @brief double precision floating point type to be used in interfaces
   @todo document me
 */

#include "var.hh"
#include "int64.hh"
#ifdef __cplusplus

namespace csl
{
  namespace common
  {
    class str;
    class ustr;
    class binry;
    class xdrbuf;

    /** @todo document me */
    class dbl : public csl::common::var
    {
      private:
        double value_;

      public:
        inline dbl() : var(), value_(0.0) { }
        inline dbl(double v) : var(), value_(v) { }

        virtual inline ~dbl() {}

        inline double value() const { return value_; }

        inline int var_type() { return CSL_TYPE_DOUBLE; }

        inline void reset() { value_ = 0.0; }

        /* conversions to other types */
        inline bool to_integer(int64 & v)     const { return v.from_double(value_); }
        inline bool to_integer(long long & v) const { v = (long long)value_; return true; }

        inline bool to_double(dbl & v)    const { return v.from_double(value_); }
        inline bool to_double(double & v) const { v = value_; return true; }

        bool to_string(str & v) const;
        bool to_string(ustr & v) const;
        bool to_string(std::string & v) const;

        bool to_binary(binry & v) const;
        bool to_binary(unsigned char * v, size_t & sz) const;
        bool to_binary(void * v, size_t & sz) const;

        bool to_xdr(xdrbuf & b) const;
        inline bool to_var(var & v) const { return v.from_double(value_); }

        /* conversions from other types */
        inline bool from_integer(const int64 & v)  { return v.to_double( value_ ); }
        inline bool from_integer(long long v)      { value_ = (double)v; return true; }

        inline bool from_double(const dbl & v) { return v.to_double(value_); }
        inline bool from_double(double v)      { value_ = v; return true; }

        bool from_string(const str & v);
        bool from_string(const ustr & v);
        bool from_string(const std::string & v);
        bool from_string(const char * v);
        bool from_string(const wchar_t * v);

        bool from_binary(const binry & v);
        bool from_binary(const unsigned char * v,size_t sz);
        bool from_binary(const void * v,size_t sz);

        bool from_xdr(xdrbuf & v);
        inline bool from_var(const var & v) { return v.to_double(value_); }
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_common_dbl_hh_included_ */
