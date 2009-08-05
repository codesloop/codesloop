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
                    ../src
                    "C:/Program Files/GnuWin32/lib/"
                    $ENV{ZLIB_DEV_ENV}/lib
                    ../../../3rdparty/zlib/GnuWin32/lib
                  )

  INCLUDE_DIRECTORIES(
                       ../src
                       "C:/Program Files/GnuWin32/include/"
                       $ENV{ZLIB_DEV_ENV}/include
                       ../../../3rdparty/zlib/GnuWin32/include
                     )

  LINK_LIBRARIES(
                  csl_common
                  ${ZLIB}
                  wsock32
                )

ELSE(WIN32)

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

  LINK_DIRECTORIES( ../src )

  INCLUDE_DIRECTORIES( ../src )

  LINK_LIBRARIES(
                  csl_common
                  z
                )

ENDIF(WIN32)

ADD_EXECUTABLE( t__zfile         t__zfile.cc )
ADD_EXECUTABLE( t__circbuf       t__circbuf.cc )
ADD_EXECUTABLE( t__pvlist        t__pvlist.cc )
ADD_EXECUTABLE( t__mpool         t__mpool.cc )
ADD_EXECUTABLE( t__pbuf          t__pbuf.cc )
ADD_EXECUTABLE( t__tbuf          t__tbuf.cc )
ADD_EXECUTABLE( t__xdrbuf        t__xdrbuf.cc )
ADD_EXECUTABLE( t__logger        t__logger.cc )
ADD_EXECUTABLE( t__str           t__str.cc )
ADD_EXECUTABLE( t__ustr          t__ustr.cc )
ADD_EXECUTABLE( t__int64         t__int64.cc )
ADD_EXECUTABLE( t__dbl           t__dbl.cc )
ADD_EXECUTABLE( t__binry         t__binry.cc )
ADD_EXECUTABLE( t__serial        t__serial.cc )
ADD_EXECUTABLE( t__obj           t__obj.cc )
ADD_EXECUTABLE( t__inpvec        t__inpvec.cc )
ADD_EXECUTABLE( t__hash          t__hash.cc )
ADD_EXECUTABLE( t__hash_helpers  t__hash_helpers.cc )

#ADD_EXECUTABLE( t__hash_macros   t__hash_macros.cc )
#SET_TARGET_PROPERTIES( t__hash PROPERTIES LINK_FLAGS -pg )

# -- EOF --
