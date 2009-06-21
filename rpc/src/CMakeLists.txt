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
  ADD_DEFINITIONS(-Wall)
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
ENDIF(WIN32)


INCLUDE_DIRECTORIES( ../../common/src) 
LINK_DIRECTORIES( ../../common/src )
LINK_LIBRARIES( csl_common )

ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/csrparser.cc
                  PRE_BUILD
                  COMMAND ragel -G2 ARGS -o csrparser.cc csrparser.rl
                  MAIN_DEPENDENCY csrparser.rl
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )

ADD_EXECUTABLE( csrgen  csrparser.cc csrparser.hh
                        iface.cc  iface.hh 
                        csrgen_main.cc 
                        csrgen.hh)
# -- EOF 
