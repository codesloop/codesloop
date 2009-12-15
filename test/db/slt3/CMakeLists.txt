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
                  ../../../db
                  ../../../db/slt3
                  ../../../nthread
                  ../../../common
                )

INCLUDE_DIRECTORIES( ../../../.. )

IF(WIN32)

  LINK_LIBRARIES(
                  csl_db_slt3
                  csl_db
                  csl_nthread
                  csl_common
                  wsock32
                )

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

ELSE(WIN32)

  find_library(DLOPEN_FOUND NAMES dl PATH /usr/lib /lib /usr/local/lib)
  set(LIBDL)

  if (DLOPEN_FOUND)
   set(LIBDL dl)
   set(HAVE_LIBDL TRUE)
  endif (DLOPEN_FOUND)

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

  LINK_LIBRARIES(
                  csl_db_slt3
                  csl_db
                  csl_nthread
                  csl_common
                  pthread
                  ${LIBDL}
                )

ENDIF(WIN32)

ADD_EXECUTABLE( t__sqlite       t__sqlite.cc )
ADD_EXECUTABLE( t__conn         t__conn.cc )
ADD_EXECUTABLE( t__tran         t__tran.cc )
ADD_EXECUTABLE( t__query        t__query.cc )
ADD_EXECUTABLE( t__reg          t__reg.cc )
ADD_EXECUTABLE( t__var          t__var.cc )
ADD_EXECUTABLE( t__sql_helper   t__sql_helper.cc )
ADD_EXECUTABLE( t__var_mem      t__var_mem.cc )
ADD_EXECUTABLE( t__query_mem    t__query_mem.cc )

ADD_TEST(db_slt3_conn ${EXECUTABLE_OUTPUT_PATH}/t__conn)
ADD_TEST(db_slt3_query ${EXECUTABLE_OUTPUT_PATH}/t__query)
ADD_TEST(db_slt3_query_mem ${EXECUTABLE_OUTPUT_PATH}/t__query_mem)
ADD_TEST(db_slt3_reg ${EXECUTABLE_OUTPUT_PATH}/t__reg)
ADD_TEST(db_slt3_sql_helper ${EXECUTABLE_OUTPUT_PATH}/t__sql_helper)
ADD_TEST(db_slt3_sqlite ${EXECUTABLE_OUTPUT_PATH}/t__sqlite)
ADD_TEST(db_slt3_tran ${EXECUTABLE_OUTPUT_PATH}/t__tran)
ADD_TEST(db_slt3_var ${EXECUTABLE_OUTPUT_PATH}/t__var)
ADD_TEST(db_slt3_var_mem ${EXECUTABLE_OUTPUT_PATH}/t__var_mem)

# -- EOF --
