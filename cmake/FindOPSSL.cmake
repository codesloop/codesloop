# Copyright (c) 2008,2009,2010, CodeSloop Team
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

# Win32 dirs
# c:/OpenSSL/include
# $ENV{OPENSSL_DEV_ENV}/include
# ../../../3rdparty/OpenSSL/include
# c:/OpenSSL/lib/MinGW
# $ENV{OPENSSL_DEV_ENV}/lib/MinGW
# ../../../3rdparty/OpenSSL/lib/MinGW

FIND_PATH( OPENSSL_INCLUDE_DIR ssl.h
  HINTS $ENV{OPENSSL_DIR} $ENV{OPENSSL_DEV_ENV}
  PATH_SUFFIXES openssl
  PATHS /usr/local/ssl/include /usr/local/include /usr/include /sw/include /opt/local/include /opt/include )

SET(OPENSSL_NAMES ssl crypto)

IF(WIN32)
  IF(MSYS)
    SET(OPENSSL_NAMES eay32)
  ELSE(MSYS)
    SET(OPENSSL_NAMES ssleay32MT libeay32MT)
  ENDIF(MSYS)
ENDIF(WIN32)

FIND_LIBRARY(OPENSSL_LIBRARY NAMES ${OPENSSL_NAMES}
  HINTS $ENV{OPENSSL_DIR} $ENV{OPENSSL_DEV_ENV}
  PATH_SUFFIXES lib64 lib
  PATHS /usr/local/ssl /usr/local /usr /sw /opt/local /opt )

SET(OPENSSL_FOUND "NO")
SET(OPENSSL_LIBS "")

IF(OPENSSL_INCLUDE_DIR AND OPENSSL_LIBRARY)
   SET(OPENSSL_FOUND TRUE)
   SET(OPENSSL_LIBS ${OPENSSL_NAMES})
ELSE(OPENSSL_INCLUDE_DIR AND OPENSSL_LIBRARY)
   SET(OPENSSL_INCLUDE_DIR "")
   SET(OPENSSL_LIBRARY "")
ENDIF(OPENSSL_INCLUDE_DIR AND OPENSSL_LIBRARY)

IF(OPENSSL_FOUND)
  MESSAGE(STATUS "OpenSSL found: ${OPENSSL_LIBS} ${OPENSSL_INCLUDE_DIR} ${OPENSSL_LIBRARY_DIR}")
ELSE(OPENSSL_FOUND)
  MESSAGE(STATUS "OpenSSL NOT found")
ENDIF(OPENSSL_FOUND)

# -- EOF --

