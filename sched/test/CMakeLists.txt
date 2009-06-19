# Copyright (c) 2008,2009, David Beck, Tamas Foldi
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

IF(WIN32)

  IF(MSYS)

    LINK_LIBRARIES(
                    csl_sched
                    csl_slt3
                    csl_sec
                    csl_nthread
                    csl_common
                    eay32
                    z
                    wsock32
                  )

    INCLUDE_DIRECTORIES(
                         ../src
                         ../../common/src
                         ../../nthread/src
                         ../../sec/src
                         ../../slt3/src
                         c:/OpenSSL/include
                         $ENV{OPENSSL_DEV_ENV}/include
                         ../../../3rdparty/OpenSSL/include
                       )

    LINK_DIRECTORIES(
                      ../src
                      ../../common/src
                      ../../nthread/src
                      ../../sec/src
                      ../../slt3/src
                      c:/OpenSSL/lib/MinGW
                      $ENV{OPENSSL_DEV_ENV}/lib/MinGW
                      ../../../3rdparty/OpenSSL/lib/MinGW
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
                     -Winline
                     -Wshadow
                     -Wredundant-decls
                     -g3
                   )

  ELSE(MSYS)
  
    LINK_LIBRARIES(
                    csl_sched
                    csl_slt3
                    csl_sec
                    csl_nthread
                    csl_common
                    ssleay32MT
                    libeay32MT
                    zlib
                    wsock32
                  )

    INCLUDE_DIRECTORIES(
                         ../src
                         ../../common/src
                         ../../nthread/src
                         ../../sec/src
                         ../../slt3/src
                         $ENV{OPENSSL_DEV_ENV}/include
                         ../../../3rdparty/OpenSSL/include
                         $ENV{ZLIB_DEV_ENV}/include
                         "C:/Program Files/GnuWin32/include/"
                         ../../../3rdparty/zlib/GnuWin32/include
                       )

    LINK_DIRECTORIES(
                      ../src
                      ../../common/src
                      ../../nthread/src
                      ../../sec/src
                      ../../slt3/src
                      $ENV{OPENSSL_DEV_ENV}/lib/VC/static
                      ../../../3rdparty/OpenSSL/lib/VC/static
                      $ENV{ZLIB_DEV_ENV}/lib
                      "C:/Program Files/GnuWin32/lib/"
                      ../../../3rdparty/zlib/GnuWin32/lib
                    )

  ENDIF(MSYS)  

ELSE(WIN32)

 find_library(DLOPEN_FOUND NAMES dl PATH /usr/lib /lib /usr/local/lib)
  set(LIBDL)

  if (DLOPEN_FOUND)
   set(LIBDL dl)
   set(HAVE_LIBDL TRUE)
  endif (DLOPEN_FOUND)

  ADD_DEFINITIONS(
                   -Wall
                   -Wwrite-strings
                   -Wcast-qual
                   -Wpointer-arith
                   -Wold-style-cast
                   -Wconversion
                   -Wcomment
                   -Wcast-align
                   -Winline
                   -Wshadow
                   -Wredundant-decls
                   -g3
                 )

  INCLUDE_DIRECTORIES(
                       ../src
                       ../../common/src
                       ../../nthread/src
                       ../../sec/src
                       ../../slt3/src
                       /usr/local/ssl/include
                     )

  LINK_LIBRARIES(
                  csl_sched
                  csl_slt3
                  csl_sec
                  csl_nthread
                  csl_common
                  ssl
                  crypto
                  z
                  stdc++
                  pthread
                  ${LIBDL}
                )

  LINK_DIRECTORIES(
                    ../src
                    ../../common/src
                    ../../sec/src
                    ../../slt3/src
                    /usr/local/ssl/lib
                  )

ENDIF(WIN32)

ADD_EXECUTABLE( t__peer      t__peer.cc )
ADD_EXECUTABLE( t__schedule  t__schedule.cc )
ADD_EXECUTABLE( t__item      t__item.cc )
#ADD_EXECUTABLE( t__states   t__states.cc )
#ADD_EXECUTABLE( t__send_msg t__send_msg.cc )
#ADD_EXECUTABLE( t__recv_msg t__recv_msg.cc )
ADD_EXECUTABLE( t__iface    t__iface.cc )

# -- EOF --
