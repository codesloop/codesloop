/*
Copyright (c) 2008,2009, Tamas Foldi

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

Credits: some techniques and code pieces are stolen from Christian 
         Stigen Larsen http://csl.sublevel3.org/programming/my_str/
*/

#ifndef _csl_common_str_hh_included_
#define _csl_common_str_hh_included_

/**
   @file str.hh
   @brief simple but efficient str class
   @todo document me
 */

#include <stdlib.h>
#include <string.h>
#include "obj.hh"
#include "exc.hh"
#ifdef __cplusplus

namespace csl
{
  namespace common
  {
    /** @todo document me */
    class str : public obj 
    {      
      public:
        enum { buf_size = 2048 };

        /** @brief constructor */
        str();
        
        /** @brief destructor 
        since there are not virtual functions, and we do not excpect inherited
        classes from str the destructor is not virtual. this casues a bit
        faster initalization, because no vptr table is required, i guess 
        */
        ~str();

        /** @brief copy constructor */
        str(const str&);
        
        /** @brief copy constructor */
        str(const char*);
        
        /** @brief let equal operator */
        str& operator=(const char*);
        /** @brief let equal operator */
        str& operator=(const str&);

        /** @brief append operator */
        str& operator+=(const str&);
        /** @brief append operator with two parameters */
        inline friend str operator+(const str& lhs, const str& rhs)
        {
          return str(lhs) += rhs; 
        }

        /** @brief is equal operator */
        inline bool operator==(const char* s) const
        {
          return !strcmp(p, s);
        }
        /** @brief is equal operator */
        inline bool operator==(const str& s) const
        {
          return !strcmp(p, s.c_str());
        }

        /** @brief resets str buffer */
        void clear();

        /** @brief gets str size 

          since in the most cases we do not need the size of the
          str the size is calculated only on the first demand
        */
        inline size_t size() const
        {
          return empty() ? 0 : strlen(p);
        }

        /** @brief true if empty str ("") is defined */
        inline bool empty() const
        {
          return *p == '\0';
        } 
        
        /** @brief returns the background c str */
        inline const char* c_str() const
        {
          return(p);
        }
        
        /**@brief substr 
           @param start start from this position
           @param length take length bytes from origin str
        */
        str substr(const size_t start, const size_t length) const;

        /**@brief unchecked access to buffer */
        inline char operator[](const size_t n) const
        {
          return p[n];
        }

        /** @brief check access to buffer */
        inline char at(const size_t n) const
        {
          if ( n > strlen(p) )
            throw exc(exc::rs_invalid_param,exc::cm_str);

          return p[n];
        }

        str& erase(size_t pos, size_t len);          

      private:
        char* p;
        char pbuf_[buf_size];  
       
        /** @brief true if the str fits into preallocated buffer */
        inline bool is_prebuf() const
        {
          return p == pbuf_;
        }
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_common_str_hh_included_ */
