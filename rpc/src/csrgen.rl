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

%%{

  machine  csrgen;


  action s { 
    ts = p;
  }

  action print_err  {
    printf("\nerror at line %d:%d (%.10s...)\n",  
              curline, 
              (p-ls), 
              ls < p-10 ? ls+1 : p-10 
          );
  }

  action newline {
    curline++;
    ls = p;
  }

  action modifier {
    printf("\tmod:");
    fwrite(ts,1,p-ts,stdout);    
    printf("\n");
  }
  action type_name {
    printf("\t\t tn:");
    fwrite(ts,1,p-ts,stdout);
    printf(" -> ");
  }

  action obj_name {
    printf("on:");
    fwrite(ts,1,p-ts,stdout);
    printf("\n");
  }

  action func_name {
    printf("function:");
    fwrite(ts,1,p-ts,stdout);
    printf("\n");
  }

  # new line handler with line counting
  newline         = '\n' @newline;
  any_count_line  = any | newline;

  ws        = ('\t'|' '|'\r'|newline);
  ws_no_nl  = ('\t'|' '|'\r');

  # language constants
  input     = 'input'       >s;
  output    = 'output'      >s;
  exc       = 'exception'   >s;
  disp      = 'disposable';
  incl      = 'include';
  name      = 'name';
  version   = 'version';
  namespc   = 'namespace';
  

  # literals, identifiers
  dquote      = ( 'L'? '"' ( [^"\\\n] | /\\./ )* '"' )  >s;
  gtquote     = ( '<' ( [^>\\\n] | /\\./ )* '>' )       >s;
  identifier  = ( [a-zA-Z_] [a-zA-Z0-9_]* )             >s;

  type_ident  = ( [a-zA-Z_:] ([a-zA-Z 0-9_:<>]*[a-zA-Z0-9_>])?) >s;
  version_num = ( [0-9] ( '.'? [0-9] )* )      >s;


  # parameters and function header definition 
  parameter_spec  = type_ident %type_name ws+ :>> identifier %obj_name;
  parameter_type  = (input|output|exc) ':' %modifier;

  func_param_line      = (ws* (parameter_type ws+)? <: parameter_spec ws*  ','); 
  func_param_lastline  = (ws* (parameter_type ws+)? <: parameter_spec ws*  '}');

  function    = (disp ws+)? <: identifier %func_name ws* '{'                 
                func_param_line*         # regular lines with comma ending
                func_param_lastline      # last line with bracket ends
                ;
  
  # Describe both c style comments and c++ style comments. The
  # priority bump on tne terminator of the comments brings us
  # out of the extend* which matches everything.
  c_comment   =  '/*' any_count_line* :>>  '*/' %{printf("c comment\n");} ;
  cpp_comment = '//' [^\n]* newline             %{printf("cpp comment\n");} ;  
  comment     = cpp_comment | c_comment;

  includes    = '#' ws_no_nl* incl ws_no_nl+ 
                (dquote  | gtquote )
                ws_no_nl* newline
                @{printf("include\n");};

  if_name     = '#' ws_no_nl* name ws_no_nl+ 
                (dquote | identifier) 
                ws_no_nl* newline
                @{printf("if name\n");};

  if_version  = '#' ws_no_nl* version ws_no_nl+ 
                version_num 
                ws_no_nl* newline
                @{printf("if version\n");};

  if_namespc  = '#' ws_no_nl* namespc ws_no_nl+ 
                type_ident
                ws_no_nl* newline
                @{printf("if namespc\n");};

  main  :=  ( ws            # whitespace
            | comment       # comments
            | includes      # include statements
            | function      # interface definition for one function
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
  char *ts = 0;
  char * eof = NULL;
  char * ls = NULL;
  std::string buffer;

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
            

  char  *p = ls  = (char*) buffer.c_str() ;
  char  *pe  =  p  +  strlen(p);

  %%  write  init;
  %%  write  exec;

  if ( cs == csrgen_error ) 
    exit(1);

  return  0;
}
