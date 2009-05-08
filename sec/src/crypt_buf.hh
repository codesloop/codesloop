/*
Copyright (c) 2008,2009, David Beck

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

#ifndef _csl_sec_crypt_buf_hh_included_
#define _csl_sec_crypt_buf_hh_included_

/**
   @file crypt_buf.hh
   @brief Helper class to encrypt and decrypt data

   Encryption is based on OpenSSL and Blowfish
*/

#include "common.h"
#ifdef __cplusplus
#include <memory>

namespace csl
{
  namespace sec
  {
    /**
       @brief Helper for en/decryption

       crypt_buf en/decrypts data in place, so no output buffer is used.

       The encryption is done in three steps:

       @li Initialization
       @li En/Decryption
       @li Checksum/MAC calculation

       The initialization part does two things, initialize the internal variables
       and generates a random number to be fed to the encryption. This random
       number is used to ensure that different runs generate different results and
       also used by the Checksum calculation.

       En/Decryption has a streaming like interface, where each call to add_data()
       adds new plain/encrypted parts to the stream.
       The data buffer don't have to be aligned or padded.

       When all data was en/decrypted then the finalize() function generates the
       Checksum/MAC. This can be used to check message integrity.

       The crypt_buf class uses the blowfish encryption provided by the OpenSSL
       library. The MAC generation is done by creating SHA1 sum of the initial
       random number, that will be fed to the encryption at the end. Since
       every byte effects the next bytes in the stream then this would be sufficient
       to ensure message integrity.

       The interface is designed to hide the internal details of OpenSSL and
       the encryption function used, so if they need to be changed, than only
       the implementation would change not the interface.
    */
    class crypt_buf
    {
    public:
      /**
         @brief how many bytes needed to store the message header
         @return number of bytes
      */
      size_t get_header_len();

      /**
         @brief how many bytes needed to store the message checksum
         @return number of bytes
      */
      size_t get_mac_len();

      /**
         @brief initialization by C string key
         @param buf the output buffer where the header will be placed
         @param key the en/decryption key, a zero terminated C string
         @param encrypt wether we do encrypt or decrypt now
         @return true if successful, false otherwise

         The function does not allocate memory for buf, it rather expects that
         buf has at least get_header_len() bytes preallocated. The key is expected
         to be a zero terminated C string.

         The key is cut to a multiple of 32 bits as Blowfish expects that. For
         security reasons at least 12 bytes of key is needed and it should not
         be longer than 56 bytes.
      */
      bool init_crypt(unsigned char * buf,const unsigned char * key,bool encrypt);

      /**
         @brief initialization by memory buffer key
         @param buf the output buffer where the header will be placed
         @param key the en/decryption key, a binary memory buffer
         @param keylen the length of the en/decryption key buffer
         @param encrypt wether we do encrypt or decrypt now
         @return true if successful, false otherwise

         The function does not allocate memory for buf, it rather expects that
         buf has at least get_header_len() bytes preallocated. The key can be
         any number of bytes between 12 and 56 bytes.

         The key is cut to a multiple of 32 bits as Blowfish expects that.
      */
      bool init_crypt(
        unsigned char * buf,
        const unsigned char * key,
        size_t keylen,
        bool encrypt);

      /**
         @brief en/decrypt data
         @param buf the buffer to be encrypted
         @param len how many bytes the buffer has
         @param encrypt wether we do encrypt or decrypt now
         @return true if successful, false otherwise

         Please note that adding data to the buffer changes both the MAC
         and the encryption of the next buffers, so the order of adding
         data does matter.
      */
      bool add_data(unsigned char * buf, size_t len, bool encrypt);

      /**
         @brief generate checksum/MAC
         @param outbuff the buffer where the checksum will be placed
         @return true if successful, false otherwise

         The finalize function should be called when no more data to be added.
         finalize() expects that outbuff has at least get_mac_len() bytes
         preallocated.
      */
      bool finalize(unsigned char * outbuff);

      crypt_buf();
      ~crypt_buf();

      struct impl;
    private:
      /* private data */
      std::auto_ptr<impl> impl_;
      /* copying not allowed */
      crypt_buf(const crypt_buf & other);
      crypt_buf & operator=(const crypt_buf & other);
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_sec_crypt_buf_hh_included_ */
