# Copyright (c) 2008,2009,2010, CodeSloop Team
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

IF(MYSQL_FOUND)
  SET(MYSQL_DB_DRIVER_LINKDIR ../db/mysql)
  SET(MYSQL_DB_DRIVER_DIR mysql)
  SET(MYSQL_DB_DRIVER_LIB csl_db_mysql)
  ADD_DEFINITIONS( -DUSE_MYSQL )
ELSE(MYSQL_FOUND)
  SET(MYSQL_DB_DRIVER_LINKDIR)
  SET(MYSQL_DB_DRIVER_DIR)
  SET(MYSQL_DB_DRIVER_LIB)
  ADD_DEFINITIONS( -DSKIP_MYSQL )
ENDIF(MYSQL_FOUND)

LINK_DIRECTORIES(
                  ../nthread
                  ../common
                  ../db/dummy
                  ${MYSQL_DB_DRIVER_LINKDIR} 
                  ../db/slt3
                )

INCLUDE_DIRECTORIES( ../.. )

LINK_LIBRARIES(
                csl_db_dummy
                csl_db_slt3
		${MYSQL_DB_DRIVER_LIB}
                csl_common
              )

SUBDIRS( slt3
         ${MYSQL_DB_DRIVER_DIR}
         dummy 
       )

ADD_LIBRARY( csl_db STATIC
             exc.cc      exc.hh
             driver.cc   driver.hh
             conn.hh
             tran.hh
             query.hh
           )

FILE(GLOB includes "${CMAKE_CURRENT_SOURCE_DIR}/*.h*")
INSTALL( FILES ${includes} DESTINATION include/codesloop/db )
INSTALL(TARGETS csl_db
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

# -- EOF --
