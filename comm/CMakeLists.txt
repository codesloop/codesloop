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

# -- comm --

LINK_DIRECTORIES(
                  ../nthread
                  ../common/libev
                  ../common
                  ../sec
                )

INCLUDE_DIRECTORIES( ../.. )

LINK_LIBRARIES( csl_common_libev csl_common )

ADD_LIBRARY( csl_comm STATIC
             # -- TCP --
             tcp_lstnr.cc         tcp_lstnr.hh
             tcp_client.cc        tcp_client.hh
             # -- UDP --
             udp_recvr.cc         udp_recvr.hh
             udp_hello.cc         udp_hello.hh
             udp_auth.cc          udp_auth.hh
             udp_data.cc          udp_data.hh
             # -- Generic --
             bfd.cc               bfd.hh
             exc.cc               exc.hh
             initcomm.cc          initcomm.hh
             sai.hh               connid.hh
             handler.hh
             csl_comm.hh
           )

ADD_DEPENDENCIES( csl_comm ../common/libev/config.h )

FILE(GLOB includes "${CMAKE_CURRENT_SOURCE_DIR}/*.h*")
INSTALL( FILES ${includes} DESTINATION include/codesloop/comm )
INSTALL(TARGETS csl_comm
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

# -- EOF --
