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

#ifndef _csl_common_pbuf_hh_included_
#define _csl_common_pbuf_hh_included_

/**
   @file pbuf.hh
   @brief Paged buffer management
 */

#include "codesloop/common/pvlist.hh"
#include "codesloop/common/mpool.hh"
#include "codesloop/common/obj.hh"
#include "codesloop/common/serializable.hh"
#ifdef __cplusplus

namespace csl
{
  namespace common
  {
    /**
    @brief Paged buffer management

    pbuf manages a list of fixed sized buffers and each buffer has pbuf::buf_size memory.
    internal buffers are represented by pbuf::buf structure that stores a pointer to the
    memory location and the size of the used memory. this size cannot be more than buf::buf_size

    pbuf::buf items are stored in a bufpool_t structure which is a pvlist. iterators over
    the internal data are provided, so it can be iterated over without the need of calling
    other accessors.

    an initial buffer is allocated on the stack, so if the memory needed is less than buf_size
    than it is a lot faster
    */
    class pbuf : public obj, public serializable
    {
      public:
        enum { buf_size = 2048 };

        /** @brief constructor */
        pbuf();

        /** @brief destructor */
        ~pbuf() {}

        /** @brief buf represents a memory region */
        struct buf
        {
          unsigned char * data_; ///<pointer to the allocated data
          unsigned int    size_; ///<used size

          unsigned int free_space()   { return buf_size - size_; }
          unsigned char * data_here() { return (size_ == buf_size ? 0 : (data_+size_)); }

          /** @brief constructor */
          buf() : data_(0), size_(0) {}
        };

        /**
        @brief appends the data pointed by dta to the internal buffers
        @param dta is the location
        @param sz is the amount of memory to be appended
        @return true if successful
        */
        bool append(const unsigned char * dta, unsigned int sz);

        /**
        @brief appends the string pointed by str to the internal buffers
        @param str is the string
        @return true if successful

        please note that the trailing zero is also appended
         */
        bool append(const char * str)
        {
          unsigned int l=0;
          if( !str || (l=::strlen(str))== 0 ) return false;
          return append( reinterpret_cast<const unsigned char *>(str),(l+1));
        }

        /**
        @brief appends the string pointed by str to the internal buffers
        @param str is the string
        @return true if successful

        please note that the trailing zero is also appended
         */
        bool append(const wchar_t * str)
        {
          unsigned int l=0;
          if( !str || (l=::wcslen(str))== 0 ) return false;
          return append( reinterpret_cast<const unsigned char *>(str),(l+1)*sizeof(wchar_t));
        }

        /**
        @brief appends a string to the buffer
        @param str is the string to be appended

        please note that the trailing zero is also appended
         */
        pbuf & operator<<(const char * str) { append(str); return *this; }

        /**
        @brief appends a wide character string to the buffer
        @param str is the string to be appended

        please note that the trailing zero is also appended
         */
        pbuf & operator<<(const wchar_t * str) { append(str); return *this; }

        /**
        @brief copy to template
        @param t is the destination where to copy the pbuf data
        @param max_size may give an upper limit of the allocated space

        this templated function may be used to copy the pbuf content to the
        templated parameter t. t must have an allocate() function that returns
        a pointer to the allocated buffer.
         */
        template <typename T> bool t_copy_to(T & t,unsigned int max_size=0)
        {
          if( !max_size ) max_size = size();
          if( max_size )
          {
            unsigned char * bf = t.allocate(max_size);
            return copy_to(bf,max_size);
          }
          return false;
        }

        /**
        @brief copies the internal data to the specified location
        @param ptr is the location
        @return true if successful

        this function assumes that the caller allocated the neccessary amount of memory
         */
        bool copy_to(unsigned char * ptr, unsigned int max_size=0) const;

        /** @brief returns the amount of data stored */
        inline unsigned int size() const   { return size_; }

        /** @brief returns the amount of bufs allocated */
        inline unsigned int n_bufs() const { return bufpool_.n_items(); }

        typedef pvlist< 32,buf,delete_destructor<buf> > bufpool_t; ///<the buffer pool type
        typedef bufpool_t::iterator iterator;                      ///<iterator over the buffer pool
        typedef bufpool_t::const_iterator const_iterator;          ///<constant iterator over the buffer pool

        inline iterator begin() { return bufpool_.begin(); }  ///<iterator to the first page
        inline iterator end()   { return bufpool_.end();   }  ///<iterator after the last page
        inline iterator last()  { return bufpool_.last();  }  ///<iterator to the last page

        inline const_iterator begin() const { return bufpool_.begin(); } ///<constant iterator to the first page
        inline const_iterator end() const   { return bufpool_.end();   } ///<constant iterator after the last page
        inline const_iterator last() const  { return bufpool_.last();  } ///<constant iterator to the last page

        inline const_iterator const_begin() const { return bufpool_.const_begin(); } ///<constant iterator to the first page
        inline const_iterator const_end() const   { return bufpool_.const_end();   } ///<constant iterator after the last page
        inline const_iterator const_last() const  { return bufpool_.const_last();  } ///<constant iterator to the last page

        /** @brief free all allocated data */
        inline void free_all()
        {
          bufpool_.free_all();
          pool_.free_all();
          size_ = 0;
        }

        /** @brief tests if equal

        iterates over each buffer and compares them one by one
        */
        bool operator==(const pbuf & other) const;

        pbuf(const pbuf & other);              ///<copy constructor
        pbuf & operator=(const pbuf & other);  ///<copy operator

        /**
          @brief serialize contents of objects
          @param buf archiver class to/from serialize
          @throw common::exc
         */
        virtual void serialize(arch & ar);

      private:
        buf * allocate(unsigned int size);

        /* variables */
        unsigned int    size_;
        unsigned char   preallocated_[buf_size];
        bufpool_t       bufpool_;
        mpool<>         pool_;
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_common_pbuf_hh_included_ */
