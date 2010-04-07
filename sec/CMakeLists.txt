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

# -- sec --

INCLUDE_DIRECTORIES( ../.. ${OPENSSL_INCLUDE_DIR} )

LINK_DIRECTORIES( ../common ${OPENSSL_LIBRARY_DIR} )

ADD_DEFINITIONS( -DUMAC_AE_USE_STDINT=1
                 -DUMAC_AE_SUPPORT=1
                 -DRUN_TESTS=0
                 -DSSE2=0
                 -DFORCE_C_ONLY=1 )

ADD_LIBRARY( csl_sec STATIC
             exc.cc              exc.hh
             bignum.cc           bignum.hh
             ecdh_key.cc         ecdh_key.hh
             sha1_conv.cc        csl_sec.h
             gen_rand.cc
             crypt_buf.cc        crypt_buf.hh
             crypt_pkt.cc        crypt_pkt.hh
             # -- 3rd party UMAC and AES
             umac.c              umac.h  umac_ae.h
             rijndael-alg-fst.c  rijndael-alg-fst.h )

FILE(GLOB includes "${CMAKE_CURRENT_SOURCE_DIR}/*.h*")
INSTALL( FILES ${includes} DESTINATION include/codesloop/sec )
INSTALL(TARGETS csl_sec
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
# -- EOF --

