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

#ifndef _csl_common_arch_hh_included_
#define _csl_common_arch_hh_included_

/**
   @file arch.hh
   @brief XDR helper to reduce XDR dependecies
*/

#include "codesloop/common/common.h"
#include "codesloop/common/obj.hh"
#include "codesloop/common/xdrbuf.hh"
#ifdef __cplusplus
#include <utility>

namespace csl
{
  namespace common
  {
    class var;
    class pbuf;
    class xdrbuf;

    /**
    @brief arch de/serializes data to a pbuf in XDR format using xdrbuf

    Allows you to save a complex network of objects in a permanent binary 
    form (usually disk storage) that persists after those objects are deleted. 
    */
    class arch : public obj
    {
      CSL_OBJ(csl::common,arch);
      public:
        /** @brief specifies arch object's behaviour. 
         
         if serialize specified in arch constructor, then
         the serialize function serializes incoming data, otherwise 
         deserializes
        */
        enum direction {
          SERIALIZE,    ///< serialize function serializes          
          DESERIALIZE   ///< serialize function deserializes
        };

        /** 
         @brief constructor with default direction 
         @param d default direction (serialization / deserialization )
         @throw common::exc
        */
        arch( direction d );
        virtual ~arch(); 

        /**
          @brief serialize or deserialize val to xdrbuf
          @param val is the value to be de/serialized
          @throw common::exc               

          based on object's direction the function loads
          or stores val from/to buffer
        */
        template <typename T> void serialize(T & val)
        {       
          if ( direction_ == SERIALIZE )
            (*xdrbuf_) << val;
          else
            (*xdrbuf_) >> val;
        }

        /**
          @brief return the size of the serialized data
        */
        unsigned int size() const;
        /**
          @brief return serialized data buffer in pbuf
        */
        pbuf * get_pbuf() const;
        /** 
          @brief sets input buffer for deserialization
        */
        void set_pbuf( const pbuf & src );
        
      private:
        pbuf * pbuf_;
        xdrbuf * xdrbuf_;
        direction direction_;
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_common_arch_hh_included_ */
