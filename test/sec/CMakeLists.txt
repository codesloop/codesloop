# Copyright (c) 2008,2009,2010, CodeSLoop Team
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# SET(CMAKE_VERBOSE_MAKEFILE ON)

# -- test / sec --

LINK_LIBRARIES( csl_sec
                csl_common
                ${OPENSSL_LIBS}
                ${ZLIB_LIBRARY}
                ${WSOCK32_LIBRARY}
                ${DLOPEN_LIBRARY}
                ${STDCXX_LIBRARY} )

INCLUDE_DIRECTORIES( ../../.. ${OPENSSL_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR} )

LINK_DIRECTORIES( ../../sec
                  ../../common
                  ${OPENSSL_LIBRARY_DIR}
                  ${ZLIB_LIBRARY_DIR} )

ADD_EXECUTABLE(t__umac               t__umac.cc)
ADD_EXECUTABLE(t__crypt_buf          t__crypt_buf.cc)
ADD_EXECUTABLE(t__crypt_pkt          t__crypt_pkt.cc)
ADD_EXECUTABLE(t__derive_ec_params   t__derive_ec_params.c)
ADD_EXECUTABLE(t__sha1               t__sha1.c)
ADD_EXECUTABLE(t__bignum             t__bignum.cc)
ADD_EXECUTABLE(t__ecdh_key           t__ecdh_key.cc)

ADD_TEST(sec_bignum ${EXECUTABLE_OUTPUT_PATH}/t__bignum)
ADD_TEST(sec_crypt_buf ${EXECUTABLE_OUTPUT_PATH}/t__crypt_buf)
ADD_TEST(sec_crypt_pkt ${EXECUTABLE_OUTPUT_PATH}/t__crypt_pkt)
ADD_TEST(sec_derive_ec_params ${EXECUTABLE_OUTPUT_PATH}/t__derive_ec_params)
ADD_TEST(sec_ecdh_key ${EXECUTABLE_OUTPUT_PATH}/t__ecdh_key)
ADD_TEST(sec_sha1 ${EXECUTABLE_OUTPUT_PATH}/t__sha1)
ADD_TEST(sec_umac ${EXECUTABLE_OUTPUT_PATH}/t__umac)

# -- EOF --
