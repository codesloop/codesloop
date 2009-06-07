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

#include "csrgen.hh"
#include "iface.hh"

using namespace csl::rpc;

const char * csl::rpc::token_type_name[] = {
  "unknown",
  "version",
  "name", 
  "namespace",
  "include",
  "function",
  "disposable function",
  "end function",
  "parameter modifier",
  "parameter type",
  "parameter name",
  "comment"
};

const char * csl::rpc::param_kind_name[] = { 
  "input", 
  "output", 
  "inout", 
  "exception" 
};

%%{

  machine  csrgen;
  access token.;
  variable p  token.p;
  variable pe token.pe;


  action s { 
    token.ts = token.p;
  }

  action on_error {
    print_error(token);
  }

  action newline {
    token.curline++;
    token.ls = token.p;
  }

  action save {
    save(ifc,token);
    reset(token);
  }

  action type_name {
    token.type = TT_PARAM_TYPE;
    token.array_length  = 0; /* reset array length */
    save(ifc,token);
  }

  action obj_name {
    token.type = TT_PARAM_NAME;
    save(ifc,token);
  }

  action end_function {
    token.type = TT_FUNCTION_END; 
    token.ts = token.p;
  }

  action func_name {
    if ( token.type != TT_DISPOSABLE_FUNCTION )
      token.type = TT_FUNCTION;
    save(ifc,token);
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
  inout     = 'inout'       >s %{token.modifier = MD_INOUT;};
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

  array_decl  = ws* (
                    '[' ws* ']'
                  | '[' ws* ( digit* @add_arry_digit ) ws* ']'
                ) %{ ifc.set_arry_len(token.array_length); }
                ;

  # parameters and function header definition 
  parameter_spec  = type_ident %type_name ws+ :>> (identifier %obj_name)
                    array_decl? 
                    ;
  parameter_type  = (input|output|inout|exc) ':' %{token.type = TT_PARAM_MOD;};

  func_param_line      =  (ws* (parameter_type ws+)? <: 
                                parameter_spec ws*  ',')
                          ;
  func_param_lastline  =  (ws* (parameter_type ws+)? <: 
                                parameter_spec ws* ('}' >s %end_function) )
                          ;

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
                identifier
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
            @save           # save identified token to iface
            $!on_error
            ;

}%%

%% write data;

void reset(token_info & token)
{
  token.type          = TT_UNKNOWN;
  token.modifier      = MD_INPUT;
  token.array_length  = 0;
}

void print_error(token_info & token) 
{
  fprintf(stderr, "can not process file: parse error at line %d column %d\n",
            token.curline,
            (token.p-token.ls) );
  if ( token.p-token.ls < 70 ) {
    char * errloc = strdup( token.ls+1);
    int col = 0;
    for ( ; col < token.pe - token.ls ; col++ )
    {
      if ( errloc[col] == '\n' ) {
        errloc[col] = '\0';
        break;
      }
    }
    fprintf(stderr, "\n\"%s\"\n", errloc );
    free(errloc);
    for ( col = 0 ; col < token.p-token.ls ; col++ )
      fputc(' ', stderr);
    fprintf(stderr, "^---- here\n");
  }
}

void save(iface & ifc, token_info & token)
{
  if ( (token.p-token.ts) == 0 || token.type <= TT_UNKNOWN || token.type >= TT_LAST  )
    return;

  switch (token.type) 
  {
    case TT_NAME:
      ifc.set_name(token);
      break;
    case TT_NAMESPACE:
      ifc.set_namespc(token);
      break;
    case TT_VERSION:
      ifc.set_version(token);
      break;
    case TT_INCLUDE:
      ifc.add_include(token);
      break;
    case TT_DISPOSABLE_FUNCTION:
    case TT_FUNCTION:
      ifc.add_function(token);
      break;
    case TT_PARAM_TYPE:
      ifc.set_param_type(token);
      break;
    case TT_PARAM_NAME:
      ifc.set_param_name(token);
      break;
    default:
      break;
  }

}

void csrgen_init(token_info & token)
{
  %%  write  init;
  reset(token);
  token.curline = 1;
}

int csrgen_execute(token_info & token)
{
  char * eof = NULL;
  iface ifc;

  %%  write  exec;

  if ( token.cs == csrgen_error )
    return(1);
  
  printf("%s", ifc.to_string().c_str() );

  return(0);
}

int  main(  int  argc,  char  **argv  )
{
  std::string buffer;
  token_info token;

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
  
  csrgen_init(token);
            
  token.p   = (char*) buffer.c_str() ;    // entry point
  token.ls  = token.p;                    // first line's start
  token.pe  = token.p + strlen(token.p);  // exit point/eof
  
  return csrgen_execute(token);
}

