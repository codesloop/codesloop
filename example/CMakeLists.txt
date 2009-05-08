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
  LINK_DIRECTORIES(
                  ../common/src
                  ../sec/src
                  ../rx/src
                  ../nthread/src
                  ../slt3/src
                  "C:/Program Files/GnuWin32/lib/"
                  $ENV{ZLIB_DEV_ENV}/lib
                  ../../3rdparty/zlib/GnuWin32/lib
                  )

  INCLUDE_DIRECTORIES(
                  ../common/src
                  ../sec/src
                  ../nthread/src
                  ../slt3/src
                  "C:/Program Files/GnuWin32/include/"
                  $ENV{ZLIB_DEV_ENV}/include
                  ../../3rdparty/zlib/GnuWin32/include
                  )

  LINK_LIBRARIES( 
                  csl_slt3
                  csl_nthread
                  csl_common
                  zlib
                  )
ELSE(WIN32)

  LINK_DIRECTORIES( 
                  ../common/src
                  ../sec/src
                  ../nthread/src
                  ../slt3/src
                  )

  INCLUDE_DIRECTORIES( 
                     ../common/src
                     ../sec/src
                     ../nthread/src
                     ../slt3/src
                     )

  ADD_DEFINITIONS( -Wall -O3 -pthread )
  LINK_LIBRARIES( csl_slt3
                  csl_nthread
                  csl_common
                  pthread
                  dl
                  z )
ENDIF(WIN32)

ADD_EXECUTABLE(ex_zfile1 ex_zfile1.cc)
ADD_EXECUTABLE(ex_zfile2 ex_zfile2.cc)
ADD_EXECUTABLE(ex_sql1 ex_sql1.cc)
ADD_EXECUTABLE(ex_sql2 ex_sql2.cc)
ADD_EXECUTABLE(ex_thread1 ex_thread1.cc)

# -- EOF --

