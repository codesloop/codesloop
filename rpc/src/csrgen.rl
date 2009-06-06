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

#include "csrpc_types.hh"

using namespace csl::rpc;
token_info token;

// forward decls
void print_token();

%%{

  machine  csrgen;
  access token.;
  variable p  token.p;
  variable pe token.pe;


  action s { 
    token.ts = token.p;
  }

  action print_err  {
    printf("\nerror at line %d:%d (%.10s...)\n",  
              token.curline, 
              (token.p-token.ls), 
              token.ls < token.p-10 ? token.ls+1 : token.p-10 
          );
  }

  action newline {
    token.curline++;
    token.ls = token.p;
  }

  action reset {
    print_token();
    reset();
  }

  action modifier {
    token.type = TT_PARAM_MOD;
    print_token();
  }

  action type_name {
    token.type = TT_PARAM_TYPE;
    token.array_length  = 0; /* reset array length */
    print_token();
  }

  action obj_name {
    token.type = TT_PARAM_NAME;
    print_token();
  }

  action end_function {
    token.type = TT_FUNCTION_END; 
    token.ts = token.p;
  }

  action func_name {
    if ( token.type != TT_DISPOSABLE_FUNCTION )
      token.type = TT_FUNCTION;
    print_token();
  }

  action add_arry_digit {
    token.array_length = token.array_length * 10 + (fc - '0');
  }

  # new line handler with line counting
  newline         = '\n' @newline;
  any_count_line  = any | newline;

  ws        = ('\t'|' '|'\r'|newline);
  ws_no_nl  = ('\t'|' '|'\r');

  # language constants
  input     = 'input'       >s %{token.modifier = MD_INPUT;};
  output    = 'output'      >s %{token.modifier = MD_OUTPUT;};
  exc       = 'exception'   >s %{token.modifier = MD_EXCEPTION;};
  disp      = 'disposable'     %{token.type     = TT_DISPOSABLE_FUNCTION;};
  incl      = 'include'        %{token.type     = TT_INCLUDE;};
  name      = 'name'           %{token.type     = TT_NAME;};
  version   = 'version'        %{token.type     = TT_VERSION;};
  namespc   = 'namespace'      %{token.type     = TT_NAMESPACE;};
  

  # literals, identifiers
  dquote      = ( 'L'? '"' ( [^"\\\n] | /\\./ )* '"' )  >s;
  gtquote     = ( '<' ( [^>\\\n] | /\\./ )* '>' )       >s;
  identifier  = ( [a-zA-Z_] [a-zA-Z0-9_]* )             >s;

  type_ident  = ( [a-zA-Z_:] ([a-zA-Z 0-9_:<>]*[a-zA-Z0-9_>])?) >s;
  version_num = ( [0-9] ( '.'? [0-9] )* )      >s;

  array_decl  =   '[' ws* ']' %{token.array_length = -1;}
                | '[' ws* ( digit* @add_arry_digit ) ws* ']'
                ;


  # parameters and function header definition 
  parameter_spec  = type_ident %type_name ws+ :>> identifier array_decl?  %obj_name;
  parameter_type  = (input|output|exc) ':' %modifier;

  func_param_line      = (ws* (parameter_type ws+)? <: parameter_spec ws*  ',');
  func_param_lastline  = (ws* (parameter_type ws+)? <: parameter_spec ws*  '}' %end_function  );

  function    = (disp ws+)? <: identifier %func_name ws* '{'                 
                func_param_line*         # regular lines with comma ending
                func_param_lastline      # last line with bracket ends
                ;
  
  # Describe both c style comments and c++ style comments. The
  # priority bump on tne terminator of the comments brings us
  # out of the extend* which matches everything.
  c_comment   =  '/*' any_count_line* :>>  '*/';
  cpp_comment = '//' [^\n]* newline;
  comment     = (cpp_comment | c_comment) >s %{token.type = TT_COMMENT;};

  includes    = '#' ws_no_nl* incl ws_no_nl+ 
                (dquote  | gtquote )
                ws_no_nl* newline
                ;

  if_name     = '#' ws_no_nl* name ws_no_nl+ 
                (dquote | identifier) 
                ws_no_nl* newline
                ;

  if_version  = '#' ws_no_nl* version ws_no_nl+ 
                version_num 
                ws_no_nl* newline
                ;

  if_namespc  = '#' ws_no_nl* namespc ws_no_nl+ 
                type_ident
                ws_no_nl* newline
                ;

  main  :=  ( ws            # whitespace
            | comment       # comments
            | includes      # include statements
            | function      # interface definition for one function
            | if_version    # version information
            | if_name       # interface name
            | if_namespc    # namespace
            )* 
            @reset          # reset after identified token
            $!print_err
            ;

}%%

%% write data;

void reset()
{
  token.type          = TT_UNKNOWN;
  token.modifier      = MD_INPUT;
  token.len           = 0;
  token.array_length  = 0;
}

void csrgen_init()
{
  %%  write  init;
  reset();
  token.curline = 1;
}

int csrgen_execute()
{
  char * eof = NULL;

  %%  write  exec;

  if ( token.cs == csrgen_error )
    return(1);
  
  return(0);
}

void print_token()
{
  if ( (token.p-token.ts) == 0 || token.type <= TT_UNKNOWN || token.type >= TT_LAST ||
        token.type == TT_COMMENT )
  {
    return;
  }

  printf("Token: \"");
  fwrite(token.ts ,1,token.p-token.ts,stdout);
  if ( token.type == TT_PARAM_NAME && token.array_length ) 
    printf("\" array len: %d (%s)\n", token.array_length, token_type_name[token.type] );
  else 
    printf("\" (%s)\n", token_type_name[token.type] );
}

int  main(  int  argc,  char  **argv  )
{
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
  
  csrgen_init();
            
  token.p   = (char*) buffer.c_str() ;    // entry point
  token.ls  = token.p;                    // first line's start
  token.pe  = token.p + strlen(token.p);  // exit point/eof
  
  return csrgen_execute();
}

