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

# -- test / common --

LINK_DIRECTORIES( ../../common
                  ${ZLIB_LIBRARY_DIR} )

INCLUDE_DIRECTORIES( ../../.. ${ZLIB_INCLUDE_DIR} )

LINK_LIBRARIES( csl_common
                ${ZLIB_LIBRARY}
                ${WSOCK32_LIBRARY} )

SET_SOURCE_FILES_PROPERTIES( t__logger.cc PROPERTIES  COMPILE_FLAGS -DENABLE_LOGGER )

ADD_EXECUTABLE( t__zfile         t__zfile.cc )
ADD_EXECUTABLE( t__circbuf       t__circbuf.cc )
ADD_EXECUTABLE( t__pvlist        t__pvlist.cc )
ADD_EXECUTABLE( t__mpool         t__mpool.cc )
ADD_EXECUTABLE( t__pbuf          t__pbuf.cc )
ADD_EXECUTABLE( t__tbuf          t__tbuf.cc )
ADD_EXECUTABLE( t__xdrbuf        t__xdrbuf.cc )
ADD_EXECUTABLE( t__logger        t__logger.cc )
ADD_EXECUTABLE( t__str           t__str.cc )
ADD_EXECUTABLE( t__ustr          t__ustr.cc )
ADD_EXECUTABLE( t__int64         t__int64.cc )
ADD_EXECUTABLE( t__dbl           t__dbl.cc )
ADD_EXECUTABLE( t__binry         t__binry.cc )
ADD_EXECUTABLE( t__serial        t__serial.cc )
ADD_EXECUTABLE( t__obj           t__obj.cc )
ADD_EXECUTABLE( t__inpvec        t__inpvec.cc )
ADD_EXECUTABLE( t__queue         t__queue.cc )
ADD_EXECUTABLE( t__hash          t__hash.cc )
ADD_EXECUTABLE( t__hash_helpers  t__hash_helpers.cc )
ADD_EXECUTABLE( t__hash_exp      t__hash_exp.cc )
ADD_EXECUTABLE( t__auto_cloce    t__auto_close.cc )
ADD_EXECUTABLE( t__rdbuf         t__rdbuf.cc )
ADD_EXECUTABLE( t__read_res      t__read_res.cc )

ADD_TEST(common_auto_cloce ${EXECUTABLE_OUTPUT_PATH}/t__auto_cloce)
ADD_TEST(common_binry ${EXECUTABLE_OUTPUT_PATH}/t__binry)
ADD_TEST(common_circbuf ${EXECUTABLE_OUTPUT_PATH}/t__circbuf)
ADD_TEST(common_dbl ${EXECUTABLE_OUTPUT_PATH}/t__dbl)
ADD_TEST(common_hash ${EXECUTABLE_OUTPUT_PATH}/t__hash)
ADD_TEST(common_hash_exp ${EXECUTABLE_OUTPUT_PATH}/t__hash_exp)
ADD_TEST(common_hash_helpers ${EXECUTABLE_OUTPUT_PATH}/t__hash_helpers)
ADD_TEST(common_inpvec ${EXECUTABLE_OUTPUT_PATH}/t__inpvec)
ADD_TEST(common_int64 ${EXECUTABLE_OUTPUT_PATH}/t__int64)
ADD_TEST(common_logger ${EXECUTABLE_OUTPUT_PATH}/t__logger)
ADD_TEST(common_mpool ${EXECUTABLE_OUTPUT_PATH}/t__mpool)
ADD_TEST(common_obj ${EXECUTABLE_OUTPUT_PATH}/t__obj)
ADD_TEST(common_pbuf ${EXECUTABLE_OUTPUT_PATH}/t__pbuf)
ADD_TEST(common_pvlist ${EXECUTABLE_OUTPUT_PATH}/t__pvlist)
ADD_TEST(common_queue ${EXECUTABLE_OUTPUT_PATH}/t__queue)
ADD_TEST(common_rdbuf ${EXECUTABLE_OUTPUT_PATH}/t__rdbuf)
ADD_TEST(common_read_res ${EXECUTABLE_OUTPUT_PATH}/t__read_res)
ADD_TEST(common_serial ${EXECUTABLE_OUTPUT_PATH}/t__serial)
ADD_TEST(common_str ${EXECUTABLE_OUTPUT_PATH}/t__str)
ADD_TEST(common_tbuf ${EXECUTABLE_OUTPUT_PATH}/t__tbuf)
ADD_TEST(common_ustr ${EXECUTABLE_OUTPUT_PATH}/t__ustr)
ADD_TEST(common_xdrbuf ${EXECUTABLE_OUTPUT_PATH}/t__xdrbuf)
ADD_TEST(common_zfile ${EXECUTABLE_OUTPUT_PATH}/t__zfile)

#ADD_EXECUTABLE( t__hash_macros   t__hash_macros.cc )
#SET_TARGET_PROPERTIES( t__hash PROPERTIES LINK_FLAGS -pg )
#SET_TARGET_PROPERTIES( t__inpvec PROPERTIES LINK_FLAGS -pg )

# -- EOF --
