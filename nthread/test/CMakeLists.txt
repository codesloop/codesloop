# Copyright (c) 2008, David Beck
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

  LINK_LIBRARIES(
      csl_common 
      csl_nthread
      zlib )
  INCLUDE_DIRECTORIES(
      ../src
      ../../common/src 
      $ENV{ZLIB_DEV_ENV}/include
      ../../../3rdparty/zlib/GnuWin32/include )
  LINK_DIRECTORIES(
      ../src
      ../../common/src
      $ENV{ZLIB_DEV_ENV}/lib
      ../../../3rdparty/zlib/GnuWin32/lib )

ELSE(WIN32)

  ADD_DEFINITIONS( -Wall -O3 -pthread )
  INCLUDE_DIRECTORIES(
      ../src
      ../../common/src )
  LINK_LIBRARIES(
      csl_common
      csl_nthread
      pthread
      z )

  LINK_DIRECTORIES( ../src 
                    ../../common/src )

  ADD_EXECUTABLE( t__pt_mutex      t__pt_mutex.cc )

ENDIF(WIN32)


ADD_EXECUTABLE( t__mutex         t__mutex.cc )
ADD_EXECUTABLE( t__thread        t__thread.cc )
ADD_EXECUTABLE( t__event         t__event.cc )
ADD_EXECUTABLE( t__pevent        t__pevent.cc )

# -- EOF --
