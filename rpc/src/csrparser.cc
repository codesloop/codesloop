
#line 1 "csrparser.rl"
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

#include "csrgen.hh"
#include "csrparser.hh"
#include "iface.hh"

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


#line 198 "csrparser.rl"


namespace csl 
{
  namespace rpc
  {
    
#line 68 "csrparser.cc"
static const int csrgen_start = 94;
static const int csrgen_first_final = 94;
static const int csrgen_error = 0;

static const int csrgen_en_main = 94;


#line 205 "csrparser.rl"

    void csrparser::reset()
    {
      token_.type          = TT_UNKNOWN;
      token_.modifier      = MD_INPUT;
      token_.array_length  = 0;
    } 

    void csrparser::print_error() const
    {
      fprintf(stderr, "can not process file: parse error at line %d column %d\n",
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
      
#line 150 "csrparser.cc"
	{
	 token_.cs = csrgen_start;
	}

#line 278 "csrparser.rl"
      reset();
      token_.curline = 1;
    }

    int csrparser::parse(char * start, char * end)
    {
      char * eof = NULL;

      token_.p = start;
      token_.pe = end;
      token_.ls = token_.p; 

      
#line 169 "csrparser.cc"
	{
	if ( (  token_.p) == ( token_.pe) )
		goto _test_eof;
	switch (  token_.cs )
	{
tr19:
#line 72 "csrparser.rl"
	{
    token_.curline++;
    token_.ls = token_.p;
  }
#line 77 "csrparser.rl"
	{
    save();
    reset();
  }
	goto st94;
tr140:
#line 77 "csrparser.rl"
	{
    save();
    reset();
  }
	goto st94;
tr143:
#line 164 "csrparser.rl"
	{token_.type = TT_COMMENT;}
#line 77 "csrparser.rl"
	{
    save();
    reset();
  }
	goto st94;
tr144:
#line 164 "csrparser.rl"
	{token_.type = TT_COMMENT;}
#line 72 "csrparser.rl"
	{
    token_.curline++;
    token_.ls = token_.p;
  }
#line 77 "csrparser.rl"
	{
    save();
    reset();
  }
	goto st94;
tr149:
#line 93 "csrparser.rl"
	{
    token_.type = TT_FUNCTION_END; 
    token_.ts = token_.p;
  }
#line 77 "csrparser.rl"
	{
    save();
    reset();
  }
	goto st94;
tr150:
#line 93 "csrparser.rl"
	{
    token_.type = TT_FUNCTION_END; 
    token_.ts = token_.p;
  }
#line 72 "csrparser.rl"
	{
    token_.curline++;
    token_.ls = token_.p;
  }
#line 77 "csrparser.rl"
	{
    save();
    reset();
  }
	goto st94;
st94:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof94;
case 94:
#line 250 "csrparser.cc"
	switch( (*(  token_.p)) ) {
		case 9: goto tr140;
		case 10: goto tr19;
		case 13: goto tr140;
		case 32: goto tr140;
		case 35: goto st1;
		case 47: goto tr141;
		case 95: goto tr109;
		case 100: goto st55;
	}
	if ( (*(  token_.p)) > 90 ) {
		if ( 97 <= (*(  token_.p)) && (*(  token_.p)) <= 122 )
			goto tr109;
	} else if ( (*(  token_.p)) >= 65 )
		goto tr109;
	goto tr0;
tr0:
#line 68 "csrparser.rl"
	{
    print_error();
  }
	goto st0;
#line 273 "csrparser.cc"
st0:
 token_.cs = 0;
	goto _out;
tr145:
#line 164 "csrparser.rl"
	{token_.type = TT_COMMENT;}
	goto st1;
tr151:
#line 93 "csrparser.rl"
	{
    token_.type = TT_FUNCTION_END; 
    token_.ts = token_.p;
  }
	goto st1;
st1:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof1;
case 1:
#line 292 "csrparser.cc"
	switch( (*(  token_.p)) ) {
		case 9: goto st1;
		case 13: goto st1;
		case 32: goto st1;
		case 105: goto st2;
		case 110: goto st16;
		case 118: goto st30;
	}
	goto tr0;
st2:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof2;
case 2:
	if ( (*(  token_.p)) == 110 )
		goto st3;
	goto tr0;
st3:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof3;
case 3:
	if ( (*(  token_.p)) == 99 )
		goto st4;
	goto tr0;
st4:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof4;
case 4:
	if ( (*(  token_.p)) == 108 )
		goto st5;
	goto tr0;
st5:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof5;
case 5:
	if ( (*(  token_.p)) == 117 )
		goto st6;
	goto tr0;
st6:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof6;
case 6:
	if ( (*(  token_.p)) == 100 )
		goto st7;
	goto tr0;
st7:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof7;
case 7:
	if ( (*(  token_.p)) == 101 )
		goto st8;
	goto tr0;
st8:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof8;
case 8:
	switch( (*(  token_.p)) ) {
		case 9: goto tr11;
		case 13: goto tr11;
		case 32: goto tr11;
	}
	goto tr0;
tr11:
#line 121 "csrparser.rl"
	{token_.type     = TT_INCLUDE;}
	goto st9;
st9:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof9;
case 9:
#line 362 "csrparser.cc"
	switch( (*(  token_.p)) ) {
		case 9: goto st9;
		case 13: goto st9;
		case 32: goto st9;
		case 34: goto tr13;
		case 60: goto tr14;
		case 76: goto tr15;
	}
	goto tr0;
tr13:
#line 64 "csrparser.rl"
	{ 
    token_.ts = token_.p;
  }
	goto st10;
st10:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof10;
case 10:
#line 382 "csrparser.cc"
	switch( (*(  token_.p)) ) {
		case 10: goto tr0;
		case 34: goto st11;
		case 92: goto st12;
	}
	goto st10;
st11:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof11;
case 11:
	switch( (*(  token_.p)) ) {
		case 9: goto st11;
		case 10: goto tr19;
		case 13: goto st11;
		case 32: goto st11;
	}
	goto tr0;
st12:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof12;
case 12:
	goto st10;
tr14:
#line 64 "csrparser.rl"
	{ 
    token_.ts = token_.p;
  }
	goto st13;
st13:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof13;
case 13:
#line 415 "csrparser.cc"
	switch( (*(  token_.p)) ) {
		case 10: goto tr0;
		case 62: goto st11;
		case 92: goto st14;
	}
	goto st13;
st14:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof14;
case 14:
	goto st13;
tr15:
#line 64 "csrparser.rl"
	{ 
    token_.ts = token_.p;
  }
	goto st15;
st15:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof15;
case 15:
#line 437 "csrparser.cc"
	if ( (*(  token_.p)) == 34 )
		goto st10;
	goto tr0;
st16:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof16;
case 16:
	if ( (*(  token_.p)) == 97 )
		goto st17;
	goto tr0;
st17:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof17;
case 17:
	if ( (*(  token_.p)) == 109 )
		goto st18;
	goto tr0;
st18:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof18;
case 18:
	if ( (*(  token_.p)) == 101 )
		goto st19;
	goto tr0;
st19:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof19;
case 19:
	switch( (*(  token_.p)) ) {
		case 9: goto tr25;
		case 13: goto tr25;
		case 32: goto tr25;
		case 115: goto st22;
	}
	goto tr0;
tr25:
#line 122 "csrparser.rl"
	{token_.type     = TT_NAME;}
	goto st20;
st20:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof20;
case 20:
#line 481 "csrparser.cc"
	switch( (*(  token_.p)) ) {
		case 9: goto st20;
		case 13: goto st20;
		case 32: goto st20;
		case 95: goto tr28;
	}
	if ( (*(  token_.p)) > 90 ) {
		if ( 97 <= (*(  token_.p)) && (*(  token_.p)) <= 122 )
			goto tr28;
	} else if ( (*(  token_.p)) >= 65 )
		goto tr28;
	goto tr0;
tr28:
#line 64 "csrparser.rl"
	{ 
    token_.ts = token_.p;
  }
	goto st21;
st21:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof21;
case 21:
#line 504 "csrparser.cc"
	switch( (*(  token_.p)) ) {
		case 9: goto st11;
		case 10: goto tr19;
		case 13: goto st11;
		case 32: goto st11;
		case 95: goto st21;
	}
	if ( (*(  token_.p)) < 65 ) {
		if ( 48 <= (*(  token_.p)) && (*(  token_.p)) <= 57 )
			goto st21;
	} else if ( (*(  token_.p)) > 90 ) {
		if ( 97 <= (*(  token_.p)) && (*(  token_.p)) <= 122 )
			goto st21;
	} else
		goto st21;
	goto tr0;
st22:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof22;
case 22:
	if ( (*(  token_.p)) == 112 )
		goto st23;
	goto tr0;
st23:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof23;
case 23:
	if ( (*(  token_.p)) == 97 )
		goto st24;
	goto tr0;
st24:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof24;
case 24:
	if ( (*(  token_.p)) == 99 )
		goto st25;
	goto tr0;
st25:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof25;
case 25:
	if ( (*(  token_.p)) == 101 )
		goto st26;
	goto tr0;
st26:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof26;
case 26:
	switch( (*(  token_.p)) ) {
		case 9: goto tr34;
		case 13: goto tr34;
		case 32: goto tr34;
	}
	goto tr0;
tr34:
#line 124 "csrparser.rl"
	{token_.type     = TT_NAMESPACE;}
	goto st27;
st27:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof27;
case 27:
#line 567 "csrparser.cc"
	switch( (*(  token_.p)) ) {
		case 9: goto st27;
		case 13: goto st27;
		case 32: goto st27;
		case 58: goto tr36;
		case 95: goto tr36;
	}
	if ( (*(  token_.p)) > 90 ) {
		if ( 97 <= (*(  token_.p)) && (*(  token_.p)) <= 122 )
			goto tr36;
	} else if ( (*(  token_.p)) >= 65 )
		goto tr36;
	goto tr0;
tr36:
#line 64 "csrparser.rl"
	{ 
    token_.ts = token_.p;
  }
	goto st28;
st28:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof28;
case 28:
#line 591 "csrparser.cc"
	switch( (*(  token_.p)) ) {
		case 9: goto st11;
		case 10: goto tr19;
		case 13: goto st11;
		case 32: goto st28;
		case 58: goto st29;
		case 60: goto st29;
		case 62: goto st28;
		case 95: goto st28;
	}
	if ( (*(  token_.p)) < 65 ) {
		if ( 48 <= (*(  token_.p)) && (*(  token_.p)) <= 57 )
			goto st28;
	} else if ( (*(  token_.p)) > 90 ) {
		if ( 97 <= (*(  token_.p)) && (*(  token_.p)) <= 122 )
			goto st28;
	} else
		goto st28;
	goto tr0;
st29:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof29;
case 29:
	switch( (*(  token_.p)) ) {
		case 32: goto st29;
		case 58: goto st29;
		case 60: goto st29;
		case 62: goto st28;
		case 95: goto st28;
	}
	if ( (*(  token_.p)) < 65 ) {
		if ( 48 <= (*(  token_.p)) && (*(  token_.p)) <= 57 )
			goto st28;
	} else if ( (*(  token_.p)) > 90 ) {
		if ( 97 <= (*(  token_.p)) && (*(  token_.p)) <= 122 )
			goto st28;
	} else
		goto st28;
	goto tr0;
st30:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof30;
case 30:
	if ( (*(  token_.p)) == 101 )
		goto st31;
	goto tr0;
st31:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof31;
case 31:
	if ( (*(  token_.p)) == 114 )
		goto st32;
	goto tr0;
st32:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof32;
case 32:
	if ( (*(  token_.p)) == 115 )
		goto st33;
	goto tr0;
st33:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof33;
case 33:
	if ( (*(  token_.p)) == 105 )
		goto st34;
	goto tr0;
st34:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof34;
case 34:
	if ( (*(  token_.p)) == 111 )
		goto st35;
	goto tr0;
st35:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof35;
case 35:
	if ( (*(  token_.p)) == 110 )
		goto st36;
	goto tr0;
st36:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof36;
case 36:
	switch( (*(  token_.p)) ) {
		case 9: goto tr45;
		case 13: goto tr45;
		case 32: goto tr45;
	}
	goto tr0;
tr45:
#line 123 "csrparser.rl"
	{token_.type     = TT_VERSION;}
	goto st37;
st37:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof37;
case 37:
#line 691 "csrparser.cc"
	switch( (*(  token_.p)) ) {
		case 9: goto st37;
		case 13: goto st37;
		case 32: goto st37;
	}
	if ( 48 <= (*(  token_.p)) && (*(  token_.p)) <= 57 )
		goto tr47;
	goto tr0;
tr47:
#line 64 "csrparser.rl"
	{ 
    token_.ts = token_.p;
  }
	goto st38;
st38:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof38;
case 38:
#line 710 "csrparser.cc"
	switch( (*(  token_.p)) ) {
		case 9: goto st11;
		case 10: goto tr19;
		case 13: goto st11;
		case 32: goto st11;
		case 46: goto st39;
	}
	if ( 48 <= (*(  token_.p)) && (*(  token_.p)) <= 57 )
		goto st38;
	goto tr0;
st39:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof39;
case 39:
	if ( 48 <= (*(  token_.p)) && (*(  token_.p)) <= 57 )
		goto st38;
	goto tr0;
tr141:
#line 64 "csrparser.rl"
	{ 
    token_.ts = token_.p;
  }
	goto st40;
tr146:
#line 164 "csrparser.rl"
	{token_.type = TT_COMMENT;}
#line 64 "csrparser.rl"
	{ 
    token_.ts = token_.p;
  }
	goto st40;
tr152:
#line 93 "csrparser.rl"
	{
    token_.type = TT_FUNCTION_END; 
    token_.ts = token_.p;
  }
#line 64 "csrparser.rl"
	{ 
    token_.ts = token_.p;
  }
	goto st40;
st40:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof40;
case 40:
#line 757 "csrparser.cc"
	switch( (*(  token_.p)) ) {
		case 42: goto st41;
		case 47: goto st93;
	}
	goto tr0;
tr52:
#line 72 "csrparser.rl"
	{
    token_.curline++;
    token_.ls = token_.p;
  }
	goto st41;
st41:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof41;
case 41:
#line 774 "csrparser.cc"
	switch( (*(  token_.p)) ) {
		case 10: goto tr52;
		case 42: goto st42;
	}
	goto st41;
st42:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof42;
case 42:
	switch( (*(  token_.p)) ) {
		case 10: goto tr52;
		case 42: goto st42;
		case 47: goto tr54;
	}
	goto st41;
tr139:
#line 72 "csrparser.rl"
	{
    token_.curline++;
    token_.ls = token_.p;
  }
#line 77 "csrparser.rl"
	{
    save();
    reset();
  }
	goto st95;
tr54:
#line 77 "csrparser.rl"
	{
    save();
    reset();
  }
	goto st95;
st95:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof95;
case 95:
#line 813 "csrparser.cc"
	switch( (*(  token_.p)) ) {
		case 9: goto tr143;
		case 10: goto tr144;
		case 13: goto tr143;
		case 32: goto tr143;
		case 35: goto tr145;
		case 47: goto tr146;
		case 95: goto tr147;
		case 100: goto tr148;
	}
	if ( (*(  token_.p)) > 90 ) {
		if ( 97 <= (*(  token_.p)) && (*(  token_.p)) <= 122 )
			goto tr147;
	} else if ( (*(  token_.p)) >= 65 )
		goto tr147;
	goto tr0;
tr109:
#line 64 "csrparser.rl"
	{ 
    token_.ts = token_.p;
  }
	goto st43;
tr147:
#line 164 "csrparser.rl"
	{token_.type = TT_COMMENT;}
#line 64 "csrparser.rl"
	{ 
    token_.ts = token_.p;
  }
	goto st43;
tr153:
#line 93 "csrparser.rl"
	{
    token_.type = TT_FUNCTION_END; 
    token_.ts = token_.p;
  }
#line 64 "csrparser.rl"
	{ 
    token_.ts = token_.p;
  }
	goto st43;
st43:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof43;
case 43:
#line 859 "csrparser.cc"
	switch( (*(  token_.p)) ) {
		case 9: goto tr55;
		case 10: goto tr56;
		case 13: goto tr55;
		case 32: goto tr55;
		case 95: goto st43;
		case 123: goto tr58;
	}
	if ( (*(  token_.p)) < 65 ) {
		if ( 48 <= (*(  token_.p)) && (*(  token_.p)) <= 57 )
			goto st43;
	} else if ( (*(  token_.p)) > 90 ) {
		if ( 97 <= (*(  token_.p)) && (*(  token_.p)) <= 122 )
			goto st43;
	} else
		goto st43;
	goto tr0;
tr60:
#line 72 "csrparser.rl"
	{
    token_.curline++;
    token_.ls = token_.p;
  }
	goto st44;
tr55:
#line 98 "csrparser.rl"
	{
    if ( token_.type != TT_DISPOSABLE_FUNCTION )
      token_.type = TT_FUNCTION;
    save();
  }
	goto st44;
tr56:
#line 98 "csrparser.rl"
	{
    if ( token_.type != TT_DISPOSABLE_FUNCTION )
      token_.type = TT_FUNCTION;
    save();
  }
#line 72 "csrparser.rl"
	{
    token_.curline++;
    token_.ls = token_.p;
  }
	goto st44;
st44:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof44;
case 44:
#line 909 "csrparser.cc"
	switch( (*(  token_.p)) ) {
		case 9: goto st44;
		case 10: goto tr60;
		case 13: goto st44;
		case 32: goto st44;
		case 123: goto st45;
	}
	goto tr0;
tr62:
#line 72 "csrparser.rl"
	{
    token_.curline++;
    token_.ls = token_.p;
  }
	goto st45;
tr58:
#line 98 "csrparser.rl"
	{
    if ( token_.type != TT_DISPOSABLE_FUNCTION )
      token_.type = TT_FUNCTION;
    save();
  }
	goto st45;
tr77:
#line 88 "csrparser.rl"
	{
    token_.type = TT_PARAM_NAME;
    save();
  }
	goto st45;
tr92:
#line 138 "csrparser.rl"
	{ iface_.set_arry_len(token_.array_length); }
	goto st45;
st45:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof45;
case 45:
#line 948 "csrparser.cc"
	switch( (*(  token_.p)) ) {
		case 9: goto st45;
		case 10: goto tr62;
		case 13: goto st45;
		case 32: goto st45;
		case 58: goto tr63;
		case 95: goto tr63;
		case 101: goto tr64;
		case 105: goto tr65;
		case 111: goto tr66;
	}
	if ( (*(  token_.p)) > 90 ) {
		if ( 97 <= (*(  token_.p)) && (*(  token_.p)) <= 122 )
			goto tr63;
	} else if ( (*(  token_.p)) >= 65 )
		goto tr63;
	goto tr0;
tr63:
#line 64 "csrparser.rl"
	{ 
    token_.ts = token_.p;
  }
	goto st46;
st46:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof46;
case 46:
#line 976 "csrparser.cc"
	switch( (*(  token_.p)) ) {
		case 9: goto tr67;
		case 10: goto tr68;
		case 13: goto tr67;
		case 32: goto tr69;
		case 58: goto st67;
		case 60: goto st67;
		case 62: goto st46;
		case 95: goto st46;
	}
	if ( (*(  token_.p)) < 65 ) {
		if ( 48 <= (*(  token_.p)) && (*(  token_.p)) <= 57 )
			goto st46;
	} else if ( (*(  token_.p)) > 90 ) {
		if ( 97 <= (*(  token_.p)) && (*(  token_.p)) <= 122 )
			goto st46;
	} else
		goto st46;
	goto tr0;
tr73:
#line 72 "csrparser.rl"
	{
    token_.curline++;
    token_.ls = token_.p;
  }
	goto st47;
tr67:
#line 82 "csrparser.rl"
	{
    token_.type = TT_PARAM_TYPE;
    token_.array_length  = 0; /* reset array length */
    save();
  }
	goto st47;
tr68:
#line 82 "csrparser.rl"
	{
    token_.type = TT_PARAM_TYPE;
    token_.array_length  = 0; /* reset array length */
    save();
  }
#line 72 "csrparser.rl"
	{
    token_.curline++;
    token_.ls = token_.p;
  }
	goto st47;
st47:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof47;
case 47:
#line 1028 "csrparser.cc"
	switch( (*(  token_.p)) ) {
		case 9: goto st47;
		case 10: goto tr73;
		case 13: goto st47;
		case 32: goto st47;
		case 95: goto tr74;
	}
	if ( (*(  token_.p)) > 90 ) {
		if ( 97 <= (*(  token_.p)) && (*(  token_.p)) <= 122 )
			goto tr74;
	} else if ( (*(  token_.p)) >= 65 )
		goto tr74;
	goto tr0;
tr74:
#line 64 "csrparser.rl"
	{ 
    token_.ts = token_.p;
  }
	goto st48;
st48:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof48;
case 48:
#line 1052 "csrparser.cc"
	switch( (*(  token_.p)) ) {
		case 9: goto tr75;
		case 10: goto tr76;
		case 13: goto tr75;
		case 32: goto tr75;
		case 44: goto tr77;
		case 91: goto tr79;
		case 95: goto st48;
		case 125: goto tr80;
	}
	if ( (*(  token_.p)) < 65 ) {
		if ( 48 <= (*(  token_.p)) && (*(  token_.p)) <= 57 )
			goto st48;
	} else if ( (*(  token_.p)) > 90 ) {
		if ( 97 <= (*(  token_.p)) && (*(  token_.p)) <= 122 )
			goto st48;
	} else
		goto st48;
	goto tr0;
tr82:
#line 72 "csrparser.rl"
	{
    token_.curline++;
    token_.ls = token_.p;
  }
	goto st49;
tr75:
#line 88 "csrparser.rl"
	{
    token_.type = TT_PARAM_NAME;
    save();
  }
	goto st49;
tr76:
#line 88 "csrparser.rl"
	{
    token_.type = TT_PARAM_NAME;
    save();
  }
#line 72 "csrparser.rl"
	{
    token_.curline++;
    token_.ls = token_.p;
  }
	goto st49;
st49:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof49;
case 49:
#line 1102 "csrparser.cc"
	switch( (*(  token_.p)) ) {
		case 9: goto st49;
		case 10: goto tr82;
		case 13: goto st49;
		case 32: goto st49;
		case 44: goto st45;
		case 91: goto st50;
		case 125: goto tr84;
	}
	goto tr0;
tr85:
#line 72 "csrparser.rl"
	{
    token_.curline++;
    token_.ls = token_.p;
  }
	goto st50;
tr79:
#line 88 "csrparser.rl"
	{
    token_.type = TT_PARAM_NAME;
    save();
  }
	goto st50;
st50:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof50;
case 50:
#line 1131 "csrparser.cc"
	switch( (*(  token_.p)) ) {
		case 9: goto st50;
		case 10: goto tr85;
		case 13: goto st50;
		case 32: goto st50;
		case 93: goto st53;
	}
	if ( 48 <= (*(  token_.p)) && (*(  token_.p)) <= 57 )
		goto tr86;
	goto tr0;
tr86:
#line 104 "csrparser.rl"
	{
    token_.array_length = token_.array_length * 10 + ((*(  token_.p)) - '0');
  }
	goto st51;
st51:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof51;
case 51:
#line 1152 "csrparser.cc"
	switch( (*(  token_.p)) ) {
		case 9: goto st52;
		case 10: goto tr89;
		case 13: goto st52;
		case 32: goto st52;
		case 93: goto st53;
	}
	if ( 48 <= (*(  token_.p)) && (*(  token_.p)) <= 57 )
		goto tr86;
	goto tr0;
tr89:
#line 72 "csrparser.rl"
	{
    token_.curline++;
    token_.ls = token_.p;
  }
	goto st52;
st52:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof52;
case 52:
#line 1174 "csrparser.cc"
	switch( (*(  token_.p)) ) {
		case 9: goto st52;
		case 10: goto tr89;
		case 13: goto st52;
		case 32: goto st52;
		case 93: goto st53;
	}
	goto tr0;
st53:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof53;
case 53:
	switch( (*(  token_.p)) ) {
		case 9: goto tr90;
		case 10: goto tr91;
		case 13: goto tr90;
		case 32: goto tr90;
		case 44: goto tr92;
		case 125: goto tr93;
	}
	goto tr0;
tr95:
#line 72 "csrparser.rl"
	{
    token_.curline++;
    token_.ls = token_.p;
  }
	goto st54;
tr90:
#line 138 "csrparser.rl"
	{ iface_.set_arry_len(token_.array_length); }
	goto st54;
tr91:
#line 138 "csrparser.rl"
	{ iface_.set_arry_len(token_.array_length); }
#line 72 "csrparser.rl"
	{
    token_.curline++;
    token_.ls = token_.p;
  }
	goto st54;
st54:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof54;
case 54:
#line 1220 "csrparser.cc"
	switch( (*(  token_.p)) ) {
		case 9: goto st54;
		case 10: goto tr95;
		case 13: goto st54;
		case 32: goto st54;
		case 44: goto st45;
		case 125: goto tr84;
	}
	goto tr0;
tr80:
#line 88 "csrparser.rl"
	{
    token_.type = TT_PARAM_NAME;
    save();
  }
#line 64 "csrparser.rl"
	{ 
    token_.ts = token_.p;
  }
#line 77 "csrparser.rl"
	{
    save();
    reset();
  }
	goto st96;
tr84:
#line 64 "csrparser.rl"
	{ 
    token_.ts = token_.p;
  }
#line 77 "csrparser.rl"
	{
    save();
    reset();
  }
	goto st96;
tr93:
#line 138 "csrparser.rl"
	{ iface_.set_arry_len(token_.array_length); }
#line 64 "csrparser.rl"
	{ 
    token_.ts = token_.p;
  }
#line 77 "csrparser.rl"
	{
    save();
    reset();
  }
	goto st96;
st96:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof96;
case 96:
#line 1274 "csrparser.cc"
	switch( (*(  token_.p)) ) {
		case 9: goto tr149;
		case 10: goto tr150;
		case 13: goto tr149;
		case 32: goto tr149;
		case 35: goto tr151;
		case 47: goto tr152;
		case 95: goto tr153;
		case 100: goto tr154;
	}
	if ( (*(  token_.p)) > 90 ) {
		if ( 97 <= (*(  token_.p)) && (*(  token_.p)) <= 122 )
			goto tr153;
	} else if ( (*(  token_.p)) >= 65 )
		goto tr153;
	goto tr0;
tr148:
#line 164 "csrparser.rl"
	{token_.type = TT_COMMENT;}
	goto st55;
tr154:
#line 93 "csrparser.rl"
	{
    token_.type = TT_FUNCTION_END; 
    token_.ts = token_.p;
  }
	goto st55;
st55:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof55;
case 55:
#line 1306 "csrparser.cc"
	if ( (*(  token_.p)) == 105 )
		goto st56;
	goto tr0;
st56:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof56;
case 56:
	if ( (*(  token_.p)) == 115 )
		goto st57;
	goto tr0;
st57:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof57;
case 57:
	if ( (*(  token_.p)) == 112 )
		goto st58;
	goto tr0;
st58:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof58;
case 58:
	if ( (*(  token_.p)) == 111 )
		goto st59;
	goto tr0;
st59:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof59;
case 59:
	if ( (*(  token_.p)) == 115 )
		goto st60;
	goto tr0;
st60:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof60;
case 60:
	if ( (*(  token_.p)) == 97 )
		goto st61;
	goto tr0;
st61:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof61;
case 61:
	if ( (*(  token_.p)) == 98 )
		goto st62;
	goto tr0;
st62:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof62;
case 62:
	if ( (*(  token_.p)) == 108 )
		goto st63;
	goto tr0;
st63:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof63;
case 63:
	if ( (*(  token_.p)) == 101 )
		goto st64;
	goto tr0;
st64:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof64;
case 64:
	switch( (*(  token_.p)) ) {
		case 9: goto tr105;
		case 10: goto tr106;
		case 13: goto tr105;
		case 32: goto tr105;
	}
	goto tr0;
tr108:
#line 72 "csrparser.rl"
	{
    token_.curline++;
    token_.ls = token_.p;
  }
	goto st65;
tr105:
#line 120 "csrparser.rl"
	{token_.type     = TT_DISPOSABLE_FUNCTION;}
	goto st65;
tr106:
#line 120 "csrparser.rl"
	{token_.type     = TT_DISPOSABLE_FUNCTION;}
#line 72 "csrparser.rl"
	{
    token_.curline++;
    token_.ls = token_.p;
  }
	goto st65;
st65:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof65;
case 65:
#line 1401 "csrparser.cc"
	switch( (*(  token_.p)) ) {
		case 9: goto st65;
		case 10: goto tr108;
		case 13: goto st65;
		case 32: goto st65;
		case 95: goto tr109;
	}
	if ( (*(  token_.p)) > 90 ) {
		if ( 97 <= (*(  token_.p)) && (*(  token_.p)) <= 122 )
			goto tr109;
	} else if ( (*(  token_.p)) >= 65 )
		goto tr109;
	goto tr0;
tr69:
#line 82 "csrparser.rl"
	{
    token_.type = TT_PARAM_TYPE;
    token_.array_length  = 0; /* reset array length */
    save();
  }
	goto st66;
st66:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof66;
case 66:
#line 1427 "csrparser.cc"
	switch( (*(  token_.p)) ) {
		case 9: goto st47;
		case 10: goto tr73;
		case 13: goto st47;
		case 32: goto st66;
		case 58: goto st67;
		case 60: goto st67;
		case 62: goto st46;
		case 95: goto tr74;
	}
	if ( (*(  token_.p)) < 65 ) {
		if ( 48 <= (*(  token_.p)) && (*(  token_.p)) <= 57 )
			goto st46;
	} else if ( (*(  token_.p)) > 90 ) {
		if ( 97 <= (*(  token_.p)) && (*(  token_.p)) <= 122 )
			goto tr74;
	} else
		goto tr74;
	goto tr0;
st67:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof67;
case 67:
	switch( (*(  token_.p)) ) {
		case 32: goto st67;
		case 58: goto st67;
		case 60: goto st67;
		case 62: goto st46;
		case 95: goto st46;
	}
	if ( (*(  token_.p)) < 65 ) {
		if ( 48 <= (*(  token_.p)) && (*(  token_.p)) <= 57 )
			goto st46;
	} else if ( (*(  token_.p)) > 90 ) {
		if ( 97 <= (*(  token_.p)) && (*(  token_.p)) <= 122 )
			goto st46;
	} else
		goto st46;
	goto tr0;
tr64:
#line 64 "csrparser.rl"
	{ 
    token_.ts = token_.p;
  }
	goto st68;
st68:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof68;
case 68:
#line 1477 "csrparser.cc"
	if ( (*(  token_.p)) == 120 )
		goto st69;
	goto tr0;
st69:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof69;
case 69:
	if ( (*(  token_.p)) == 99 )
		goto st70;
	goto tr0;
st70:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof70;
case 70:
	if ( (*(  token_.p)) == 101 )
		goto st71;
	goto tr0;
st71:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof71;
case 71:
	if ( (*(  token_.p)) == 112 )
		goto st72;
	goto tr0;
st72:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof72;
case 72:
	if ( (*(  token_.p)) == 116 )
		goto st73;
	goto tr0;
st73:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof73;
case 73:
	if ( (*(  token_.p)) == 105 )
		goto st74;
	goto tr0;
st74:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof74;
case 74:
	if ( (*(  token_.p)) == 111 )
		goto st75;
	goto tr0;
st75:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof75;
case 75:
	if ( (*(  token_.p)) == 110 )
		goto st76;
	goto tr0;
st76:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof76;
case 76:
	if ( (*(  token_.p)) == 58 )
		goto tr119;
	goto tr0;
tr119:
#line 119 "csrparser.rl"
	{token_.modifier = MD_EXCEPTION;}
	goto st77;
tr129:
#line 118 "csrparser.rl"
	{token_.modifier = MD_INOUT;}
	goto st77;
tr132:
#line 116 "csrparser.rl"
	{token_.modifier = MD_INPUT;}
	goto st77;
tr138:
#line 117 "csrparser.rl"
	{token_.modifier = MD_OUTPUT;}
	goto st77;
st77:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof77;
case 77:
#line 1557 "csrparser.cc"
	switch( (*(  token_.p)) ) {
		case 9: goto tr120;
		case 10: goto tr121;
		case 13: goto tr120;
		case 32: goto tr120;
	}
	goto tr0;
tr123:
#line 72 "csrparser.rl"
	{
    token_.curline++;
    token_.ls = token_.p;
  }
	goto st78;
tr120:
#line 145 "csrparser.rl"
	{token_.type = TT_PARAM_MOD;}
	goto st78;
tr121:
#line 145 "csrparser.rl"
	{token_.type = TT_PARAM_MOD;}
#line 72 "csrparser.rl"
	{
    token_.curline++;
    token_.ls = token_.p;
  }
	goto st78;
st78:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof78;
case 78:
#line 1589 "csrparser.cc"
	switch( (*(  token_.p)) ) {
		case 9: goto st78;
		case 10: goto tr123;
		case 13: goto st78;
		case 32: goto st78;
		case 58: goto tr63;
		case 95: goto tr63;
	}
	if ( (*(  token_.p)) > 90 ) {
		if ( 97 <= (*(  token_.p)) && (*(  token_.p)) <= 122 )
			goto tr63;
	} else if ( (*(  token_.p)) >= 65 )
		goto tr63;
	goto tr0;
tr65:
#line 64 "csrparser.rl"
	{ 
    token_.ts = token_.p;
  }
	goto st79;
st79:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof79;
case 79:
#line 1614 "csrparser.cc"
	if ( (*(  token_.p)) == 110 )
		goto st80;
	goto tr0;
st80:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof80;
case 80:
	switch( (*(  token_.p)) ) {
		case 111: goto st81;
		case 112: goto st84;
	}
	goto tr0;
st81:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof81;
case 81:
	if ( (*(  token_.p)) == 117 )
		goto st82;
	goto tr0;
st82:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof82;
case 82:
	if ( (*(  token_.p)) == 116 )
		goto st83;
	goto tr0;
st83:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof83;
case 83:
	if ( (*(  token_.p)) == 58 )
		goto tr129;
	goto tr0;
st84:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof84;
case 84:
	if ( (*(  token_.p)) == 117 )
		goto st85;
	goto tr0;
st85:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof85;
case 85:
	if ( (*(  token_.p)) == 116 )
		goto st86;
	goto tr0;
st86:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof86;
case 86:
	if ( (*(  token_.p)) == 58 )
		goto tr132;
	goto tr0;
tr66:
#line 64 "csrparser.rl"
	{ 
    token_.ts = token_.p;
  }
	goto st87;
st87:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof87;
case 87:
#line 1679 "csrparser.cc"
	if ( (*(  token_.p)) == 117 )
		goto st88;
	goto tr0;
st88:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof88;
case 88:
	if ( (*(  token_.p)) == 116 )
		goto st89;
	goto tr0;
st89:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof89;
case 89:
	if ( (*(  token_.p)) == 112 )
		goto st90;
	goto tr0;
st90:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof90;
case 90:
	if ( (*(  token_.p)) == 117 )
		goto st91;
	goto tr0;
st91:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof91;
case 91:
	if ( (*(  token_.p)) == 116 )
		goto st92;
	goto tr0;
st92:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof92;
case 92:
	if ( (*(  token_.p)) == 58 )
		goto tr138;
	goto tr0;
st93:
	if ( ++(  token_.p) == ( token_.pe) )
		goto _test_eof93;
case 93:
	if ( (*(  token_.p)) == 10 )
		goto tr139;
	goto st93;
	}
	_test_eof94:  token_.cs = 94; goto _test_eof; 
	_test_eof1:  token_.cs = 1; goto _test_eof; 
	_test_eof2:  token_.cs = 2; goto _test_eof; 
	_test_eof3:  token_.cs = 3; goto _test_eof; 
	_test_eof4:  token_.cs = 4; goto _test_eof; 
	_test_eof5:  token_.cs = 5; goto _test_eof; 
	_test_eof6:  token_.cs = 6; goto _test_eof; 
	_test_eof7:  token_.cs = 7; goto _test_eof; 
	_test_eof8:  token_.cs = 8; goto _test_eof; 
	_test_eof9:  token_.cs = 9; goto _test_eof; 
	_test_eof10:  token_.cs = 10; goto _test_eof; 
	_test_eof11:  token_.cs = 11; goto _test_eof; 
	_test_eof12:  token_.cs = 12; goto _test_eof; 
	_test_eof13:  token_.cs = 13; goto _test_eof; 
	_test_eof14:  token_.cs = 14; goto _test_eof; 
	_test_eof15:  token_.cs = 15; goto _test_eof; 
	_test_eof16:  token_.cs = 16; goto _test_eof; 
	_test_eof17:  token_.cs = 17; goto _test_eof; 
	_test_eof18:  token_.cs = 18; goto _test_eof; 
	_test_eof19:  token_.cs = 19; goto _test_eof; 
	_test_eof20:  token_.cs = 20; goto _test_eof; 
	_test_eof21:  token_.cs = 21; goto _test_eof; 
	_test_eof22:  token_.cs = 22; goto _test_eof; 
	_test_eof23:  token_.cs = 23; goto _test_eof; 
	_test_eof24:  token_.cs = 24; goto _test_eof; 
	_test_eof25:  token_.cs = 25; goto _test_eof; 
	_test_eof26:  token_.cs = 26; goto _test_eof; 
	_test_eof27:  token_.cs = 27; goto _test_eof; 
	_test_eof28:  token_.cs = 28; goto _test_eof; 
	_test_eof29:  token_.cs = 29; goto _test_eof; 
	_test_eof30:  token_.cs = 30; goto _test_eof; 
	_test_eof31:  token_.cs = 31; goto _test_eof; 
	_test_eof32:  token_.cs = 32; goto _test_eof; 
	_test_eof33:  token_.cs = 33; goto _test_eof; 
	_test_eof34:  token_.cs = 34; goto _test_eof; 
	_test_eof35:  token_.cs = 35; goto _test_eof; 
	_test_eof36:  token_.cs = 36; goto _test_eof; 
	_test_eof37:  token_.cs = 37; goto _test_eof; 
	_test_eof38:  token_.cs = 38; goto _test_eof; 
	_test_eof39:  token_.cs = 39; goto _test_eof; 
	_test_eof40:  token_.cs = 40; goto _test_eof; 
	_test_eof41:  token_.cs = 41; goto _test_eof; 
	_test_eof42:  token_.cs = 42; goto _test_eof; 
	_test_eof95:  token_.cs = 95; goto _test_eof; 
	_test_eof43:  token_.cs = 43; goto _test_eof; 
	_test_eof44:  token_.cs = 44; goto _test_eof; 
	_test_eof45:  token_.cs = 45; goto _test_eof; 
	_test_eof46:  token_.cs = 46; goto _test_eof; 
	_test_eof47:  token_.cs = 47; goto _test_eof; 
	_test_eof48:  token_.cs = 48; goto _test_eof; 
	_test_eof49:  token_.cs = 49; goto _test_eof; 
	_test_eof50:  token_.cs = 50; goto _test_eof; 
	_test_eof51:  token_.cs = 51; goto _test_eof; 
	_test_eof52:  token_.cs = 52; goto _test_eof; 
	_test_eof53:  token_.cs = 53; goto _test_eof; 
	_test_eof54:  token_.cs = 54; goto _test_eof; 
	_test_eof96:  token_.cs = 96; goto _test_eof; 
	_test_eof55:  token_.cs = 55; goto _test_eof; 
	_test_eof56:  token_.cs = 56; goto _test_eof; 
	_test_eof57:  token_.cs = 57; goto _test_eof; 
	_test_eof58:  token_.cs = 58; goto _test_eof; 
	_test_eof59:  token_.cs = 59; goto _test_eof; 
	_test_eof60:  token_.cs = 60; goto _test_eof; 
	_test_eof61:  token_.cs = 61; goto _test_eof; 
	_test_eof62:  token_.cs = 62; goto _test_eof; 
	_test_eof63:  token_.cs = 63; goto _test_eof; 
	_test_eof64:  token_.cs = 64; goto _test_eof; 
	_test_eof65:  token_.cs = 65; goto _test_eof; 
	_test_eof66:  token_.cs = 66; goto _test_eof; 
	_test_eof67:  token_.cs = 67; goto _test_eof; 
	_test_eof68:  token_.cs = 68; goto _test_eof; 
	_test_eof69:  token_.cs = 69; goto _test_eof; 
	_test_eof70:  token_.cs = 70; goto _test_eof; 
	_test_eof71:  token_.cs = 71; goto _test_eof; 
	_test_eof72:  token_.cs = 72; goto _test_eof; 
	_test_eof73:  token_.cs = 73; goto _test_eof; 
	_test_eof74:  token_.cs = 74; goto _test_eof; 
	_test_eof75:  token_.cs = 75; goto _test_eof; 
	_test_eof76:  token_.cs = 76; goto _test_eof; 
	_test_eof77:  token_.cs = 77; goto _test_eof; 
	_test_eof78:  token_.cs = 78; goto _test_eof; 
	_test_eof79:  token_.cs = 79; goto _test_eof; 
	_test_eof80:  token_.cs = 80; goto _test_eof; 
	_test_eof81:  token_.cs = 81; goto _test_eof; 
	_test_eof82:  token_.cs = 82; goto _test_eof; 
	_test_eof83:  token_.cs = 83; goto _test_eof; 
	_test_eof84:  token_.cs = 84; goto _test_eof; 
	_test_eof85:  token_.cs = 85; goto _test_eof; 
	_test_eof86:  token_.cs = 86; goto _test_eof; 
	_test_eof87:  token_.cs = 87; goto _test_eof; 
	_test_eof88:  token_.cs = 88; goto _test_eof; 
	_test_eof89:  token_.cs = 89; goto _test_eof; 
	_test_eof90:  token_.cs = 90; goto _test_eof; 
	_test_eof91:  token_.cs = 91; goto _test_eof; 
	_test_eof92:  token_.cs = 92; goto _test_eof; 
	_test_eof93:  token_.cs = 93; goto _test_eof; 

	_test_eof: {}
	if ( (  token_.p) == eof )
	{
	switch (  token_.cs ) {
	case 1: 
	case 2: 
	case 3: 
	case 4: 
	case 5: 
	case 6: 
	case 7: 
	case 8: 
	case 9: 
	case 10: 
	case 11: 
	case 12: 
	case 13: 
	case 14: 
	case 15: 
	case 16: 
	case 17: 
	case 18: 
	case 19: 
	case 20: 
	case 21: 
	case 22: 
	case 23: 
	case 24: 
	case 25: 
	case 26: 
	case 27: 
	case 28: 
	case 29: 
	case 30: 
	case 31: 
	case 32: 
	case 33: 
	case 34: 
	case 35: 
	case 36: 
	case 37: 
	case 38: 
	case 39: 
	case 40: 
	case 41: 
	case 42: 
	case 43: 
	case 44: 
	case 45: 
	case 46: 
	case 47: 
	case 48: 
	case 49: 
	case 50: 
	case 51: 
	case 52: 
	case 53: 
	case 54: 
	case 55: 
	case 56: 
	case 57: 
	case 58: 
	case 59: 
	case 60: 
	case 61: 
	case 62: 
	case 63: 
	case 64: 
	case 65: 
	case 66: 
	case 67: 
	case 68: 
	case 69: 
	case 70: 
	case 71: 
	case 72: 
	case 73: 
	case 74: 
	case 75: 
	case 76: 
	case 77: 
	case 78: 
	case 79: 
	case 80: 
	case 81: 
	case 82: 
	case 83: 
	case 84: 
	case 85: 
	case 86: 
	case 87: 
	case 88: 
	case 89: 
	case 90: 
	case 91: 
	case 92: 
	case 93: 
#line 68 "csrparser.rl"
	{
    print_error();
  }
	break;
	case 96: 
#line 93 "csrparser.rl"
	{
    token_.type = TT_FUNCTION_END; 
    token_.ts = token_.p;
  }
	break;
	case 95: 
#line 164 "csrparser.rl"
	{token_.type = TT_COMMENT;}
	break;
#line 1936 "csrparser.cc"
	}
	}

	_out: {}
	}

#line 291 "csrparser.rl"

      if ( token_.cs == csrgen_error )
        return(1);

      printf("%s", iface_.to_string().c_str() );

      return(0);
    } 

  } /* namespace rpc */
} /* namespace csl */
