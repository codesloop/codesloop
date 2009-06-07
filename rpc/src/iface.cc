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

#include "iface.hh"
#include "common.h"

/**
  @file csl_rpc/src/iface.cc
  @brief implementation of codesloop interface descriptor
 */

namespace csl
{
  namespace rpc
  {
    void iface::set_name( token_info &  ti )
    {
      name_ = token_to_string(ti);
    }

    void iface::set_version( token_info & ti )
    {
      version_ = token_to_string(ti);
    }

    void iface::set_namespc( token_info & ti )
    {
      namespc_ = token_to_string(ti);
    }

    void iface::add_include(token_info & ti)
    {
      includes_.push_back( token_to_string(ti) );
    }

    std::string iface::to_string() const
    {
      std::string ret;

      ret  = "Interface: " + name_ + "\n";
      ret += "\tversion:\t" + version_ + "\n";
      ret += "\tnamespace:\t" + namespc_ + "\n";

      ret += "Includes:\n";
      for ( size_t i = 0; i < includes_.size() ; i++ )
        ret += "\t" + includes_[i] + "\n";

      return ret;
    }

    std::string iface::token_to_string( token_info & ti ) const
    {
      return std::string( ti.p, ti.p-ti.ts );
    }
  };
};

/* EOF */
