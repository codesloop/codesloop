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
         Stigen Larsen http://csl.sublevel3.org/programming/my_string/
*/

#ifndef _csl_common_str_hh_included_
#define _csl_common_str_hh_included_

/**
   @file str.hh
   @brief simple but efficient string class
   @todo document me
 */

#include <stdlib.h>
#include <string.h>
#include "obj.hh"
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
        string();
        
        /** @brief destructor 
        since there are not virtual functions, and we do not excpect inherited
        classes from str the destructor is not virtual. this casues a bit
        faster initalization, because no vptr table is required, i guess 
        */
        ~string();

        /** @brief copy constructor */
        string(const string&);
        
        /** @brief copy constructor */
        string(const char*);
        
        /** @brief let equal operator */
        string& operator=(const char*);
        /** @brief let equal operator */
        string& operator=(const string&);

        /** @brief append operator */
        string& operator+=(const string&);
        /** @brief append operator with two parameters */
        inline friend string operator+(const string& lhs, const string& rhs)
        {
          return string(lhs) += rhs; 
        }

        /** @brief is equal operator */
        inline bool operator==(const char* s) const
        {
          return !strcmp(p, s);
        }
        /** @brief is equal operator */
        inline bool operator==(const string& s) const
        {
          return !strcmp(p, s.c_str());
        }

        /** @brief resets string buffer */
        void clear();

        /** @brief gets string size 

          since in the most cases we do not need the size of the
          string the size is calculated only on the first demand
        */
        inline size_t size() const
        {
          return empty() ? 0 : strlen(p);
        }

        /** @brief true if empty string ("") is defined */
        inline bool empty() const
        {
          return *p == '\0';
        } 
        
        /** @brief returns the background c string */
        inline const char* c_str() const
        {
          return(p);
        }
        
        /**@brief substring 
           @param start start from this position
           @param length take length bytes from origin string
        */
        string substr(const size_t start, const size_t length) const;

        /**@brief unchecked access to buffer */
        inline char operator[](const size_t n) const
        {
          return p[n];
        }

        /** @brief check access to buffer */
        inline char at(const size_t n) const
        {
          if ( n > strlen(p) )
            throw exc(); // TODO: use exception framework

          return p[n];
        }

        string& erase(size_t pos, size_t len);          

      private:
        char* p;
        char pbuf_[buf_size];  
       
        /** @brief true if the string fits into preallocated buffer */
        inline bool is_prebuf() const
        {
          return p == pbuf;
        }
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_common_str_hh_included_ */
