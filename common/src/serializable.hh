/*
Copyright (c) 2008,2009, Tamas Foldi

Rediserializableibution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Rediserializableibutions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Rediserializableibutions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the diserializableibution.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, serializableICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _csl_common_serializable_hh_included_
#define _csl_common_serializable_hh_included_

/**
   @file serializable.hh
   @brief super class of all csl serializables
   @todo document me
 */

#include <xdrbuf.hh>
#ifdef __cplusplus

namespace csl
{
  namespace common
  {
    /** @brief superclass of all codesloop serializableects */
    class serializable
    {
      virtual void serialize(xdrbuf & buf) = 0;
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_common_serializable_hh_included_ */
