/*
Copyright (c) 2008,2009, David Beck, Tamas Foldi

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials_ provided with the distribution.

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

#include "stub_header.hh"
#include "common.h"
#include "ustr.hh"
#include "csrparser.hh"

#include <iostream>
#include <fstream>

using csl::common::ustr;
using std::endl;

/**
  @file csl_rpc/src/stub_header.cc
  @brief implementation of codesloop interface descriptor
 */

namespace csl
{
  namespace rpc
  {
    void stub_header::generate()
    {   

      output_
        << "#ifndef __csl_interface_" << ifc_->get_name().c_str() << endl
        << "#define __csl_interface_" << ifc_->get_name().c_str() << endl 
        << "#ifdef __cplusplus" << endl
        << endl
      ;

      /*---------------------------------------------------------\
      |  Generate includes                                       |
      \---------------------------------------------------------*/
      iface::include_iterator inc_it = ifc_->get_includes()->begin() ;

      if ( ifc_->get_includes()->size() != 0 )
        output_ << "/* User defined includes */" << endl;

      while ( inc_it != ifc_->get_includes()->end() )
      {
        output_ << "#include " << *inc_it << endl; 
        inc_it++;
      }
      output_ << endl;

      // TODO: CSL transport and rpc includes comes here

      output_ 
        << "#define __if_" << ifname_.c_str()  << "_ver\t" 
        << "\"" << ifc_->get_version() << "\""
        << endl;
      output_ 
        << "#define __if_" << ifname_.c_str()  << "_crc\t" 
        << ustr( ifc_->to_string().c_str() ).crc64().value()
        << "LL"
        << endl << endl;
            
      /*---------------------------------------------------------\
      |  Generate namespace info                                 |
      \---------------------------------------------------------*/
      
      // TODO

      /*---------------------------------------------------------\
      |  Generate function specs                                 |
      \---------------------------------------------------------*/
      iface::function_iterator func_it = ifc_->get_functions()->begin();
      int func_seq = 0;

      while ( func_it != ifc_->get_functions()->end() )
      {
        output_ 
          << ls_ << "#define __func_"  << ifname_.c_str()  << "_"
          << (*func_it).name << "_id " << func_seq++ << endl;

        output_ << ls_ << "void " << (*func_it).name << " (" << endl;

        this->generate_func_params( (*func_it).name );

        output_ << ";" << endl << endl << endl;

        func_it++;
      }


      // TODO

      /*---------------------------------------------------------\
      |  Cleanup                                                 |
      \---------------------------------------------------------*/
      output_
        << endl
        << "#ifndef /* __csl_interface_" << ifc_->get_name().c_str() << "*/" 
        << endl
        << "#endif /* __cplusplus */" << endl
        << "/* EOF */" << endl
      ;

      output_.close();
    }
  };
};

/* EOF */
