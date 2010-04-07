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

# -- test / comm --

LINK_DIRECTORIES( ../../comm
                  ../../nthread
                  ../../sec
                  ../../common
                  ../../common/libev
                  ${OPENSSL_LIBRARY_DIR}
                  ${ZLIB_LIBRARY_DIR}
                  ${DLOPEN_LIBRARY_DIR} )

LINK_LIBRARIES( csl_comm
                csl_sec
                csl_nthread
                csl_common
                csl_common_libev
                ${OPENSSL_LIBS}
                ${ZLIB_LIBRARY}
                ${PTHREAD_LIBRARY}
                ${WSOCK32_LIBRARY}
                ${DLOPEN_LIBRARY} )

INCLUDE_DIRECTORIES( ../../.. ${OPENSSL_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR} ${DLOPEN_INCLUDE_DIR} )

ADD_DEFINITIONS( ${PTHREAD_FLAG} )

ADD_EXECUTABLE( t__bfd                 t__bfd.cc )
ADD_EXECUTABLE( t__mt_udp              t__mt_udp.cc )
ADD_EXECUTABLE( t__tcp_libev           t__tcp_libev.cc )
ADD_EXECUTABLE( t__tcp_client          t__tcp_client.cc )
ADD_EXECUTABLE( t__tcp_lstnr           t__tcp_lstnr.cc )
ADD_EXECUTABLE( t__udp_hello_client    t__udp_hello_client.cc )
ADD_EXECUTABLE( t__udp_hello_server    t__udp_hello_server.cc )
ADD_EXECUTABLE( t__udp_auth_client     t__udp_auth_client.cc )
ADD_EXECUTABLE( t__udp_auth_server     t__udp_auth_server.cc )
ADD_EXECUTABLE( t__udp_data_client     t__udp_data_client.cc )
ADD_EXECUTABLE( t__udp_data_server     t__udp_data_server.cc )

ADD_TEST(comm_bfd ${EXECUTABLE_OUTPUT_PATH}/t__bfd)
ADD_TEST(comm_mt_udp ${EXECUTABLE_OUTPUT_PATH}/t__mt_udp)
ADD_TEST(comm_tcp_client ${EXECUTABLE_OUTPUT_PATH}/t__tcp_client)
ADD_TEST(comm_tcp_libev ${EXECUTABLE_OUTPUT_PATH}/t__tcp_libev)
ADD_TEST(comm_tcp_lstnr ${EXECUTABLE_OUTPUT_PATH}/t__tcp_lstnr)
ADD_TEST(comm_udp_auth_client ${EXECUTABLE_OUTPUT_PATH}/t__udp_auth_client)
ADD_TEST(comm_udp_auth_server ${EXECUTABLE_OUTPUT_PATH}/t__udp_auth_server)
ADD_TEST(comm_udp_data_client ${EXECUTABLE_OUTPUT_PATH}/t__udp_data_client)
ADD_TEST(comm_udp_data_server ${EXECUTABLE_OUTPUT_PATH}/t__udp_data_server)
ADD_TEST(comm_udp_hello_client ${EXECUTABLE_OUTPUT_PATH}/t__udp_hello_client)
ADD_TEST(comm_udp_hello_server ${EXECUTABLE_OUTPUT_PATH}/t__udp_hello_server)

# -- EOF --
