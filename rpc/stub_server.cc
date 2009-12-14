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

#include "codesloop/rpc/stub_server.hh"
#include "codesloop/common/common.h"

using std::endl;

/**
  @file rpc/src/stub_server.cc
  @brief implementation of codesloop interface descriptor
 */

namespace csl
{
  namespace rpc
  {
    void stub_server::generate()
    {
      const char * class_name = (ifname_ + "_srv").c_str();

      open_file((ifname_+"_srv.cc").c_str());

      output_
        << "#ifdef __cplusplus" << endl
        << "#include \"" << (ifname_+"_srv.hh").c_str() << "\""
        << endl
        << "#include <codesloop/common/arch.hh>"
        << endl
        << "#include <codesloop/common/pbuf.hh>" 
        << endl
        << "#include <codesloop/rpc/exc.hh>" 
        << endl
      ;

      /*---------------------------------------------------------\
      |  Generate namespace info                                 |
      \---------------------------------------------------------*/
      this->generate_ns_open();




      /*---------------------------------------------------------\
      |  Despatcher function                                     |
      \---------------------------------------------------------*/
      iface::function_iterator func_it = ifc_->get_functions()->begin();
      iface::func::param_iterator param_it;

      output_
        << ls_ << "/* implement function call routing */" << endl
        << ls_ << "void " << class_name << "::despatch (" << endl
        << ls_ << "  /* inout */  csl::common::arch & archiver)" << endl
        << ls_ << "{" << endl 
        << ls_ << "  ENTER_FUNCTION();" << endl << endl
        << ls_ << "  int64_t interface_id;" << endl
        << ls_ << "  uint32_t function_id = fid_hello;" << endl 
        << ls_ << endl
        << ls_ << "  archiver.serialize(interface_id);" << endl
        << ls_ << "  archiver.serialize(function_id);" << endl 
        << ls_ << endl
        << ls_ << "  if ( interface_id != get_crc64() ) " << endl
        << ls_ << "    throw csl::rpc::exc(csl::rpc::exc::rs_incompat_iface,"
        <<        "L\"Can not despatch request, interfaces are different\"); " << endl 
        << ls_ << endl
        << ls_ << "  switch( function_id )" << endl          
        << ls_ << "  {" << endl
      ;

      while ( func_it != ifc_->get_functions()->end() )
      {
        output_
          << ls_ << "    case fid_" << (*func_it).name << ":" << endl
          << ls_ << "    {" << endl
        ; 
        param_it = (*func_it).params.begin();
        while( param_it != (*func_it).params.end() )
        {
          // create type 
          if ( (*param_it).kind!=MD_EXCEPTION ) {
            output_ 
              << ls_ << "      " << (*param_it).type << " " << (*param_it).name << ";"
              << endl
              ;

            // deserialize data if necessary
            if ( (*param_it).kind!=MD_OUTPUT ) {
              output_ 
                << ls_ << "      archiver.serialize( " << (*param_it).name << ");" 
                << endl
              ;
            }
          
          } // create type

          param_it++;
        }
        output_ << endl;


        // call it!
        output_ 
          << ls_ << "      try { " << endl << endl
          << ls_ << "        " << (*func_it).name << "("
        ;

        param_it = (*func_it).params.begin();
        while( param_it != (*func_it).params.end() )
        {
          if ( (*param_it).kind!=MD_EXCEPTION ) {
            output_ <<  (*param_it).name ;
          }
          param_it++;
          // TODO: now it works only, if exceptions are defined last
          if ( (*param_it).kind!=MD_EXCEPTION && param_it != (*func_it).params.end() )
            output_ << ", ";
        }

        output_ << ");" << endl << endl;

        // handl exceptions
        param_it = (*func_it).params.begin();
        while( param_it != (*func_it).params.end() )
        {
          if ( (*param_it).kind==MD_EXCEPTION ) {
            output_
              << ls_ << "      } catch(" << (*param_it).type << " & " 
              << (*param_it).name<< ") {" << endl
              << ls_ << "        /* TODO: handle as output parameter */" << endl 
            ;
          }
          param_it++;
        }
        output_ << ls_ << "      } catch(...) {" << endl;
        output_ << ls_ << "        /* TODO: raise csl::rpc::exc exception */" << endl;
        output_ << ls_ << "      } " << endl;

        output_
          << ls_ << "    }" << endl
          << ls_ << "    break;" << endl
        ;

        func_it++;
      }
      output_
        << ls_ << "    default:" << endl
        << ls_ << "      throw csl::rpc::exc(csl::rpc::exc::rs_invalid_fid,L\""
        <<               ifname_.c_str() << " interface\");" << endl
        << ls_ << "    break;" << endl
        << ls_ << "  } /* switch */" << endl
      ;


      output_
        << ls_ << "  LEAVE_FUNCTION();" << endl 
        << ls_ << "}" << endl
      ;

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
    
  };
};

/* EOF */
