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

#ifndef _csl_common_int64_hh_included_
#define _csl_common_int64_hh_included_

/**
   @file int64.hh
   @brief 64 bit signed integer type to be used in interfaces
 */

#include "var.hh"
#ifdef __cplusplus

namespace csl
{
  namespace common
  {
    class str;
    class ustr;
    class dbl;
    class binry;
    class xdrbuf;

    /**
    @brief 64 bit signed integer variable class

    this class is part of the common::var family and implements all neccesary abstract
    functions that are mainly related to conversions to and from other variable classes.

    integers are represented by the internal long long type
     */
    class int64 : public csl::common::var
    {
      private:
        long long value_;

      public:
        typedef long long value_t;
        enum { var_type_v =  CSL_TYPE_INT64 }; 
      
        inline int64() : var(), value_(0) { }             ///<default constructor
        inline int64(long long v) : var(), value_(v) { }  ///<copy constructor
        virtual inline ~int64() {}                        ///<destructor
        inline value_t value() const { return value_; } ///< returns the long long value
        inline int var_type() const { return var_type_v; }   ///<value type helps distinguish from other var types
        inline void reset() { value_ = 0LL; }             ///<resets the internal value to 0LL

        /** @brief copy operator */
        inline int64 & operator=(const int64 & other)
        {
          value_ = other.value_;
          return *this;
        }

        /** @brief returns a const pointer to internal data */
        inline operator const unsigned char *() const { return (unsigned char *)(&value_); }
        
        /** @brief returns the size of the variable data */
        inline size_t var_size() const { return sizeof(value_); }

        /* conversions to other types */
        /**
        @brief convert to common::int64
        @param v is where to put the data
        @return true if successful

        this function delegates the conversion to common::int64 class which boils down to
        a single assingment
         */
        inline bool to_integer(int64 & v) const { return v.from_integer(value_); }

        /**
        @brief convert to long long
        @param v is where to put the data
        @return true if successful

        no conversion is needed
         */
        inline bool to_integer(long long & v) const { v = value_; return true; }

        /**
        @brief convert to common::dbl
        @param v is where to put the data
        @return true if successful

        this function delegates the conversion to common::dbl class which boils down to
        a cast and an assignment
         */
        bool to_double(dbl & v) const;

        /**
        @brief convert to double
        @param v is where to put the data
        @return true if successful

        conversion is done using cast and assignment
         */
        inline bool to_double(double & v) const { v = (double)value_; return true; }

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

        /**
        @brief stores the content of this instance to an XDR stream
        @param b is the XDR stream to be used
        @return true if successful

        note that usual XDR conversion will apply. the high 32 bit of the integer is stored
        first and the low 32 bit will be stored next.
         */
        bool to_xdr(xdrbuf & b) const;

        /**
        @brief polymorphic call to store the data into common::var type
        @param v is where to place the data
        @return true if successful

        this function delegates the conversion to v
         */
        inline bool to_var(var & v) const { return v.from_integer(value_); }

        /* conversions from other types */
        /**
        @brief convert an common::int64
        @param v is the value to be read in
        @return true if successful

        this function delegates the conversion task to the common::int64 class
         */
        inline bool from_integer(const int64 & v) { return v.to_integer(value_); }

        /**
        @brief convert a long long
        @param v is the value to be stored
        @return true if successful

        no conversion is needed
         */
        inline bool from_integer(long long v) { value_ = v; return true; }

        /**
        @brief convert a common::dbl
        @param v is the value to be read in

        this function delegates the conversion task to the common::dbl class
         */
        bool from_double(const dbl & v);

        /**
        @brief convert a double
        @param v is the value to be stored
        @return true if successful

        conversion is done w/ cast and assignment
         */
        inline bool from_double(double v) { value_ = (long long)v; return true; }

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

        /**
        @brief read the content of this instance from an XDR stream
        @param b is the XDR stream to be used
        @return true if successful

        note that usual XDR conversion will apply. the high 32 bit of the integer is stored
        first and the low 32 bit will be stored next.
         */
        bool from_xdr(xdrbuf & v);

        /**
        @brief polymorphic call to let v store its value in this buffer
        @param v is where to read the data from
        @return true if successful

        this function delegates the conversion to v
         */
        inline bool from_var(const var & v) { return v.to_integer(value_); }
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_common_int64_hh_included_ */
