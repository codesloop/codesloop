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
      $ENV{ZLIB_DEV_ENV}/lib
      ../../../3rdparty/zlib/GnuWin32/lib )

  INCLUDE_DIRECTORIES(
      $ENV{ZLIB_DEV_ENV}/include
      ../../../3rdparty/zlib/GnuWin32/include )

ELSE(WIN32)
  ADD_DEFINITIONS(-Wall -g3 )
ENDIF(WIN32)

ADD_DEFINITIONS( -DDEBUG) 

ADD_LIBRARY( csl_common STATIC
             exc.cc        exc.hh
             zfile.cc      zfile.hh
             pbuf.cc       pbuf.hh
             xdrbuf.cc     xdrbuf.hh
             test_timer.c  test_timer.h
             common.h      pvlist.hh
             circbuf.hh
             mpool.hh      tbuf.hh 
             logger.cc     logger.hh
             str.hh        str.cc
<<<<<<< HEAD:common/src/CMakeLists.txt
             obj.hh )
=======
             obj.cc        obj.hh)

>>>>>>> 2b5366b84cf8c5afeac93cc1910e52b792b8cc3f:common/src/CMakeLists.txt

# -- EOF --
