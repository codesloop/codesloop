/*
Copyright (c) 2008,2009, David Beck, Tamas Foldi
 
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

#ifndef _csl_rpc_csrparser_hh_included_
#define _csl_rpc_csrparser_hh_included_

#include "csl_common.hh"
#ifdef __cplusplus
#include "obj.hh"
#include "iface.hh"

namespace csl 
{
  /** @brief remote procedure call related classes
   * 
   * "rpc" namespace is responsible for codesloop
   * remote procedure call implementation, like
   * stub generation, serialization and transport 
   * management, etc. 
   */
  namespace rpc
  {
 
    /** @brief codesloop rpc interface parser */
    class csrparser : public csl::common::obj
    {
    public:
      /** @brief default constructor */
      csrparser();

      /** @brief parse input 
       *
       * @param start pointer to char * data to parse
       * @param end pointer of "eof"
       * @return 0 on success, otherwise 1
       */
      int parse(char * start, char * end); 

      /** @brief return parsed interface descriptor
       *
       * @return const pointer to interface descriptor
       */
      const iface * get_iface() const { return &iface_; }

    private:
      void reset();
      void save();
      void print_error(const char * error_msg) const;
      iface iface_;
      token_info token_;

      CSL_OBJ(csl::rpc,csrparser);
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_rpc_csrparser_hh_included_ */
