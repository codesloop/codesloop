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

LINK_DIRECTORIES(
                  ../../common
                  ../../db
                )

INCLUDE_DIRECTORIES( ../../.. )

LINK_LIBRARIES(
                csl_common
              )

IF(NOT WIN32)

  SET( CMAKE_CXX_FLAGS "-Wconversion -Wcast-qual -Wpointer-arith -Wcast-align -Wold-style-cast" )

  ADD_DEFINITIONS(
                   -Wall
                   -g3
                   -Wwrite-strings
                   -Wcomment
                   -Wshadow
                   -Wredundant-decls
                 )

ELSE(NOT WIN32)

  IF(MSYS)

    SET( CMAKE_CXX_FLAGS "-Wconversion -Wcast-qual -Wpointer-arith -Wcast-align -Wold-style-cast" )

    ADD_DEFINITIONS(
                      -Wall
                      -g3
                      -Wwrite-strings
                      -Wcomment
                      -Wshadow
                      -Wredundant-decls
                   )

  ELSE(MSYS)


  ENDIF(MSYS)

ENDIF(NOT WIN32)

ADD_LIBRARY( csl_db_mysql STATIC
             driver.cc  driver.hh
           )

FILE(GLOB includes "${CMAKE_CURRENT_SOURCE_DIR}/*.h*")
INSTALL( FILES ${includes} DESTINATION include/codesloop/db/mysql )
INSTALL(TARGETS csl_db_mysql
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

# -- EOF --
