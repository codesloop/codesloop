/*
Copyright (c) 2008,2009, Beck David, Tamas Foldi

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

#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>

#include "csrparser.hh"

#include "stub_header.hh"
#include "stub_client.hh"
#include "stub_server.hh"

using namespace csl::rpc;

int  main(  int  argc,  char  **argv  )
{
  std::string buffer;
  csrparser p;
  int ret = 0;

  if ( argc == 1 ) 
  {
    fprintf(stderr, "usage: %s <filename>\n", argv[0] );
    exit(1);
  } else {
    std::ifstream in( argv[1] );
    if ( in.good() )
    {
      buffer = std::string(std::istreambuf_iterator<char>(in),std::istreambuf_iterator<char>());
    } else {
      fprintf(stderr, "%s: can not open file \"%s\"\n", argv[0], argv[1] );
      exit(1);
    }
  }
  
  ret = p.parse( 
      const_cast<char*>(buffer.c_str()),                 // file content
      const_cast<char*>(buffer.c_str()) + buffer.size()  // end of file
    );          
  

  stub_header h(p.get_iface());
  h.generate();
//  stub_client::generate( p.get_iface() );
//  stub_server::generate( p.get_iface() );

  if ( ret != 0 )
    exit(0);

  return 0;
}

