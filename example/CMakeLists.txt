# Copyright (c) 2008,2009, CodeSLoop Team
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

  IF(MSYS)

    SET(ZLIB z)

    ADD_DEFINITIONS(
                     -Wall
                     -Wwrite-strings
                     -Wcast-qual
                     -Wpointer-arith
                     -Wold-style-cast
                     -Wconversion
                     -Wcomment
                     -Wcast-align
                     -Wshadow
                     -Wredundant-decls
                     -g3
                   )

  ELSE(MSYS)

    SET(ZLIB zlib)

  ENDIF(MSYS)

  LINK_DIRECTORIES(
                  ../common
                  ../sec
                  ../nthread
                  ../db
                  "C:/Program Files/GnuWin32/lib/"
                  $ENV{ZLIB_DEV_ENV}/lib
                  ../../3rdparty/zlib/GnuWin32/lib
                  )

  INCLUDE_DIRECTORIES(
                  ../..
                  "C:/Program Files/GnuWin32/include/"
                  $ENV{ZLIB_DEV_ENV}/include
                  ../../3rdparty/zlib/GnuWin32/include
                  )

  LINK_LIBRARIES( 
                  csl_slt3
                  csl_nthread
                  csl_common
                  ${ZLIB}
                  wsock32
                  )
ELSE(WIN32)

  FIND_LIBRARY(DLOPEN_FOUND NAMES dl PATH /usr/lib /lib /usr/local/lib)
  SET(LIBDL)

  IF(DLOPEN_FOUND)
   SET(LIBDL dl)
   SET(HAVE_LIBDL TRUE)
  ENDIF(DLOPEN_FOUND)

  LINK_DIRECTORIES( 
                  ../common
                  ../sec
                  ../nthread
                  ../db
                  )

  INCLUDE_DIRECTORIES( 
                     ../..
                     )

  ADD_DEFINITIONS(
                   -Wall
                   -Wwrite-strings
                   -Wcast-qual
                   -Wpointer-arith
                   -Wold-style-cast
                   -Wconversion
                   -Wcomment
                   -Wcast-align
                   -Wshadow
                   -Wredundant-decls
                   -g3
                   -pthread
                 )

  LINK_LIBRARIES(
                  csl_slt3
                  csl_nthread
                  csl_common
                  pthread
                  ${LIBDL}
                  z
                )

ENDIF(WIN32)

ADD_EXECUTABLE(ex_zfile1 ex_zfile1.cc)
ADD_EXECUTABLE(ex_zfile2 ex_zfile2.cc)
ADD_EXECUTABLE(ex_sql1 ex_sql1.cc)
ADD_EXECUTABLE(ex_sql2 ex_sql2.cc)
ADD_EXECUTABLE(ex_thread1 ex_thread1.cc)
ADD_EXECUTABLE(ex_thrpool ex_thrpool.cc)

# -- EOF --

