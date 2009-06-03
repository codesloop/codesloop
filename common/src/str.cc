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

#include "str.hh"
#include "common.h"

/**
  @file csl_common/src/str.cc
  @brief implementation of simple string class
 */

namespace csl
{
  namespace common
  {
    str& str::operator+=(const str& s)
    {
      // remove trailing NULL character
      buf_.allocate( size() * sizeof(wchar_t) ); 

      // append new string 
      buf_.append( s.buffer() );

      return *this;
    }

    str str::substr(const size_t start, const size_t length) const
    {
      str s;
      size_t len = length; 
      wchar_t t = 0;

      if ( start > size() )
        throw exc(exc::rs_invalid_param,exc::cm_str,"out of range");

      // shrink length to fit in
      if ( size() < length + start )
        len = size() - start;

      // copy string
      s.buf_.set( (unsigned char *)(data() + start), (len) * sizeof(wchar_t) );
      // terminate string
      s.buf_.append( (unsigned char *)&t, sizeof(wchar_t) );

      return s;
    }

    /* public interface */
  };
};

/* EOF */
