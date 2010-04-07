# Copyright (c) 2008,2009,2010, CodeSLoop Team
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

# -- test / nthread --

LINK_LIBRARIES( csl_nthread
                csl_common
                ${PTHREAD_LIBRARY}
                ${ZLIB_LIBRARY}
                ${WSOCK32_LIBRARY}
                  )

INCLUDE_DIRECTORIES( ../../.. ${ZLIB_INCLUDE_DIR} )

LINK_DIRECTORIES( ../../nthread
                  ../../common
                  ${ZLIB_LIBRARY_DIR}
                  )

INCLUDE_DIRECTORIES( ../../.. )

IF(NOT WIN32)
  ADD_EXECUTABLE( t__pt_mutex t__pt_mutex.cc )
ENDIF(NOT WIN32)

ADD_EXECUTABLE( t__mutex         t__mutex.cc )
ADD_EXECUTABLE( t__thread        t__thread.cc )
ADD_EXECUTABLE( t__event         t__event.cc )
ADD_EXECUTABLE( t__pevent        t__pevent.cc )
ADD_EXECUTABLE( t__thrpool       t__thrpool.cc )

ADD_TEST(nthread_event ${EXECUTABLE_OUTPUT_PATH}/t__event)
ADD_TEST(nthread_mutex ${EXECUTABLE_OUTPUT_PATH}/t__mutex)
ADD_TEST(nthread_pevent ${EXECUTABLE_OUTPUT_PATH}/t__pevent)
ADD_TEST(nthread_pt_mutex ${EXECUTABLE_OUTPUT_PATH}/t__pt_mutex)
ADD_TEST(nthread_thread ${EXECUTABLE_OUTPUT_PATH}/t__thread)
ADD_TEST(nthread_thrpool ${EXECUTABLE_OUTPUT_PATH}/t__thrpool)

# -- EOF --
