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

LINK_DIRECTORIES( ../nthread
                  ../common )

INCLUDE_DIRECTORIES( ../../.. )

IF(NOT WIN32)

  SET( CMAKE_CXX_FLAGS "-Wconversion -Wcast-qual -Wpointer-arith -Wcast-align -Wold-style-cast" )

  ADD_DEFINITIONS(
                    -Wall
                    -g3
                    # -O3 -Os
                    -DTHREADSAFE=1
                   -DSQLITE_DEFAULT_PAGE_SIZE=32768
                   -DSQLITE_MAX_ATTACHED=30
                   -DSQLITE_ENABLE_COLUMN_METADATA=1
                   -DSQLITE_DEFAULT_CACHE_SIZE=10000
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
                      -DTHREADSAFE=1
                      -DSQLITE_DEFAULT_PAGE_SIZE=32768
                      -DSQLITE_MAX_ATTACHED=30
                      -DSQLITE_ENABLE_COLUMN_METADATA=1
                      -DSQLITE_DEFAULT_CACHE_SIZE=10000
                      # -DDEBUG
                      # -DPRINT_SQL
                     -Wwrite-strings
                     -Wcomment
                     -Wshadow
                     -Wredundant-decls
                   )

  ELSE(MSYS)

    ADD_DEFINITIONS(
                     -DTHREADSAFE=1
                     -DSQLITE_DEFAULT_PAGE_SIZE=32768
                     -DSQLITE_MAX_ATTACHED=30
                     -DSQLITE_ENABLE_COLUMN_METADATA=1
                     -DSQLITE_DEFAULT_CACHE_SIZE=10000
                     # -DDEBUG
                     # -DPRINT_SQL
                   )

  ENDIF(MSYS)

ENDIF(NOT WIN32)

ADD_LIBRARY( csl_db_slt3 STATIC
             sqlite3.c
             _shared_impl.cc  _shared_impl.hh
             sql.cc           sql.hh
             var.cc           var.hh
             reg.cc           reg.hh
             conn.cc          conn.hh
             tran.cc          tran.hh
             query.cc         query.hh
             obj.cc           obj.hh
             driver.cc        driver.hh
           )

FILE(GLOB includes "${CMAKE_CURRENT_SOURCE_DIR}/*.h*")
INSTALL( FILES ${includes} DESTINATION include/codesloop/db/slt3 )
INSTALL(TARGETS csl_db_slt3
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

# -- EOF --
