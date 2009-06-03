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

  ws = ('\t'|' '|'\r') @{printf("ws\n");};

  newline = '\n' @newline;

  comment =  '/*' any* :>>  '*/' @{printf("comment\n");};

  main  := (ws | comment | newline )*  $!print_err;

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
