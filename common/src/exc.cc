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

#include "exc.hh"
#include "str.hh"
#include "common.h"

/**
  @file csl_common/src/exc.cc
  @brief implementation of common::exc
 */

namespace csl
{
  namespace common
  {
    const wchar_t * exc::reason_string(int rc)
    {
      switch( rc )
      {
        case rs_invalid_param:   return L"Invalid parameter received";
        case rs_cannot_append:   return L"Cannot append data to pbuf";
        case rs_cannot_get:      return L"Cannot get data from pbuf";
        case rs_xdr_eof:         return L"End of XDR data";
        case rs_xdr_invalid:     return L"Invalid XDR data";
        case rs_empty:           return L"Empty container.";
        case rs_unknown:
          default:               return L"Unknown reason";
      };
    }

    const wchar_t * exc::component_string(int cm)
    {
      switch( cm )
      {
        case cm_pbuf:      return L"common::pbuf";
        case cm_zfile:     return L"common::zfile";
        case cm_xdrbuf:    return L"common::xdrbuf";
        case cm_circbuf:   return L"common::circbuf";
        case cm_logger:    return L"common::logger";
        case cm_unknown:
          default:         return L"unknown component";
      };
    }

    void exc::to_string(str & res)
    {
      str t(L"Exception");
      if( file_.size() > 0 && line_ > 0 )
      {
        wchar_t tx[200];
        SNPRINTF(tx,199,L"(%sl:%d): ",file_.c_str(),line_);
        t += tx;
      }
      t += L" [";
      t += component_string(component_);
      t += L"] [";
      t += reason_string(reason_);
      t += L"] ";
      if( text_.size() > 0 ) t+= text_;
      res = t;
    }

    str exc::to_string() 
    {
      str ret;
      to_string(ret);
      return ret;
    }

    exc::exc() : reason_(rs_unknown), component_(cm_unknown) {}
    exc::~exc() {}
    /* public interface */
  };
};

/* EOF */
