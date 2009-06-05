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

#include <string.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>

using namespace std;

%%{


  machine  csl_rpcgen;

  action print_err  {
    printf("error at line %d:%d (0x%x)\n", curline, (p-ls), *p );
  }

  action newline {
    curline++;
    ls = p;
  }

  # new line handler with line counting
  newline         = '\n' @newline;
  any_count_line  = any | newline;

  ws        = ('\t'|' '|'\r'|newline);
  ws_no_nl  = ('\t'|' '|'\r');

  # language constants
  disp      = 'disposable';
  input     = 'input';
  output    = 'output';
  exc       = 'exception';
  incl      = 'include';
  name      = 'name';
  version   = 'version';
  namespc   = 'namespace';
  

  # literals, identifiers
  dquote      = ( 'L'? '"' ( [^"\\\n] | /\\./ )* '"' );
  gtquote     = ( '<' ( [^>\\\n] | /\\./ )* '>' );
  identifier  = ( [a-zA-Z_] [a-zA-Z0-9_]* )     @{printf("identifier\n");} ;
  type_ident  = ( [a-zA-Z_:] [a-zA-Z0-9_:<>]*)  @{printf("type identifier\n");} ;
  version_num = ( [0-9] ( '.'? [0-9] )* )       @{printf("version num\n");} ; 


  # parameters and function header definition 
  parameter_spec  = type_ident ws* identifier @{printf("spec\n");};
  parameter_type  = (input|output|exc) ':'    @{printf("type\n");};

  function    = (disp ws+)? identifier ws* '{' ws*
                  ((parameter_spec ws* ',' | parameter_type) ws*)*    # parameters ended by ,
                  ( parameter_spec         | parameter_type) ws*      # last parameter without ,
                '}'
                @{printf("function\n");}
                ;
  
  # Describe both c style comments and c++ style comments. The
  # priority bump on tne terminator of the comments brings us
  # out of the extend* which matches everything.
  c_comment   =  '/*' any_count_line* :>>  '*/' @{printf("c comment\n");} ;
  cpp_comment = '//' [^\n]* newline @{printf("cpp comment\n");} ;  
  comment     = cpp_comment | c_comment;

  includes    = '#' ws_no_nl* incl ws_no_nl+ 
                (dquote  | gtquote )
                @{printf("include\n");};

  if_name     = '#' ws_no_nl* name ws_no_nl+ 
                (dquote | identifier) 
                @{printf("if name\n");};

  if_version  = '#' ws_no_nl* version ws_no_nl+ 
                version_num 
                @{printf("if version\n");};

  if_namespc  = '#' ws_no_nl* namespc ws_no_nl+ 
                type_ident
                @{printf("if name\n");};

  main  :=  ( ws            # whitespace
            | comment       # comments
            | includes      # include statements
            | function      # interface declaration for on function
            | if_version    # version information
            | if_name       # interface name
            | if_namespc    # namespace
            )*    
            $!print_err;

}%%

%% write data;

int  main(  int  argc,  char  **argv  )
{
  int cs, curline = 1;
  stringstream buffer;
  string line;
  char * eof = NULL;
  char * ls = NULL;

  if ( argc == 1 ) 
  {
    while ( cin >> line)
      buffer << line << endl;      

    printf("<< buffer >>\n%s<< buffer >>\n", buffer.str().c_str() );

  } else {
    cout << "Readiny file " << argv[1] << endl;
    std::ifstream in( argv[1] );
    buffer << std::string(std::istreambuf_iterator<char>(in),std::istreambuf_iterator<char>());      
  }
            

  char  *p = ls  = (char*) buffer.str().c_str() ;
  char  *pe  =  p  +  strlen(p);

  %%  write  init;
  %%  write  exec;

  if ( cs == csl_rpcgen_error ) {
    fprintf(stderr, "PARSE ERROR\n" );
  }


  printf("lines  =  %i\n",  curline );
  return  0;
}
