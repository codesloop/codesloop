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

#ifndef _csl_common_preallocated_array_hh_included_
#define _csl_common_preallocated_array_hh_included_


#include "codesloop/common/pbuf.hh"
#include "codesloop/common/hlprs.hh"
#include "codesloop/common/obj.hh"
#include "codesloop/common/common.h"
#ifdef __cplusplus

namespace csl
{
  namespace common
  {
    template <typename T, size_t SZ> class preallocated_array
    {
      public:
        static const size_t  preallocated_size_ = SZ;
        static const size_t  item_size_         = sizeof(T);

        ~preallocated_array();
        preallocated_array();
        explicit preallocated_array(const T & c);
        preallocated_array(const preallocated_array & other);
        explicit preallocated_array(const T * other);
       
        bool operator==(const preallocated_array & other) const;
        
        preallocated_array & operator=(const char * other);
        preallocated_array & operator=(const pbuf & other);
        preallocated_array & operator=(const preallocated_array & other);

        void reset();
        bool get(T * dta) const;

        bool set(const T * dta, size_t sz);
        T * allocate(size_t sz);
        T * allocate_nocopy(size_t sz);
        
        void append(const T & c);
        bool append(const uint8_t * dta, size_t sz);
        bool append(const preallocated_array & other);
        void set_at(size_t pos,const T & c);

        /* inline functions */
        inline bool is_empty() const        { return (size_ == 0); }
        inline bool has_data() const        { return (size_ > 0); }
        inline bool is_static() const       { return (data_ == preallocated_); }

        inline size_t size() const      { return size_; }
        inline const T * data() const   { return data_; }
        inline T * private_data() const { return data_; }

      private:
        T            preallocated_[SZ];
        T *          data_;
        size_t       size_;
    };
  }
}

#endif /* __cplusplus */
#include "codesloop/common/preallocated_array_impl.hh"
#endif /* _csl_common_preallocated_array_hh_included_ */
