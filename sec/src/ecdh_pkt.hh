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

#ifndef _csl_sec_ecdh_pkt_hh_included_
#define _csl_sec_ecdh_pkt_hh_included_

/**
   @file ecdh_pkt.hh
   @brief Helper class to generate challenge and response

   Key generation is based on OpenSSL and Elliptic Curve DH cryptography.
   Internal buffer and pool management is implemented with the help of
   the Boost pool library.
*/

#include "common.h"
#include "pbuf.hh"
#ifdef __cplusplus
#include <memory>
#include <vector>
#include <string>

namespace csl
{
  namespace sec
  {
    /**
       @brief Helper to generate challenge and the corresponding response

       The role of ecdh_pkt is to generate a challenge and process the response
       and generate the shared secret: the session key. The party that generates
       the challenge is refered as server and the other side is client. This
       is just terminology. In real life different scenarios can be imagined.

       ecdh_pkt has no transport logic built in. It works on buffers and it is
       not its responsibility to transfer these.

       ecdh_pkt is not able to encrypt external buffers or streams. However it
       does encrypt the credential information (login and password) using the
       generated session key and crypt_buf.

       The generated packets are encoded using XDR under the cover.

       The implementation details of OpenSSL, ECDH cryptography and XDR are
       not exposed to the public interface. This is a design decision to make
       the interface stable and the implementation could be changed without
       changing its users.

       The only OpenSSL exposed part is the name of the algorithms that may
       be used for generating the session key.

       The challenge packet has the following contents:

       @li A set of EC challenge keys (1-3 items, depending on the settings)
       @li An optional server certificate (compressed by zfile)

       The response packet has the following contents:

       @li A response EC key
       @li Client certificate (compressed by zfile)
       @li Client credentials (login and password : encrypted w/ session key)

       The challenge packet contains maximum 3 ECDH keys from which the client
       chooses which one to use. This value is chosen to provide some degree
       of flexiblity but still make the key generation fast. If too many keys
       would be generated and sent, it would seriously hurt performance.

       The client and server certificates are handled as uninterpreted binary
       data. This data is placed into the buffer in compressed form and will
       be decompressed automatically. The reason for this is to save space, so
       a challenge with a reasonable sized certificate can fit into an UDP
       packet. The maximum compressed certificate size is CSL_CERTIFICATE_MAX_LEN
       that is 4096 bytes at the current implementation.
    */
    class ecdh_pkt
    {
    public:
      /** @brief represents a binary buffer to pass data */
      typedef std::vector<unsigned char> bindata_t;

      /** @brief binary buffer without length info */
      typedef unsigned char * bin_ptr_t;

      /** @brief const binary buffer without length info */
      typedef const unsigned char * const_bin_ptr_t;

      /**
         @brief Sets the supported EC algorithms
         @param alg is an array of C strings terminated by a NULL pointer
         @return true if any algorithms added, false otherwise

         The list of supported algorithms may be obtained by the following
         command:

         @li openssl ecparam -list_curves

         The first column of the list contains the names to be fed to this
         function. Recommended values are:

         @li { "prime192v3", "secp224r1", "prime256v1", "secp384r1", "secp521r1", NULL }

         These values are recommended based on performance comparison of the
         supported algorithms on Intel Core2 CPU.

         Algorithm names should fit into 32 bytes. This is set by the
         CSL_ECDH_ALGNAME_MAX constant.

         Must be called both at the server and the client side. This should be
         the first step when using and ecdh_pkt object.
      */
      bool add_algorithms(
              const char ** alg);

      /**
         @brief returns the generated session key
         @return the session key or char[] = { 0 } if unavailable

         Session key will only be available if:

         @li the server received the response through the set_xdr_response() function @em or
         @li the client received the challenge through the set_xdr_challenge() and the add_key_to_response() function is called

         May be called both at the server and the client side, but different
         prerequistes must be met as described above.
      */
      unsigned char * get_session_key();

      /**
         @brief this function selects what keys to include in the challenge
         @param min_strength minimum strength in EC bits
         @param max_strength maximum strength in EC bits
         @return true if any key was added, false otherwise

         The add_algorithms() function may specify a lot more algorithms than
         the challenge packet allows (3 at the moment). The given min and max
         strength specifies which algorithms may be used. Max 3 of them will
         be chosen in increasing order of strength to be included in the challenge.

         Intended to be called at the server side.
      */
      bool add_keys_to_challenge(
              int min_strength,
              int max_strength );

      /** @brief Add the server certificate to the challenge
          @param data the binary data buffer
          @return true if successful, false otherwise

         Intended to be called at the server side.
      */
      bool add_cert_to_challenge(
              const bindata_t & data );

      /** @brief Add the server certificate to the challenge
          @param p is a pointer to the binary data
          @param len is the length of the data
          @return true if successful, false otherwise

         Intended to be called at the server side.
      */
      bool add_cert_to_challenge(
              const_bin_ptr_t p,
              size_t len );

      /** @brief Add the server certificate to the challenge
          @param filename is the name of the file that contains the certificate
          @return true if successful, false otherwise

         Intended to be called at the server side.
      */
      bool add_cert_file_to_challenge(
              const char * filename );

      /** @brief Gets the challenge data
          @param data_ptr is a pointer reference that will be set to the internal challenge buffer
          @param data_len will be set to the length of the challenge buffer
          @return true if successful, false otherwise

          The data_ptr will be set to an internal buffer that stores the
          challenge data. This data_ptr must not be freed.
          It is managed by ecdh_pkt.

         Intended to be called at the server side.
      */
      bool get_xdr_challenge(
              bin_ptr_t & data_ptr,
              size_t & data_len );

      /** @brief Sets the response data
          @param data_ptr is a pointer to the a buffer that contains the response
          @param data_len is the length of the buffer
          @return true if successful, false otherwise

          The given data will be parsed and relevant parts will be stored in the
          internal structures. This function is intended to be called at the server
          side when the response packet is received.
      */
      bool set_xdr_response(
              bin_ptr_t data_ptr,
              size_t data_len );

      /** @brief Sets the response data
          @param dta binary buffer that contains the response
          @return true if successful, false otherwise

          The given data will be parsed and relevant parts will be stored in the
          internal structures. This function is intended to be called at the server
          side when the response packet is received.
      */
      bool set_xdr_response(
              bindata_t & dta);

      /** @brief Gets the peer certificate
          @param len will be set to the length of the returned buffer
          @return pointer to the certificate buffer or NULL if fails

          May be called at both server and client side after:
          @li the set_xdr_challenge() is called on the client side @em or
          @li the set_xdr_response() is called on the server side
      */
      const unsigned char * get_peer_cert(size_t & len) const;

      /** @brief Gets the client login name
          @return C string or NULL if fails

          May be called at the server side after set_xdr_response() is called.
          The login name is transfered in an encrypted form and transparently
          decrypted when needed.
      */
      const char * get_peer_login() const;

      /** @brief Gets the client password
          @return C string or NULL if fails

          May be called at the server side after set_xdr_response() is called.
          The password is transfered in an encrypted form and transparently
          decrypted when needed.
      */
      const char * get_peer_password() const;

      /** @brief Sets the challenge data
          @param data_ptr is a pointer to the a buffer that contains the challenge
          @param data_len is the length of the buffer
          @return true if successful, false otherwise

          The given data will be parsed and relevant parts will be stored in the
          internal structures. To complete the process add_algorithms() and
          add_key_to_response() must be called too.

          Intended to be called at the client side.
      */
      bool set_xdr_challenge(
              bin_ptr_t data_ptr,
              size_t data_len);

      /** @brief Sets the challenge data
          @param dta is binary buffer that stores the challenge
          @return true if successful, false otherwise

          The given data will be parsed and relevant parts will be stored in the
          internal structures. To complete the process add_algorithms() and
          add_key_to_response() must be called too.

          Intended to be called at the client side.
      */
      bool set_xdr_challenge(
              bindata_t & dta);

      /** @brief Add the client certificate to the response
          @param data the binary data buffer
          @return true if successful, false otherwise

          Intended to be called at the client side.
      */
      bool add_cert_to_response(
              const bindata_t & data );

      /** @brief Add the client certificate to the response
          @param p is a pointer to the binary data
          @param len is the length of the data
          @return true if successful, false otherwise

          Intended to be called at the client side.
      */
      bool add_cert_to_response(
              const_bin_ptr_t p,
              size_t len );

      /** @brief Add the client certificate to the response
          @param filename is the name of the file that contains the certificate
          @return true if successful, false otherwise

          Intended to be called at the client side.
      */
      bool add_cert_file_to_response(
              const char * filename );

      /**
         @brief this function selects what key to include in the response
         @param min_strength minimum strength in EC bits
         @param max_strength max strength in EC bits
         @return true if key was added, false otherwise

         This function will only succeed if the add_algorithms() and the
         set_xdr_challenge() function were called before. Then this function
         selects which key will be chosen from the challenge packet based on
         the following criterias:

         @li the algorithm must be supported
         @li the chosen key strength must be bw/ min_strength and max_strength measured in EC bits

          Intended to be called at the client side.
      */
      bool add_key_to_response(
              int min_strength,
              int max_strength );

      /** @brief Add the client credentials to the response
          @param login login name
          @param pass is the password
          @return true if successful, false otherwise

          The login and password parameters are uninterpreted. They will
          be encrypted using the session key, so it must be available when
          this function is called. The client must call at least the following
          functions to successfuly call this function:

          @li add_algorithms()
          @li set_xdr_challenge()
          @li add_key_to_response()

          After these were called a session key should be generated if the challenge
          was valid.

          Intended to be called at the client side.
      */
      bool add_creds_to_response(
              const char * login,
              const char * pass);

      /** @brief Gets the response data
          @param data_ptr is a pointer reference that will be set to the internal response buffer
          @param data_len will be set to the length of the response buffer
          @return true if successful, false otherwise

          The data_ptr will be set to an internal buffer that stores the
          response data. This data_ptr must not be freed. It is managed by ecdh_pkt.

          Intended to be called at the client side.
      */
      bool get_xdr_response(
              bin_ptr_t & data_ptr,
              size_t & data_len );

      ecdh_pkt();
      ~ecdh_pkt();

      struct impl;
    private:
      /* private data */
      std::auto_ptr<impl> impl_;
      /* copying not allowed */
      ecdh_pkt(const ecdh_pkt & other);
      ecdh_pkt & operator=(const ecdh_pkt & other);
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_sec_ecdh_pkt_hh_included_ */
