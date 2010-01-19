/*
Copyright (c) 2008,2009,2010, CodeSloop Team

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

#include "codesloop/rpc/csrgen.hh"
#include "codesloop/rpc/csrparser.hh"
#include "codesloop/rpc/iface.hh"
#include "codesloop/common/logger.hh"

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
  access token_.;
  variable p  token_.p;
  variable pe token_.pe;


  action s { 
    token_.ts = token_.p;
  }

  action on_error {
    print_error("parse error");
  }

  action newline {
    token_.curline++;
    token_.ls = token_.p;
  }

  action save {
    save();
    reset();
  }

  action type_name {
    token_.type = TT_PARAM_TYPE;
    token_.array_length  = 0; /* reset array length */
    save();
  }

  action obj_name {
    token_.type = TT_PARAM_NAME;
    save();
  }

  action end_function {
    token_.type = TT_FUNCTION_END; 
    token_.ts = token_.p;
  }

  action func_name {
    if ( token_.type != TT_DISPOSABLE_FUNCTION )
      token_.type = TT_FUNCTION;
    save();
  }

  action add_arry_digit {
    token_.array_length = token_.array_length * 10 + (fc - '0');
  }

  # new line handler with line counting
  newline         = '\n' @newline;
  any_count_line  = any | newline;

  ws        = ('\t'|' '|'\r'|newline);
  ws_no_nl  = ('\t'|' '|'\r');

  # language constants
  input     = 'input'       >s %{token_.modifier = MD_INPUT;};
  output    = 'output'      >s %{token_.modifier = MD_OUTPUT;};
  inout     = 'inout'       >s %{token_.modifier = MD_INOUT;};
  exc       = 'exception'   >s %{token_.modifier = MD_EXCEPTION;};
  disp      = 'disposable'     %{token_.type     = TT_DISPOSABLE_FUNCTION;};
  incl      = 'include'        %{token_.type     = TT_INCLUDE;};
  name      = 'name'           %{token_.type     = TT_NAME;};
  version   = 'version'        %{token_.type     = TT_VERSION;};
  namespc   = 'namespace'      %{token_.type     = TT_NAMESPACE;};
  transport = 'transport'      %{token_.type     = TT_TRANSPORT;};
  

  # literals, identifiers
  dquote      = ( 'L'? '"' ( [^"\\\n] | /\\./ )* '"' )  >s;
  gtquote     = ( '<' ( [^>\\\n] | /\\./ )* '>' )       >s;
  identifier  = ( [a-zA-Z_] [a-zA-Z0-9_]* )             >s;

  type_ident  = ( [a-zA-Z_:] ([a-zA-Z 0-9_:<>]*[a-zA-Z0-9_>])?) >s;
  namespc_def = ( [a-zA-Z_]  ([a-zA-Z0-9_:]*[a-zA-Z0-9_])?)     >s;
  version_num = ( [0-9] ( '.'? [0-9] )* )      >s;

  array_decl  = ws* (
                    '[' ws* ']'
                  | '[' ws* ( digit* @add_arry_digit ) ws* ']'
                ) %{ iface_.set_arry_len(token_.array_length); }
                ;

  # parameters and function header definition 
  parameter_spec  = type_ident %type_name ws+ :>> (identifier %obj_name)
                    array_decl? 
                    ;
  parameter_type  = (input|output|inout|exc) ':' %{token_.type = TT_PARAM_MOD;};

  func_param_line      =  (ws* (parameter_type ws+)?  
                                parameter_spec ws*  ',')
                          ;
  func_param_lastline  =  (ws* (parameter_type ws+)?  
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
  comment     = (cpp_comment | c_comment) >s %{token_.type = TT_COMMENT;};

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
                namespc_def
                ws_no_nl* newline
                ;

  if_transprt = '#' ws_no_nl* transport ws_no_nl+
                ('tcp'|'udp'|'mq') >s
                ws_no_nl* newline
                ;


  main  :=  ( ws            # whitespace
            | comment       # comments
            | includes      # include statements
            | function      # interface definition for one function
            | if_version    # version information
            | if_name       # interface name
            | if_namespc    # namespace
            | if_transprt   # transport layer
            )* 
            @save           # save identified token to iface
            $!on_error
            ;

}%%

namespace csl 
{
  namespace rpc
  {
    %% write data;

    void csrparser::reset()
    {
      token_.type          = TT_UNKNOWN;
      token_.modifier      = MD_INPUT;
      token_.array_length  = 0;
    } 

    void csrparser::print_error(const char * error_msg) const
    {
      fputs("can not process file: ", stderr);
      fputs( error_msg, stderr);
      fprintf(stderr, " at line %d column %d\n",
          token_.curline,
          (token_.p-token_.ls) );

      if ( token_.p-token_.ls < 70 ) {
        char * errloc = strdup( token_.ls+1);
        int col = 0;
        for ( ; col < token_.pe - token_.ls ; col++ )
        {
          if ( errloc[col] == '\n' ) {
            errloc[col] = '\0';
            break;
          }
        }
        fprintf(stderr, "\n\"%s\"\n", errloc );
        free(errloc);
        for ( col = 0 ; col < token_.p-token_.ls ; col++ )
          fputc(' ', stderr);
        fprintf(stderr, "^---- here\n");
      }
    }

    void csrparser::save()
    {
      if ( (token_.p-token_.ts) == 0 || token_.type <= TT_UNKNOWN 
        || token_.type >= TT_LAST  )
      {
        return;
      }

      /* store tokens in iface */
      switch (token_.type) 
      {
        case TT_NAME:
          iface_.set_name(token_);
          break;
        case TT_NAMESPACE:
          iface_.set_namespc(token_);
          break;
        case TT_VERSION:
          iface_.set_version(token_);
          break;
        case TT_TRANSPORT:
          iface_.set_transport(token_);
          break;
        case TT_INCLUDE:
          iface_.add_include(token_);
          break;
        case TT_DISPOSABLE_FUNCTION:
        case TT_FUNCTION:
          iface_.add_function(token_);
          break;
        case TT_PARAM_TYPE:
          iface_.set_param_type(token_);
          break;
        case TT_PARAM_NAME:
          iface_.set_param_name(token_);
          break;
        default:
          break;
      }

    }

    csrparser::csrparser()
    {
      memset( &token_, 0, sizeof(token_) );
      %%  write  init;
      reset();
      token_.curline = 1;
    }

    int csrparser::parse(char * start, char * end)
    {
      char * eof = NULL;

      token_.p = start;
      token_.pe = end;
      token_.ls = token_.p; 

      %%  write  exec;

      if ( token_.cs == csrgen_error )
        return(1);

      CSL_DEBUGF( L"Parsed token tree:\n%s", iface_.to_string().c_str() );

      return(0);
    } 

  } /* namespace rpc */
} /* namespace csl */
