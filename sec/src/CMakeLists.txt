# Copyright (c) 2008,2009, David Beck
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

IF(WIN32)

  INCLUDE_DIRECTORIES(
      ../../common/src 
      $ENV{OPENSSL_DEV_ENV}/include
      ../../../3rdparty/OpenSSL/include
      $ENV{ZLIB_DEV_ENV}/include
      ../../../3rdparty/zlib/GnuWin32/include )

  LINK_DIRECTORIES(
      ../../common/src
      $ENV{OPENSSL_DEV_ENV}/lib/VC/static
      $ENV{ZLIB_DEV_ENV}/lib )

ELSE(WIN32)

  ADD_DEFINITIONS(-Wall -g3)

  INCLUDE_DIRECTORIES(
	  ../src
	  ../../common/src
	  /usr/local/ssl/include
	  )

  LINK_DIRECTORIES(
	  ../../common/src
	  ../src
	  /usr/local/ssl/lib
	  )
ENDIF(WIN32)

ADD_LIBRARY( csl_sec STATIC
             bignum.cc         bignum.hh
             ecdh_key.cc       ecdh_key.hh
             xdr_challenge.cc  xdr_challenge.hh
             xdr_response.cc   xdr_response.hh
             sha1_conv.cc
             ecdh_pkt.cc   ecdh_pkt.hh
             crypt_buf.cc  crypt_buf.hh )

# -- EOF --

