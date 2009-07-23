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
      open_file((ifname_+"_cli.hh").c_str());
      write_file(STUB_CLIENT);

      open_file((ifname_+"_srv.hh").c_str());
      write_file(STUB_SERVER);
    }

    void stub_header::write_file(stub_kind kind)
    {   
      const char * class_name, * parent_class;
      if ( kind == STUB_SERVER ) {
        class_name = (ifname_ + "_srv").c_str();
        parent_class = "rpc_server";
      } else {
        class_name = (ifname_ + "_cli").c_str();
        parent_class = "rpc_client";
      }

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

      if ( kind == STUB_SERVER ) 
      {
        output_ 
         << "#ifdef __if_" << ifname_.c_str() << "_client" << endl
         << "#error \"both client and server headers are included\"" << endl
         << "#endif" << endl
         << "#define  __if_" << ifname_.c_str()  << "_server"
         << endl << endl
        ;
      } else {
        output_ 
         << "#ifdef __if_" << ifname_.c_str() << "_server" << endl
         << "#error both client and server headers are included" << endl
         << "#endif" << endl
         << "#define  __if_" << ifname_.c_str()  << "_client"
         << endl << endl;
      }

      if ( ifc_->get_includes()->size() != 0 )
        output_ << "/* User defined includes */" << endl;

      while ( inc_it != ifc_->get_includes()->end() )
      {
        output_ << "#include " << *inc_it << endl; 
        inc_it++;
      }
      output_ << endl;

      // TODO: CSL transport and rpc includes comes here
      //
      
      /*---------------------------------------------------------\
      |  Generate namespace info and class begin                 |
      \---------------------------------------------------------*/
      
      this->generate_ns_open();
      output_ 
        << ls_ << "class " << class_name << " : public " 
        << parent_class << endl
        << ls_ << "{" << endl
        << ls_ << "public:" << endl
        << ls_ << "  CSL_OBJ(" << ifc_->get_namespc().c_str() 
        << ", "<< ifname_.c_str() << "_cli);" << endl
        << endl 
      ;
      ls_ += "  ";
      
      // constructor
      output_ 
        << ls_ << class_name << " : " << parent_class 
        << "() {}" << endl
        << endl
      ;

      // version
      output_
        << ls_ << "static const char * get_version() {" << endl
        << ls_ << "  return " 
        << "\"" << ifc_->get_version() << "\";" << endl
        << ls_ << "}" << endl << endl
      ;
      // crc
      output_
        << ls_ << "static const long long get_crc64() {" << endl
        << ls_ << "  return " 
        << ustr( ifc_->to_string().c_str() ).crc64().value()         
        << "LL;" << endl
        << ls_ << "}" << endl << endl
      ;

      /*---------------------------------------------------------\
      |  Generate function IDs enumeration                       |
      \---------------------------------------------------------*/
      iface::function_iterator func_it = ifc_->get_functions()->begin();
      int func_seq = 0;

      output_ 
        << ls_ << "enum function_ids { " << endl
      ;
      for ( ; func_it != ifc_->get_functions()->end() ; func_it++ )      
      {
        output_
          << ls_ << "  fid_" << (*func_it).name 
          << " = " << func_seq++
          << "," << endl
        ;
      }
      output_ << ls_ << "};" << endl << endl;

      /*---------------------------------------------------------\
      |  Generate function specs                                 |
      \---------------------------------------------------------*/
      func_it = ifc_->get_functions()->begin();      

      while ( func_it != ifc_->get_functions()->end() )
      {
        /* synchronous call */
        output_ 
          << ls_ << "virtual void " << (*func_it).name 
          << " (" << endl
        ;
        this->generate_func_params( (*func_it).name, kind, false);
        output_ << ";" << endl;

        /* asynchronous call */
        if ( kind == STUB_CLIENT ) {
          output_ << ls_ << "virtual void " << (*func_it).name 
            << " (" << endl
          ;
          this->generate_func_params( (*func_it).name, kind, true);
          output_ << ";" << endl;
        }

        output_ << endl << endl;

        func_it++;
      }

      /*---------------------------------------------------------\
      |  Cleanup                                                 |
      \---------------------------------------------------------*/
      //ls_ = ls_.substr( 0, ls_.size() - 2 );
      generate_ns_close();

      output_
        << endl
        << "#ifndef /* __csl_interface_" << ifc_->get_name().c_str() << "*/" 
        << endl
        << "#endif /* __cplusplus */" << endl
        << "/* EOF */" << endl
      ;

    }
  };
};

/* EOF */
