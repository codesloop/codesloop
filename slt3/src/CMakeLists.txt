# Copyright (c) 2008,2009, David Beck
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

LINK_DIRECTORIES( ../../nthread/src
                  ../../common/src )

INCLUDE_DIRECTORIES( ../../nthread/src
                     ../../common/src )

IF(NOT WIN32)
  ADD_DEFINITIONS(
    -Wall
    -g3
# -O3 -Os
    -DTHREADSAFE=1
    -DSQLITE_DEFAULT_PAGE_SIZE=32768
    -DSQLITE_MAX_ATTACHED=30
    -DSQLITE_ENABLE_COLUMN_METADATA=1
    -DSQLITE_DEFAULT_CACHE_SIZE=10000
    )
ELSE(NOT WIN32)
  ADD_DEFINITIONS(
    -DTHREADSAFE=1
    -DSQLITE_DEFAULT_PAGE_SIZE=32768
    -DSQLITE_MAX_ATTACHED=30
    -DSQLITE_ENABLE_COLUMN_METADATA=1
    -DSQLITE_DEFAULT_CACHE_SIZE=10000
    # -DDEBUG
    # -DPRINT_SQL
    )
ENDIF(NOT WIN32)

ADD_LIBRARY( csl_slt3 STATIC
             sqlite3.c
             _shared_impl.cc _shared_impl.hh
             sql.cc     sql.hh
             var.cc     var.hh
             exc.cc     exc.hh
             reg.cc     reg.hh
             conn.cc    conn.hh
             tran.cc    tran.hh
             synqry.cc  synqry.hh
             param.cc   param.hh
             obj.cc     obj.hh
           )

# -- EOF --
