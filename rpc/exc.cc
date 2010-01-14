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

#include "codesloop/rpc/exc.hh"
#include "codesloop/common/str.hh"
#include "codesloop/common/common.h"

/**
  @file rpc/src/exc.cc
  @brief implementation of rpc::exc
 */

namespace csl
{
  namespace rpc
  {
    const wchar_t * exc::reason_string(int rc)
    {
      switch( rc )
      {
        case rs_invalid_param:   return L"Invalid parameter received.";
        case rs_not_implemented: return L"Function is not implemented.";
        case rs_invalid_handle:  return L"Invalid handle received";
        case rs_comm_err:        return L"Communication error";
        case rs_incompat_iface:  return L"Incompatible interfaces";
        case rs_srv_unknown_exc: return L"Unknown exception received from server side";
        case rs_invalid_fid:     return L"Internal error: requested function does not exist";
        case rs_unknown:
          default:               return L"Unknown reason";
      };
    }

    /* public interface */
  };
};

/* EOF */
