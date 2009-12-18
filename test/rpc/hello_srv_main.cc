/*
Copyright (c) 2008,2009, CodeSLoop Team

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

#include "hello_srv.hh"

/**
  @file test/rpc/hello_srv_main.cc
  @brief main function for hello world test case
 */

using csl::common::str;

namespace csl 
{
  namespace rpc 
  {
    namespace hello 
    {
      class hello_impl : public hello_srv
      {
        CSL_OBJ(csl::rpc::hello,hello);
      public:
        virtual void hello( const csl::rpc::client_info & ci, 
            const int & n_times, 
            const csl::common::str & hello_to, 
            csl::common::str & result)        
        {
          ENTER_FUNCTION();
          printf("Hello %ls %d times\n", hello_to.c_str(), n_times );
          csl::common::logger::info( str( L"Hello ")  + hello_to ); 
          LEAVE_FUNCTION();
        }

      };

    };
  };
};


int main()
{
    csl::rpc::hello::hello_impl srv;

    srv.listen( "127.0.0.1", 12321 );

    exit(0);
}

/* EOF */
