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
                  ../../common/src
                  ../../sec/src
                )

INCLUDE_DIRECTORIES( ../../nthread/src
                     ../../common/src
                     ../../sec/src
                   )

IF(NOT WIN32)
  ADD_DEFINITIONS( -Wall
                   -g3
                 )

ELSE(NOT WIN32)
ENDIF(NOT WIN32)

ADD_LIBRARY( csl_comm STATIC
             udp_chann.cc         udp_chann.hh
             udp_cli.cc           udp_cli.hh
             udp_srv.cc           udp_srv.hh
             udp_srv_entry.cc     udp_srv_entry.hh
             udp_auth_entry.cc    udp_auth_entry.hh
             udp_data_entry.cc    udp_data_entry.hh
             udp_chann_entry.cc   udp_chann_entry.hh
             udp_srv_info.cc      udp_srv_info.hh
             udp_pkt.cc           udp_pkt.hh
             udp_recvr.cc         udp_recvr.hh
             udp_hello.cc         udp_hello.hh
             udp_auth.cc          udp_auth.hh
             cb.hh                csl_comm.hh
             exc.cc               exc.hh
             wsa.cc               wsa.hh
           )

# -- EOF --

