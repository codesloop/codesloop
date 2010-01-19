/*
Copyright (c) 2008,2009,2010, CodeSLoop Team

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

#ifndef _csl_common_zfile_hh_included_
#define _csl_common_zfile_hh_included_

/**
   @file zfile.hh
   @brief Helper class to read files and compress data

   Compression is based on zlib
*/

#include "codesloop/common/pbuf.hh"
#include "codesloop/common/str.hh"
#ifdef __cplusplus
#include <memory>
#include <vector>

namespace csl
{
  namespace common
  {
    /**
       @brief Helper for file I/O and compression

       The zfile helper uses two buffer lists. One for the uncompressed
       data and an other for the compressed data. Each buffer lists contains
       fixed sized buffers. Its size is determined at compile time by the
       CSL_COMMON_ZFILE_BLOCK_SIZE macro. Its default is 2048 bytes.

       The concept of the interface is to do the de/compression only when needed.

       zfile uses the pool allocator of the boost library. This tracks all
       allocated buffers and all will be freed when the zfile is destroyed.

       The interface is designed to hide the internal details of zlib, so
       if the compression method need to be changed, than only the
       implementation would change not the interface.

       Please note that compressed data does not contain the zlib header, so
       if external program interprets the data it may need to take care of that.
    */
    class zfile
    {
    public:
      /**
         @brief Reads the file into the uncompressed buffers
         @param filename The name of the file to be read
         @return true if successful, false otherwise

         If the buffers already have data then they will be discarded.
      */
      bool read_file(const char * filename);

      /**
         @brief Reads the file into the compressed buffers
         @param filename The name of the file to be read
         @return true if successful, false otherwise

         If the buffers already have data then they will be discarded.
         This function expects to read data that was compressed by zfile.
      */
      bool read_zfile(const char * filename);

      /**
         @brief Write the contents of the uncompressed data to file
         @param filename The name of the file to be written
         @return true if successful, false otherwise

         If only the compressed buffer has data then it gets uncompressed
         first.
      */
      bool write_file(const char * filename);

      /**
         @brief Write the contents of the compressed data to file
         @param filename The name of the file to be written
         @return true if successful, false otherwise

         If only the uncompressed buffer has data then it gets compressed
         first.
      */
      bool write_zfile(const char * filename);

      /**
         @brief Returns the size of the uncompressed buffer
         @return the size

         If only the compressed buffer has data then it gets uncompressed
         first.
      */
      uint64_t get_size();

      /**
      @brief Returns the size of the uncompressed buffer
      @return the size

      Even if only the compressed buffer has data it will not uncompress it.
      If that is the case it returns 0.
       */
      uint64_t get_size_const() const;

      /**
         @brief The data of the uncompressed buffers will be placed into dta
         @param dta where to place the data
         @return true if successful, false otherwise

         If only the compressed buffer has data then it gets uncompressed
         first. dta is expected to have enough space to be able to store
         the data.
      */
      bool get_data(unsigned char * dta);

      /**
         @brief The data of the uncompressed buffers will be placed into dta
         @param dta where to place the data
         @return true if successful, false otherwise

         It only returns data if the uncompressed buffer has data.
       */
      bool get_data_const(unsigned char * dta) const;

      /**
         @brief The data of the uncompressed buffers will be placed into dta
         @param dta where to place the data
         @return true if successful, false otherwise

         If only the compressed buffer has data then it gets uncompressed
         first. dta is expected to have enough space to be able to store
         the data.
       */
      bool get_data(pbuf & dta);

      /**
         @brief The data of the uncompressed buffers will be placed into dta
         @param dta where to place the data
         @return true if successful, false otherwise

         It only returns data if the uncompressed buffer has data.
       */
      bool get_data_const(pbuf & dta) const;

      /**
         @brief The data of the uncompressed buffers will be returned
         @return the internal uncompressed data buffer

         If only the compressed buffer has data then it gets uncompressed
         first.
       */
      const pbuf & get_data();

      /**
         @brief Puts data into the uncompressed buffers
         @param dta pointer to the data
         @param len the length of the data to be put
         @return true if successful, false otherwise

         If the buffers already have data then they will be discarded.
      */
      bool put_data(const unsigned char * dta, uint64_t len);

      /**
         @brief Puts data into the uncompressed buffers
         @param dta is the buffer to be put into zfile
         @return true if successful, false otherwise

         If the buffers already have data then they will be discarded.
       */
      bool put_data(const pbuf & buf);

      /**
         @brief Debug function to see what is inside the uncompressed buffer
         @param str Reference of the output string
         @return true if successful, false otherwise
      */
      bool dump_data(common::str & str);

      /**
         @brief Allocates a continous buffer and puts the uncompressed data into that
         @return the buffer

         Allocates a continous buffer and puts the uncompressed data into that.
         If only the compressed buffer has data then it gets uncompressed
         first. The returned pointer must not be freed. It will be deallocated
         by the given zfile object.
      */
      unsigned char * get_buff();

      /**
         @brief Returns the compressed buffer size
         @return the size

         If only the uncompressed buffer has data then it gets compressed
         first.
      */
      uint64_t get_zsize();

      /**
      @brief Returns the compressed buffer size
      @return the size

      If there is no compressed data, then it will not compress the data.
      If that is the case it returns 0.
       */
      uint64_t get_zsize_const() const;

      /**
         @brief The data of the compressed buffers will be placed into dta
         @param dta where to place the data
         @return true if successful, false otherwise

         If only the uncompressed buffer has data then it gets compressed
         first. dta is expected to have enough space to be able to store
         the data.
      */
      bool get_zdata(unsigned char * dta);

      /**
         @brief The data of the compressed buffers will be placed into dta
         @param dta where to place the data
         @return true if successful, false otherwise

         It only returns true if the compressed buffer has data.
       */
      bool get_zdata_const(unsigned char * dta) const;

      /**
         @brief The data of the compressed buffers will be placed into dta
         @param dta where to place the data
         @return true if successful, false otherwise

         If only the uncompressed buffer has data then it gets compressed
         first. dta is expected to have enough space to be able to store
         the data.
       */
      bool get_zdata(pbuf & dta);

      /**
         @brief The data of the compressed buffers will be placed into dta
         @param dta where to place the data
         @return true if successful, false otherwise

         It only returns true if the compressed buffer has data.
       */
      bool get_zdata_const(pbuf & dta) const;

      /**
         @brief The data of the compressed buffers will be returned
         @return the internal compressed data buffer

         If only the uncompressed buffer has data then it gets compressed
         first.
       */
      const pbuf & get_zdata();

      /**
         @brief Puts data into the compressed buffers
         @param dta pointer to the data
         @param len the length of the data to be put
         @return true if successful, false otherwise

         If the buffers already have data then they will be discarded.
      */
      bool put_zdata(const unsigned char * dta, uint64_t len);

      /**
         @brief Puts data into the compressed buffers
         @param dta is the buffer to be put into zfile
         @return true if successful, false otherwise

         If the buffers already have data then they will be discarded.
       */
      bool put_zdata(const pbuf & dta);

      /**
         @brief Debug function to see what is inside the compressed buffer
         @param str Reference of the output string
         @return true if successful, false otherwise
      */
      bool dump_zdata(common::str & str);

      /**
         @brief Allocates a continous buffer and puts the compressed data into that
         @return the buffer

         Allocates a continous buffer and puts the compressed data into that.
         If only the uncompressed buffer has data then it gets compressed
         first. The returned pointer must not be freed. It will be deallocated
         by the given zfile object.
      */
      unsigned char * get_zbuff();

      /**
         @brief Drops all data from the uncompressed buffers
         @return true if successful, false otherwise

         Success means there was data to be dropped and it was freed.
      */
      bool drop_data();

      /**
         @brief Drops all data from the compressed buffers
         @return true if successful, false otherwise

         Success means there was data to be dropped and it was freed.
      */
      bool drop_zdata();

      /**
         @brief Instruct zlib to use the pool allocator
         @param yesno use it or no

         Originally this function is provided to make valgrind happy.
         There are numerous errors reported if zlib internal allocator
         is used.
      */
      void custom_zlib_allocator(bool yesno);

      /**
         @brief Instruct the pool allocator to initialize the allocated memory
         @param yesno initialize or no

         Originally this function is provided to make valgrind happy.
         There are numerous errors reported if zlib internal allocator
         is used.
      */
      void init_custom_memory(bool yesno);

      /**
         @brief Compare the uncompressed data of zfile to other's
         @param other is the other zfile instance

         This functions is intended to be used for testing because of performance
         reasons. This function copies out the uncompressed data of both zfiles
         and then compares the two. This may result in a decompression.
       */
      bool operator==(const zfile & other) const;

      zfile();
      ~zfile();

      zfile(const zfile & other);
      zfile & operator=(const zfile & other);

      struct impl;
    private:
      friend class rd_iter;

      /* private data */
      std::auto_ptr<impl> impl_;
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_common_zfile_hh_included_ */
