# Copyright (c) 2008,2009, CodeSloop Team
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
                   -Wshadow
                   -Wredundant-decls
                   -g3
                 )
ENDIF(WIN32)


ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/hello_cli.cc
                  PRE_BUILD
                  COMMAND ../../rpc/csrgen simple_hello.iface
                  MAIN_DEPENDENCY simple_hello.iface
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )

INCLUDE_DIRECTORIES( ../../.. )
LINK_DIRECTORIES( ../../common
                 ../../rpc 
                )

LINK_LIBRARIES(
                csl_rpc
                csl_common
              )


ADD_EXECUTABLE( hello_cli
                 hello_cli_main.cc
                 hello_cli.cc hello_cli.hh
              )

ADD_EXECUTABLE( hello_srv
                 hello_srv_main.cc
                 hello_srv.cc hello_srv.hh
              )



# simple conversion tests
#ADD_TEST(rpc_csrgen_exp1 ../../rpc/csrgen exp1.txt )
#ADD_TEST(rpc_csrgen_exp2 ../../rpc/csrgen exp2.txt )

# -- EOF --
