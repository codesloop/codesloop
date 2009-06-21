/*
Copyright (c) 2008,2009, David Beck, Tamas Foldi

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

/**
  @file sched/src/exc.cc
  @brief implementation of sched::exc
 */

namespace csl
{
  namespace sched
  {
    const wchar_t * exc::reason_string(int rc)
    {
      switch( rc )
      {
        case rs_database_error:  return L"Database error";
        case rs_invalid_param:   return L"Invalid parameter received";
        case rs_insufficient:    return L"Insufficient data";
        case rs_xdr:             return L"XDR conversion error";
        case rs_private_fun:     return L"This function is private";
        case rs_internal:        return L"Internal error";
        case rs_unknown:
          default:               return L"Unknown reason";
      };
    }

    const wchar_t * exc::component_string(int cm)
    {
      switch( cm )
      {
        case cm_cfg:           return L"sched::cfg";
        case cm_data:          return L"sched::data";
        case cm_head:          return L"sched::head";
        case cm_item:          return L"sched::item";
        case cm_peer:          return L"sched::peer";
        case cm_schedule:      return L"sched::schedule";
        case cm_item_state:    return L"sched::item_state";
        case cm_remote_peer:   return L"sched::remote_peer";
        case cm_route:         return L"sched::route";

        case cm_unknown:
          default:             return L"unknown component";
      };
    }

    void exc::to_string(common::str & res)
    {
      common::str t("Exception");
      if( file_.size() > 0 && line_ > 0 )
      {
        wchar_t tx[200];
        SWPRINTF(tx,199,L"(%s:%d): ",file_.c_str(),line_);
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
