/*
Copyright (c) 2008,2009, CodeSLoop Team

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "hash.hh"

/**
  @file common/src/hlprs.cc
  @brief hash table implementation
 */

namespace csl
{
  namespace common
  {
    const unsigned char * byte_bits()
    {
      static const unsigned char byte_bits_[] = {
        0,1,1,2,1,2,2,3,    1,2,2,3,2,3,3,4,    1,2,2,3,2,3,3,4,    2,3,3,4,3,4,4,5,
        1,2,2,3,2,3,3,4,    2,3,3,4,3,4,4,5,    2,3,3,4,3,4,4,5,    3,4,4,5,4,5,5,6,
        1,2,2,3,2,3,3,4,    2,3,3,4,3,4,4,5,    2,3,3,4,3,4,4,5,    3,4,4,5,4,5,5,6,
        2,3,3,4,3,4,4,5,    3,4,4,5,4,5,5,6,    3,4,4,5,4,5,5,6,    4,5,5,6,5,6,6,7,
        1,2,2,3,2,3,3,4,    2,3,3,4,3,4,4,5,    2,3,3,4,3,4,4,5,    3,4,4,5,4,5,5,6,
        2,3,3,4,3,4,4,5,    3,4,4,5,4,5,5,6,    3,4,4,5,4,5,5,6,    4,5,5,6,5,6,6,7,
        2,3,3,4,3,4,4,5,    3,4,4,5,4,5,5,6,    3,4,4,5,4,5,5,6,    4,5,5,6,5,6,6,7,
        3,4,4,5,4,5,5,6,    4,5,5,6,5,6,6,7,    4,5,5,6,5,6,6,7,    5,6,6,7,6,7,7,8
      };
      return byte_bits_;
    }

    const unsigned char * byte_last_free()
    {
      static const unsigned char byte_last_free_[] = {
        0,1,2,2,3,3,3,3,    4,4,4,4,4,4,4,4,    5,5,5,5,5,5,5,5,    5,5,5,5,5,5,5,5,
        6,6,6,6,6,6,6,6,    6,6,6,6,6,6,6,6,    6,6,6,6,6,6,6,6,    6,6,6,6,6,6,6,6,
        7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,
        8,8,8,8,8,8,8,8,    8,8,8,8,8,8,8,8,    8,8,8,8,8,8,8,8,    8,8,8,8,8,8,8,8,
        8,8,8,8,8,8,8,8,    8,8,8,8,8,8,8,8,    8,8,8,8,8,8,8,8,    8,8,8,8,8,8,8,8,
        8,8,8,8,8,8,8,8,    8,8,8,8,8,8,8,8,    8,8,8,8,8,8,8,8,    8,8,8,8,8,8,8,8,
        8,8,8,8,8,8,8,8,    8,8,8,8,8,8,8,8,    8,8,8,8,8,8,8,8,    8,8,8,8,8,8,8,8
      };
      return byte_last_free_;
    }

    const unsigned char * byte_last_used()
    {
      static const unsigned char byte_last_used_[] = {
        255,0,1,1,2,2,2,2,  3,3,3,3,3,3,3,3,    4,4,4,4,4,4,4,4,    4,4,4,4,4,4,4,4,
        5,5,5,5,5,5,5,5,    5,5,5,5,5,5,5,5,    5,5,5,5,5,5,5,5,    5,5,5,5,5,5,5,5,
        6,6,6,6,6,6,6,6,    6,6,6,6,6,6,6,6,    6,6,6,6,6,6,6,6,    6,6,6,6,6,6,6,6,
        6,6,6,6,6,6,6,6,    6,6,6,6,6,6,6,6,    6,6,6,6,6,6,6,6,    6,6,6,6,6,6,6,6,
        7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7,    7,7,7,7,7,7,7,7
      };
      return byte_last_used_;
    }
  };
};

/* EOF */
