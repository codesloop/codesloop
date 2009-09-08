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

#ifndef _csl_common_mpool_hh_included_
#define _csl_common_mpool_hh_included_

/**
   @file mpool.hh
   @brief mpool is a template class to collect dinamically allocated memory blocks

   mpool is parametrized by the container class that stores the pointers, its
   default is pvlist
*/

#include "codesloop/common/pvlist.hh"
#include "codesloop/common/common.h"
#ifdef __cplusplus

namespace csl
{
  namespace common
  {
    /**
       @brief collects dinamically allocated memory blocks

       mpool is parametrized by the container class that stores the pointers, its
       default is pvlist
     */
    template <typename P=pvlist< 64,void,free_destructor<void> > >
    class mpool
    {
    private:
      P v_;

      mpool(const mpool & other) {}
      mpool & operator=(const mpool & other) { return *this; }

    public:
      /** @brief constructor */
      inline mpool() {}

      /** 
       @brief allocates memory w/ malloc() and stores the result 
       @param len is the length to be allocated
       */
      inline void * allocate(size_t len)
      {
        if( !len ) { return 0; }
        void * ret = ::malloc( len );
        v_.push_back(ret);
        return ret;
      }

      /**
      @brief get n-th pointer of pool
      @param which is the item position
      */
      inline void * get_at(size_t which) const
      {
        return v_.get_at(which);
      }

      /**
      @brief duplicates the given string (allocates memory from pool)
      @param str is the string to be duplicated
       */
      inline char * strdup(const char * str)
      {
        if( !str ) return 0;
        size_t len = ::strlen(str);
        char * ret = 0;
        ret = reinterpret_cast<char *>(allocate(len+1));
        if( len ) ::memcpy(ret,str,len);
        ret[len] = 0;
        return ret;
      }

      /**
        @brief duplicates the given string (allocates memory from pool)
        @param str is the string to be duplicated
      */
      inline wchar_t * wcsdup(const wchar_t * str)
      {
        if( !str ) return 0;
        size_t len = ::wcslen(str);
        wchar_t * ret = 0;
        ret = reinterpret_cast<wchar_t *>(allocate((len+1)*sizeof(wchar_t)));
        if( len )
        {
          wcsncpy(ret,str,len);
          ret[len] = L'\0';
        }
        return ret;
      }

      /**
      @brief duplicates the given memory region (allocates memory from pool)
      @param ptr is the start of the memory region to be duplicated
      @param sz is the size of the memory region
       */
      inline void * memdup(const void * ptr, unsigned int sz)
      {
        if( !ptr || !sz ) return 0;
        void * ret = 0;
        ret = allocate(sz);
        ::memcpy(ret,ptr,sz);
        return ret;
      }

      /** @brief calls the container's debug() function */
      inline void debug()
      {
        v_.debug();
      }

      /**
       @brief calls the container's free_all() function

       frees all pointers in the pool
      */
      inline void free_all()
      {
        v_.free_all();
      }

      /**
       @brief calls the container's free_one() function 
       @param p is the pointer to be freed

       the function assumes, that only this class inserts pointers
       w/ allocate() and thus free_one() is sufficient, because the other
       functions only insert fresh new objects, so there is
       no need to scan the whole collection as the standard free() function
       of the container would do
       */
      inline bool free(void * p)
      {
        return v_.free_one(p);
      }

      /**
       @brief calls the container's find(p) function
       @param p is the pointer to be found

       checks wether a given pointer is allocated from this pool
       */
      inline bool is_from(void * p)
      {
        return v_.find(p);
      }

      /**
       @brief calls the container's find(p) function
       @param p is the pointer to be found

       the same functionality as is_from(). it checks wether a given
       pointer is allocated from this pool
       */
      inline bool find(void * p)
      {
        return v_.find(p);
      }
    };

    typedef mpool<> default_mpool_t;
  }
}

#endif /* __cplusplus */
#endif /* _csl_common_mpool_hh_included_ */

