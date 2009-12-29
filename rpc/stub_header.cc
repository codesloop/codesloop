/*
Copyright (c) 2008,2009, CodeSLoop Team

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

#include "codesloop/rpc/stub_header.hh"
#include "codesloop/common/common.h"
#include "codesloop/common/ustr.hh"
#include "codesloop/rpc/csrparser.hh"

#include <iostream>
#include <fstream>

using csl::common::ustr;
using std::endl;

/**
  @file rpc/src/stub_header.cc
  @brief implementation of codesloop interface descriptor
 */

namespace csl
{
  namespace rpc
  {
    void stub_header::generate()
    {
      // add internal functions for client and server stub
      add_internal_functions();    

      open_file((ifname_+"_srv.hh").c_str());
      write_file(STUB_SERVER);

      open_file((ifname_+"_cli.hh").c_str());
      write_file(STUB_CLIENT);
    }

    void stub_header::write_file(stub_kind kind)
    {   
      const char * class_name, * parent_class;
      const char * srv_cli;

      if ( kind == STUB_SERVER ) {
        class_name = (ifname_ + "_srv").c_str();
        parent_class = (std::string("csl::rpc::srv_trans_") + ifc_->get_transport() ).c_str();
        srv_cli = "srv";
      } else {
        class_name = (ifname_ + "_cli").c_str();
        parent_class = (std::string("csl::rpc::cli_trans_") + ifc_->get_transport() ).c_str();
        srv_cli = "cli";
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

      output_
        << "#include <codesloop/rpc/" << srv_cli << "_trans_" << ifc_->get_transport() << ".hh>" 
        << endl
      ;

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
        << ", "<< ifname_.c_str() << "_" << srv_cli << ");" << endl
        << endl 
      ;
      ls_ += "  ";
      
      // constructor
      output_ 
        << ls_ << class_name << "() : " << parent_class 
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
        << ls_ << "inline static const int64_t get_crc64() {" << endl
        << ls_ << "  return " 
        << ustr( ifc_->to_string().c_str() ).crc64().value()         
        << "LL;" << endl
        << ls_ << "}" << endl << endl
      ;

      /*---------------------------------------------------------\
      |  Generate function IDs enumeration                       |
      \---------------------------------------------------------*/
      iface::function_iterator func_it = ifc_->get_functions()->begin();
      int func_seq = 1000;

      output_ 
        << ls_ << "enum function_ids { " << endl
      ;
      for ( ; func_it != ifc_->get_functions()->end() ; func_it++ )      
      {
        if ( (*func_it).name == "ping" )
          continue;

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
        if ( kind == STUB_SERVER && (*func_it).name == "ping" ) 
        {
          func_it++;
          continue;
        }
        /* synchronous call */
        output_ 
          << ls_ << "virtual void " << (*func_it).name 
          << " (" << endl
        ;
        this->generate_func_params( (*func_it).name, kind, false);
        // make server definitions pure virtual
        if ( kind == STUB_SERVER ) {
          output_ << " = 0";
        }
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
      |  Specialities                                            |
      \---------------------------------------------------------*/
      ls_ = ls_.substr( 0, ls_.size() - 2 );
      output_ << ls_ << "protected:" << endl;
      ls_ += "  ";

      if ( kind == STUB_SERVER ) 
      {
        output_ << ls_ << "virtual void despatch(" << endl;
        output_ << ls_ << "  /* input */  const csl::rpc::client_info & ci," << endl;
        output_ << ls_ << "  /* inout */  csl::common::arch & archive" << endl;
        output_ << ls_ << ");" << endl;
      } else if ( kind == STUB_CLIENT)  {
        output_ << ls_ << "virtual void decode_response(" << endl;
        output_ << ls_ << "  /* input */  const csl::rpc::handle & __handle," << endl;
        output_ << ls_ << "  /* input */  const uint32_t function_id," << endl;
        output_ << ls_ << "  /* inout */  csl::common::arch & archive" << endl;
        output_ << ls_ << ");" << endl;
      }

      /*---------------------------------------------------------\
      |  Cleanup                                                 |
      \---------------------------------------------------------*/
      ls_ = ls_.substr( 0, ls_.size() - 2 );
      output_ << ls_ << "}; /* class */" << endl << endl;

      generate_ns_close();

      output_
        << endl
        << "#endif /* __cplusplus */" << endl
        << "#endif /* __csl_interface_" << ifc_->get_name().c_str() << "*/" << endl
        << "/* EOF */" << endl
      ;

    }
  };
};

/* EOF */
