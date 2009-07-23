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

#include "stub_client.hh"
#include "common.h"

/**
  @file csl_rpc/src/stub_client.cc
  @brief implementation of codesloop interface descriptor
 */

using std::endl;

namespace csl
{
  namespace rpc
  {
    void stub_client::generate()
    {
      open_file((ifname_+"_cli.cc").c_str());

      output_
        << "#ifdef __cplusplus" << endl
        << "#include \"" << (ifname_+"_cli.hh").c_str() << "\""        
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
        output_ << ls_ << "void " << (*func_it).name << " (" << endl;
        this->generate_func_params( (*func_it).name, STUB_CLIENT, false);
        output_ 
          << endl 
          << ls_ << "{" 
          << endl
        ;

      /*---------------------------------------------------------\
      |  Synchronous function                                    |
      \---------------------------------------------------------*/        
        output_ 
          << ls_ << "  " << (*func_it).name << "("  << endl
          << ls_ << "    " << "NULL," << endl
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
          << endl
          << ls_ << "}" << endl
        ;

      /*---------------------------------------------------------\
      |  Function implementation on client side stub             |
      \---------------------------------------------------------*/        
        output_ << ls_ << "void " << (*func_it).name << " (" << endl;
        this->generate_func_params( (*func_it).name, STUB_CLIENT, true);
        output_
          << endl
          << ls_ << "{"
          << endl << endl
        ;

        // serializer
        output_ 
          << ls_ << "  arch archiver(arch::SERIALIZE);" << endl
          << endl
          << ls_ << "  archiver.serialize(__if_" << ifname_.c_str()  
          <<             "_crc );" << endl
          << ls_ << "  archiver.serialize(__func_"<< ifname_.c_str()  
          <<             "_"<< (*func_it).name << "_id);" << endl        
        ;

        param_it = (*func_it).params.begin();
        while( param_it != (*func_it).params.end() )
        {
          /* ignore exceptions and output parameters */
          if( (*param_it).kind==MD_EXCEPTION ||(*param_it).kind==MD_OUTPUT) {
            param_it++;
            continue;
          }

          output_
            << ls_ << "  archiver.serialize(" << (*param_it).name
            << ");" << endl
          ;

          param_it++;
        }

        output_
          << endl
          << ls_ << "  send(__handle,archiver.get_pbuf());"
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
  };
};

/* EOF */
