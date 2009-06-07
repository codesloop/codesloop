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
    void iface::set_name(const token_info & ti)
    {
      name_ = token_to_string(ti);
    }

    void iface::set_version(const token_info & ti)
    {
      version_ = token_to_string(ti);
    }

    void iface::set_namespc(const token_info & ti)
    {
      namespc_ = token_to_string(ti);
    }

    void iface::add_include(const token_info & ti)
    {
      includes_.push_back( token_to_string(ti) );
    }

    void iface::add_function(const token_info & ti)
    {
      struct func f;

      if ( ti.type == TT_DISPOSABLE_FUNCTION )
        f.disposable = true;
      else
        f.disposable = false;

      f.name = token_to_string(ti);
      functions_.push_back(f);
    }

    void iface::set_param_type(const token_info & ti)
    {
      param_type_ = token_to_string(ti);
    }

    void iface::set_param_name(const token_info & ti)
    {
      struct param p;

      p.name = token_to_string(ti);
      p.type = param_type_;
      p.kind = ti.modifier;
      p.array_length = 0;
      p.is_array = false;

      functions_[functions_.size()-1].params.push_back(p);
    }

    void iface::set_arry_len(int size) 
    {
      size_t idx_func = functions_.size() - 1;
      size_t idx_param = functions_[idx_func].params.size() - 1;

      functions_[idx_func].params[idx_param].array_length = size;
      functions_[idx_func].params[idx_param].is_array = true;
    }

    std::string iface::to_string() const
    {
      std::string ret;
      size_t i;

      ret  = "Interface: " + name_ + "\n";
      ret += "\tversion:\t" + version_ + "\n";
      ret += "\tnamespace:\t" + namespc_ + "\n";

      ret += "Includes:\n";
      for ( i = 0; i < includes_.size() ; i++ )
        ret += "\t#include " + includes_[i] + "\n";

      ret += "Functions:\n";
      for ( i = 0; i < functions_.size() ; i++ )
      {
        ret += "\tfunction: " + functions_[i].name + 
          (functions_[i].disposable ? " disposable\n" : "\n");
        for (size_t j = 0; j < functions_[i].params.size(); j++ ) 
        {
          ret += "\t\ttype: " + functions_[i].params[j].type + " (";
          ret += param_kind_name[functions_[i].params[j].kind];
          ret += ")\n";

          ret += "\t\tname: " + functions_[i].params[j].name;
          if ( functions_[i].params[j].array_length > 0 )
            ret += " (static sized array)";
          else if ( functions_[i].params[j].is_array == true )
            ret += " (dynamic sized array)";

          ret += "\n\n";
        }
      }

      return ret;
    }

    std::string iface::token_to_string(const token_info & ti) const
    {
      return std::string( ti.ts, ti.p-ti.ts );
    }
  };
};

/* EOF */
