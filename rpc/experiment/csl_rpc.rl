#include <string.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

%%{

machine  foo;
main  :=
         ((  "foo"  |  "bar"  )+ '\n')+ @{res++;};
         
}%%

%% write data;

int  main(  int  argc,  char  **argv  )
{
  int  cs,  res  =  0;
  stringstream buffer;
  string line;

  while ( cin >> line)
    buffer << line << endl;
            

  char  *p  = (char*) buffer.str().c_str() ;
  char  *pe  =  p  +  strlen(p)  +  1;
  printf("<< buffer >>\n%s<< buffer >>\n",p);


  %%  write  init;
  %%  write  exec;

  printf("result  =  %i\n",  res  );
  return  0;
}
