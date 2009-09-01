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
                   # -- is this really needed ? --
                   -DHAVE_DLFCN_H=1
                   -DHAVE_EPOLL_CTL=1
                   -DHAVE_EVENTFD=1
                   -DHAVE_INOTIFY_INIT=1
                   -DHAVE_INTTYPES_H=1
                   -DHAVE_LIBM=1
                   -DHAVE_MEMORY_H=1
                   -DHAVE_POLL=1
                   -DHAVE_POLL_H=1
                   -DHAVE_SELECT=1
                   -DHAVE_SIGNALFD=1
                   -DHAVE_STDINT_H=1
                   -DHAVE_STDLIB_H=1
                   -DHAVE_STRINGS_H=1
                   -DHAVE_STRING_H=1
                   -DHAVE_SYS_EPOLL_H=1
                   -DHAVE_SYS_EVENTFD_H=1
                   -DHAVE_SYS_INOTIFY_H=1
                   -DHAVE_SYS_QUEUE_H=1
                   -DHAVE_SYS_SELECT_H=1
                   -DHAVE_SYS_SIGNALFD_H=1
                   -DHAVE_SYS_STAT_H=1
                   -DHAVE_SYS_TYPES_H=1
                   -DHAVE_UNISTD_H=1
                   # -- feature settings --
                   -DEV_USE_NANOSLEEP=1
                   -DEV_USE_EPOLL=1
                   -DEV_USE_INOTIFY=1
                   -DEV_USE_4HEAP=1
                   -DEV_HEAP_CACHE_AT=1
                 )

  #ADD_CUSTOM_COMMAND(
  #   OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/config.h
  #   COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/config.h-Linux-i686 ${CMAKE_CURRENT_BINARY_DIR}/config.h
  #   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/config.h-Linux-i686
  # )

ENDIF(WIN32)

ADD_LIBRARY( csl_common_libev STATIC evwrap.c evwrap.h )

# -- EOF --
