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
 
#ifndef _csl_rpc_iface_hh_included_
#define _csl_rpc_iface_hh_included_

#include "common.h"
#ifdef __cplusplus
#include <string>
#include <vector>
#include "obj.hh"
#include "csrgen.hh"

namespace csl 
{ 
  namespace rpc 
  {

    struct param {
      std::string type;
      std::string name;
      bool is_array;
      bool array_size;
    };

    struct func {
      bool disposable;
      std::vector<param> params;
    };

    class iface : public csl::common::obj
    {
      void set_name( token_info &  );
      void set_version( token_info & );
      void set_namespc( token_info &);

      void add_function( token_info & );
      void set_param_type( token_info &);
      void set_param_name( token_info &);

      void add_include(token_info &);

      std::string to_string() const;

    protected:
      std::string name_;
      std::string version_;
      std::string namespc_;

      std::vector<func> functions_;
      std::vector<std::string> includes_;

    private:
      std::string token_to_string( token_info & ) const;
    };


  }
}

#endif /* __cplusplus */
#endif /* _csl_rpc_iface_hh_included_ */
