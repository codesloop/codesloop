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

#include "cexc.hh"
#include "str.hh"
#include "common.h"

/**
  @file common/src/cexc.cc
  @brief implementation of common::cexc
 */

namespace csl
{
  namespace common
  {
    void cexc::to_string(str & res)
    {
      str t(L"exception");
      if( file_.size() > 0 && line_ > 0 )
      {
        wchar_t tx[200];
        SWPRINTF(tx,199,L"(%ls:%d): ",file_.c_str(),line_);
        t += tx;
      }
      t += L" [";
      t += component_;    
      t += L"] [";
      t += reason_string(reason_);
      t += L"] ";
      if( text_.size() > 0 ) t+= text_;
      res = t;
    }

    str cexc::to_string() 
    {
      str ret;
      to_string(ret);
      return ret;
    }

    const wchar_t * cexc::reason_string(int rc)
    {
        return L"unknown";
    }


    cexc::cexc() : reason_(0), component_(L"unknown") {}
    cexc::~cexc() {}
    /* public interface */
  };
};

/* EOF */
