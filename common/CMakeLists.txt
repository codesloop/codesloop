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

    ADD_DEFINITIONS( -Wall
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

  ENDIF(MSYS)

  LINK_DIRECTORIES(
      $ENV{ZLIB_DEV_ENV}/lib
      ../../../3rdparty/zlib/GnuWin32/lib )

  INCLUDE_DIRECTORIES(
      $ENV{ZLIB_DEV_ENV}/include
      ../../../3rdparty/zlib/GnuWin32/include )

ELSE(WIN32)

  SUBDIRS( libev )

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

ENDIF(WIN32)

ADD_DEFINITIONS( -DDEBUG)

INCLUDE_DIRECTORIES(
             ../..
             )


ADD_LIBRARY( csl_common STATIC
             cexc.cc       cexc.hh
             exc.cc        exc.hh
             zfile.cc      zfile.hh
             pbuf.cc       pbuf.hh
             xdrbuf.cc     xdrbuf.hh
             test_timer.c  test_timer.h
             common.h      pvlist.hh
             circbuf.hh
             mpool.hh      tbuf.hh
             logger.cc     logger.hh
             arch.cc       arch.hh
             serializable.hh
             obj.cc        obj.hh
             hlprs.cc      hlprs.hh
             hash.cc       hash.hh
             hash_helpers.hh
             auto_close.hh
             var.hh
             int64.cc      int64.hh
             dbl.cc        dbl.hh
             str.cc        str.hh
             ustr.cc       ustr.hh
             binry.cc      binry.hh )

# -- EOF --
