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

SET(IGEP_CXX_FLAGS "")
SET(IGEP_C_FLAGS "")

SET(IGEP_COMPILATION_FLAGS "-march=armv7-a -mfpu=neon -mtune=cortex-a8 -mfloat-abi=softfp")

IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
  IF(CMAKE_SYSTEM_PROCESSOR MATCHES "armv7l")
    EXEC_PROGRAM(cat ARGS "/proc/cpuinfo" OUTPUT_VARIABLE CPUINFO)
    STRING(REGEX REPLACE "^.*(IGEP v2).*$" "\\1" IS_IGEP ${CPUINFO})
    STRING(COMPARE EQUAL "IGEP v2" "${IS_IGEP}" IS_IGEP_TRUE)
    IF(IS_IGEP_TRUE)
      MESSAGE(STATUS "Configuring for the IGEP v2 platform")
      SET(IGEP_CXX_FLAGS ${IGEP_COMPILATION_FLAGS})
      SET(IGEP_C_FLAGS ${IGEP_COMPILATION_FLAGS})
    ENDIF(IS_IGEP_TRUE)
  ENDIF(CMAKE_SYSTEM_PROCESSOR MATCHES "armv7l")
ENDIF(CMAKE_SYSTEM_NAME MATCHES "Linux")

# -- EOF --
