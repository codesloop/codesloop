/*
Copyright (c) 2008,2009, CodeSLoop Team

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

#include "codesloop/rpc/stub_client.hh"
#include "codesloop/common/common.h"
#include "codesloop/common/ustr.hh"
#include "codesloop/rpc/iface.hh"


/**
  @file rpc/src/stub_client.cc
  @brief implementation of codesloop interface descriptor
 */

using std::endl;
using csl::common::ustr;
using csl::rpc::iface;

namespace csl
{
  namespace rpc
  {
    void stub_client::generate()
    {
      const char * class_name = (ifname_ + "_cli").c_str();

      open_file((ifname_+"_cli.cc").c_str());

//      add_internal_functions();

      output_
        << "#ifdef __cplusplus" << endl
        << "#include <sys/time.h>"
        << endl
        << "#include \"" << (ifname_+"_cli.hh").c_str() << "\""  
        << endl
        << "#include <codesloop/common/arch.hh>"        
        << endl
        ;

      /*---------------------------------------------------------\
      |  Generate namespace info                                 |
      \---------------------------------------------------------*/
      this->generate_ns_open();


      /*---------------------------------------------------------\
      |  Generate function bodies                                 |
      \---------------------------------------------------------*/
      iface::function_iterator func_it = ifc_->get_functions()->begin();
      iface::func::param_iterator param_it;

      while ( func_it != ifc_->get_functions()->end() )
      {
        /* synchronous call */
        output_ 
          << ls_ << "void " << class_name << "::" 
          << (*func_it).name << " (" << endl
        ;
        this->generate_func_params( (*func_it).name, STUB_CLIENT, false);
        output_ 
          << endl 
          << ls_ << "{" << endl
          << ls_ << "  csl::rpc::handle __handle;" << endl
          << ls_ << "  this->create_handle(__handle);" << endl
          << endl
        ;

      /*---------------------------------------------------------\
      |  Synchronous function                                    |
      \---------------------------------------------------------*/        
        output_ 
          << ls_ << "  " << (*func_it).name << "("  << endl
          << ls_ << "    " << "__handle," << endl
        ;

        param_it = (*func_it).params.begin();
        while( param_it != (*func_it).params.end() ) 
        {
          /* ignore exceptions */
          if( (*param_it).kind==MD_EXCEPTION) {
            param_it++;
            continue;
          }

          output_ 
            << ls_ << "    " << (*param_it).name 
          ;          
          param_it++;

          if ( (*param_it).kind!=MD_EXCEPTION && param_it != (*func_it).params.end() )          
            output_ << ",";
          output_ << endl;
        }
        output_
          << ls_ << "  );" 
          << endl << endl 
          << ls_ << "  this->wait( __handle);" << endl
          << endl
          << ls_ << "}" << endl
          << endl
        ;

      /*---------------------------------------------------------\
      |  Function implementation on client side stub             |
      \---------------------------------------------------------*/        
        output_ 
          << ls_ << "void " << class_name << "::" 
          << (*func_it).name << " (" << endl
        ;
        this->generate_func_params( (*func_it).name, STUB_CLIENT, true);
        output_
          << endl
          << ls_ << "{"
          << endl << endl
        ;

        // serializer
        output_
          << ls_ << "  static int64_t interface_id = " <<  ustr( ifc_->to_string().c_str() ).crc64().value()
          << "LL;"<< endl
          << ls_ << "  static uint32_t function_id = fid_" << (*func_it).name << ";"<< endl
          << ls_ << "  ptr_ivec_t * opp = new ptr_ivec_t();" << endl
          
        ;

        if ( (*func_it).name == "ping" )
          generate_ping_body();

        output_
          << ls_ << "  csl::common::arch archiver(csl::common::arch::SERIALIZE);" << endl
          << endl
          << ls_ << "  archiver.serialize(interface_id); " << endl
          << ls_ << "  archiver.serialize(function_id);" << endl        
          << ls_ << "  archiver.serialize(__handle);" << endl        
        ;

        param_it = (*func_it).params.begin();
        while( param_it != (*func_it).params.end() )
        {
          /* ignore exceptions */
          if( (*param_it).kind==MD_EXCEPTION ) 
          {
            param_it++;
            continue;
          }
          
          /* parameter types which requrire output ptr registration */
          if ( (*param_it).kind==MD_OUTPUT || (*param_it).kind==MD_INOUT ) 
          {
            output_ 
              << ls_ << "  opp->push_back( " 
              << (((*param_it).kind==MD_INOUT) ? "&" : "") // inout types needs refernce op 
              << (*param_it).name << ");" << endl
            ;
          } 

          if ( (*param_it).kind!=MD_OUTPUT) 
          {
            output_
              << ls_ << "  archiver.serialize(const_cast<" << (*param_it).type 
              << "&>(" << (*param_it).name
              << "));" << endl
            ;
          }

          param_it++;
        }

        output_
          << endl
          << ls_ << "  send(__handle,archiver.get_pbuf());"
          << endl
          << ls_ << "  outp_ptrs_.insert(handle_params_pair_t(__handle,out_params_t(function_id, opp)));"
          << endl
          << ls_ << "}" << endl
          << endl << endl
        ;

        func_it++;
      }



      /*---------------------------------------------------------\
      |  Cleanup                                                 |
      \---------------------------------------------------------*/
      generate_ns_close();

      output_
        << endl
        << "#endif /* __cplusplus */" << endl
        << "/* EOF */" << endl
      ;
    }

    void stub_client::generate_ping_body()
    {
      output_ 
        << ls_ << "  struct timeval tv;" << endl
        << ls_ << "  struct timezone tz;" << endl << endl
        << ls_ << "  gettimeofday(&tv,&tz);" << endl
        << ls_ << "  client_time = tv.tv_sec * 1000000 + tv.tv_usec;" << endl
      ;

      return; 
    }
  };
};

/* EOF */
