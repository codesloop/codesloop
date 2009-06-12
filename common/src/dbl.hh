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

    /**
    @brief floating point variable class

    this class is part of the common::var family and implements all neccesary abstract
    functions that are mainly related to conversions to and from other variable classes.

    double values are represented by the internal double type
     */
    class dbl : public csl::common::var
    {
      private:
        double value_;

      public:
        inline dbl() : var(), value_(0.0) { }             ///<default constructor
        inline dbl(double v) : var(), value_(v) { }       ///<copy constructor
        virtual inline ~dbl() {}                          ///<destructor
        inline double value() const { return value_; }    ///<returns the long long value
        inline int var_type() { return CSL_TYPE_DOUBLE; } ///<value type helps distinguish from other var types
        inline void reset() { value_ = 0.0; }             ///<resets the internal value to 0.0

        /* conversions to other types */
        /**
        @brief convert to common::int64
        @param v is where to put the data
        @return true if successful

        this function delegates the conversion to common::int64 class which boils down
        to an internal cast and assignment
         */
        inline bool to_integer(int64 & v) const { return v.from_double(value_); }

        /**
        @brief convert to long long
        @param v is where to put the data
        @return true if successful

        convert using cast and assignment
         */
        inline bool to_integer(long long & v) const { v = (long long)value_; return true; }

        /**
        @brief convert to common::dbl
        @param v is where to put the data
        @return true if successful

        this function delegates the conversion to common::dbl class which boils down
        to a simple assignment
         */
        inline bool to_double(dbl & v) const { return v.from_double(value_); }

        /**
        @brief convert to double
        @param v is where to put the data
        @return true if successful

        no conversion is needed
         */
        inline bool to_double(double & v) const { v = value_; return true; }

        /**
        @brief convert to common::str
        @param v is where to put the data
        @return true if successful

        this function delegates the conversion to common::str class
         */
        bool to_string(str & v) const;

        /**
        @brief convert to common::ustr
        @param v is where to put the data
        @return true if successful

        this function delegates the conversion to common::ustr class
         */
        bool to_string(ustr & v) const;

        /**
        @brief convert to std::string
        @param v is where to put the data
        @return true if successful

        this function delegates the conversion to common::ustr class
         */
        bool to_string(std::string & v) const;

        /**
        @brief convert to common::binry
        @param v is where to put the data
        @return true if successful

        this function delegates the conversion to the other common::binry instance
         */
        bool to_binary(binry & v) const;

        /**
        @brief convert to (ptr+size)
        @param v is where to put the data
        @return true if successful

        this function delegates the conversion to the other common::binry instance.
        this function assumes that v has enough space. sz will be updated to indicate, how
        many bytes were stored.
         */
        bool to_binary(unsigned char * v, size_t & sz) const;

        /**
        @brief convert to (ptr+size)
        @param v is where to put the data
        @return true if successful

        this function delegates the conversion to the other common::binry instance.
        this function assumes that v has enough space. sz will be updated to indicate, how
        many bytes were stored.

         */
        bool to_binary(void * v, size_t & sz) const;

        /** @todo not implemented */
        bool to_xdr(xdrbuf & b) const;

        /**
        @brief polymorphic call to store the data into common::var type
        @param v is where to place the data
        @return true if successful

        this function delegates the conversion to v
         */
        inline bool to_var(var & v) const { return v.from_double(value_); }

        /* conversions from other types */
        /**
        @brief convert an common::int64
        @param v is the value to be read in
        @return true if successful

        this function delegates the conversion task to the common::int64 class
         */
        inline bool from_integer(const int64 & v) { return v.to_double( value_ ); }

        /**
        @brief convert a long long
        @param v is the value to be stored
        @return true if successful

        the conversion is done using internal cast and assignment
         */
        inline bool from_integer(long long v) { value_ = (double)v; return true; }

        /**
        @brief convert a common::dbl
        @param v is the value to be read in

        this function delegates the conversion task to the common::dbl class
         */
        inline bool from_double(const dbl & v) { return v.to_double(value_); }

        /**
        @brief convert a double
        @param v is the value to be stored
        @return true if successful

        no conversion is needed
         */
        inline bool from_double(double v) { value_ = v; return true; }

        /**
        @brief convert a common::str
        @param v is the value to be stored
        @return true if successful

        this function delegates the conversion to common::str class
         */
        bool from_string(const str & v);

        /**
        @brief convert a common::ustr
        @param v is the value to be stored
        @return true if successful

        this function delegates the conversion to common::ustr class
         */
        bool from_string(const ustr & v);

        /**
        @brief convert a std::string
        @param v is the value to be stored
        @return true if successful

        this function delegates the conversion to common::ustr class
         */
        bool from_string(const std::string & v);

        /**
        @brief convert a char* string
        @param v is the value to be stored
        @return true if successful

        this function delegates the conversion to common::ustr class
         */
        bool from_string(const char * v);

        /**
        @brief convert a wchar_t* string
        @param v is the value to be stored
        @return true if successful

        this function delegates the conversion to common::str class
         */
        bool from_string(const wchar_t * v);

        /**
        @brief convert a common::binry
        @param v is the value to be read in
        @return true if successful

        this function delegates the conversion task to the common::binry class
         */
        bool from_binary(const binry & v);

        /**
        @brief convert a memory region
        @param v is the pointer to the start of the memory region
        @param sz is the number of bytes to be read

        this function delegates the conversion task to the common::binry class
         */
        bool from_binary(const unsigned char * v,size_t sz);

        /**
        @brief convert a memory region
        @param v is the pointer to the start of the memory region
        @param sz is the number of bytes to be read

        this function delegates the conversion task to the common::binry class
         */
        bool from_binary(const void * v,size_t sz);

        /** @todo not implemented */
        bool from_xdr(xdrbuf & v);

        /**
        @brief polymorphic call to let v store its value in this buffer
        @param v is where to read the data from
        @return true if successful

        this function delegates the conversion to v
         */
        inline bool from_var(const var & v) { return v.to_double(value_); }
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_common_dbl_hh_included_ */
