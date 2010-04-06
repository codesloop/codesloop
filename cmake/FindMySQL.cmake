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

FIND_PATH(MYSQL_INCLUDE_DIR mysql_version.h
  HINTS
  $ENV{MYSQL_DIR}
  PATH_SUFFIXES mysql
  PATHS
  /usr/local/include
  /usr/include
  /sw/include
  /opt/local/include
  /opt/include
)

FIND_LIBRARY(MYSQL_LIBRARY
  NAMES mysqlclient
  HINTS
  $ENV{MYSQL_DIR}
  PATH_SUFFIXES lib64 lib
  PATHS
  /usr/local
  /usr
  /sw
  /opt/local
  /opt
  /usr/local/lib/mysql/
)

SET(MYSQL_FOUND "NO")

IF(MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)
   SET(MYSQL_FOUND TRUE)
ENDIF(MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)

IF(MYSQL_FOUND)
  MESSAGE(STATUS "MySQL found")
ELSE(MYSQL_FOUND)
  MESSAGE(STATUS "MySQL NOT found")
ENDIF(MYSQL_FOUND)

# -- EOF --
