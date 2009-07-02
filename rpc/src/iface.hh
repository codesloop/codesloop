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
 
#ifndef _csl_rpc_iface_hh_included_
#define _csl_rpc_iface_hh_included_

#include "common.h"
#ifdef __cplusplus
#include <string>
#include <vector>
#include "obj.hh"
#include "csrgen.hh"

namespace csl 
{ 
  namespace rpc 
  {
    /** @brief stores parsed interface description */
    class iface : public csl::common::obj
    {
      public:
        /** @brief structure to hold information about a parameter */ 
        struct param {
          std::string type; ///< parameter's C++ type name
          std::string name; ///< parameter name
          param_kind kind;  ///< parameter's kind (eg. input, output, etc.)
          bool is_array;    ///< true when parameter is an array 
          size_t array_length;  ///< length of an array
        };
        
        /** @brief contains a function and its parameters */
        struct func {
          std::string name;           ///< function name
          bool disposable;            ///< true when invoker can omit return values
          std::vector<param> params;  ///< parameters in original order
          typedef std::vector<param>::const_iterator 
            param_iterator;           ///< iterator for parameters
        };

        /*
         * setters
         */
        void set_name(const token_info &);    ///< sets interface name
        void set_version(const token_info &); ///< sets version string
        void set_namespc(const token_info &); ///< sets namespace 

        void add_function(const token_info &);   ///< adds one function 
        void set_param_type(const token_info &); ///< adds a parameter type 
        void set_param_name(const token_info &); ///< adds a parameter name
        void set_arry_len(int);                  ///< sets parameter's array attribute

        /** @brief adds an include statement from interface file */
        void add_include(const token_info &);   

        /*
         * getters 
         */
        /** @breif returns interface name */
        const std::string get_name() const { return name_;}
        /** @breif returns interface version */
        const std::string get_version() const { return version_;}
        /** @breif returns interface namespace */
        const std::string get_namespc() const { return namespc_;}
        /** @brief return list of defined functions */
        const std::vector<func> * get_functions() const 
        {
          return &functions_;
        }

        const std::vector<std::string> * get_includes() const
        {
          return &includes_;
        }

        /** @brief dump iface content (for debug) */
        std::string to_string() const;        


        /** @brief iterator to access includes */
        typedef std::vector<std::string>::const_iterator include_iterator;
        /** @brief iterator to access function */  
        typedef std::vector<func>::const_iterator function_iterator;
      private:
        std::string name_;
        std::string version_;
        std::string namespc_;

        std::string token_to_string(const token_info & ) const;
        std::string param_type_;

        std::vector<func> functions_;
        std::vector<std::string> includes_;
    };


  }
}

#endif /* __cplusplus */
#endif /* _csl_rpc_iface_hh_included_ */
