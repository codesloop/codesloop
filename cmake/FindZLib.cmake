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
# "C:/Program Files/GnuWin32/lib/"
# $ENV{ZLIB_DEV_ENV}/lib
# ../../3rdparty/zlib/GnuWin32/lib
# "C:/Program Files/GnuWin32/include/"
# $ENV{ZLIB_DEV_ENV}/include
# ../../3rdparty/zlib/GnuWin32/include

FIND_PATH( ZLIB_INCLUDE_DIR zlib.h
  HINTS $ENV{ZLIB_DIR} $ENV{ZLIB_DEV_ENV}
  PATHS /usr/local/include /usr/include /sw/include /opt/local/include /opt/include )

SET(ZLIB_NAMES z zlib zdll)

FIND_LIBRARY( ZLIB_LIBRARY NAMES ${ZLIB_NAMES}
  HINTS $ENV{ZLIB_DIR} $ENV{ZLIB_DEV_ENV}
  PATH_SUFFIXES lib64 lib
  PATHS /usr/local /usr /sw /opt/local /opt )

SET(ZLIB_FOUND "NO")

IF(ZLIB_INCLUDE_DIR AND ZLIB_LIBRARY)
   SET(ZLIB_FOUND TRUE)
ELSE(ZLIB_INCLUDE_DIR AND ZLIB_LIBRARY)
   SET(ZLIB_INCLUDE_DIR "")
   SET(ZLIB_LIBRARY "")
ENDIF(ZLIB_INCLUDE_DIR AND ZLIB_LIBRARY)

IF(ZLIB_FOUND)
  MESSAGE(STATUS "zlib found: ${ZLIB_LIBRARY} ${ZLIB_INCLUDE_DIR} ${ZLIB_LIBRARY_DIR}")
ELSE(ZLIB_FOUND)
  MESSAGE(STATUS "zlib NOT found")
ENDIF(ZLIB_FOUND)

# -- EOF --

