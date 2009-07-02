/*
Copyright (c) 2008,2009, Tamas Foldi, David Beck

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

#ifndef _csl_common_obj_hh_included_
#define _csl_common_obj_hh_included_

/**
   @file obj.hh
   @brief super class of all csl objects
   @todo document me
 */

#include "common.h"
#ifdef __cplusplus

namespace csl
{
  namespace common
  {
    /** @brief superclass of all codesloop objects */
    class obj
    {
      public:
        obj();                    ///<constructor
        virtual inline ~obj() {}  ///<destructor

        inline bool use_exc() const     { return use_exc_;  } ///<checks exception usage
        inline void use_exc(bool yesno) { use_exc_ = yesno; } ///<adjust exception usage

        /** @brief implemented by CSL_OBJECT_DECL macro */
        virtual const char * get_class_name();

        /** @brief returns true when the objects have the same type */
        virtual bool is_kind_of( obj & o );

      protected:
        int logger_flags_; ///< enable or disable per instance logging

      private:
        bool use_exc_; ///<use exceptions?
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_common_obj_hh_included_ */
