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
#include "common.h"
#include "str.hh"

/**
  @file csl_sec/src/exc.cc
  @brief implementation of sec::exc
 */

namespace csl
{
  namespace sec
  {
    const wchar_t * exc::reason_string(int rc)
    {
      switch( rc )
      {
        case rs_salt_size:        return L"Invalid salt size.";
        case rs_null_key:         return L"Null key received.";
        case rs_too_big:          return L"Data too big.";
        case rs_rand_failed:      return L"Cannot generate random.";
        case rs_header_size:      return L"Invalid header size.";
        case rs_footer_size:      return L"Invalid footer size.";
        case rs_null_data:        return L"Data too short for encryption.";
        case rs_cksum:            return L"Checksum mismatch.";
        case rs_unknown:
          default:               return L"Unknown reason";
      };
    }

    const wchar_t * exc::component_string(int cm)
    {
      switch( cm )
      {
        case cm_crypt_pkt: return L"sec::crypt_pkt";
        case cm_unknown:
          default:         return L"unknown component";
      };
    }

    void exc::to_string(common::str & res)
    {
      common::str t(L"Exception");
      if( file_.size() > 0 && line_ > 0 )
      {
        wchar_t tx[200];
        SWPRINTF(tx,199,L"(%ls:%d): ",file_.c_str(),line_);
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

    exc::exc() : reason_(rs_unknown), component_(cm_unknown) {}
    exc::~exc() {}
    /* public interface */
  };
};

/* EOF */
