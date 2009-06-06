/*
Copyright (c) 2008,2009, David Beck, Tamas Foldi

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

#include "dbl.hh"
#include "int64.hh"
#include "str.hh"
#include "ustr.hh"
#include "binry.hh"
#include "xdrbuf.hh"

/**
  @file csl_common/src/int64.cc
  @brief implementation of 64 bit integer type
 */

namespace csl
{
  namespace common
  {
    /** @todo implement */
    int64::int64() : var() { }

    /* conversions to other types */
    bool int64::to_integer(int64 & v) const { return false; }
    bool int64::to_integer(long long & v) const { return false; }

    bool int64::to_double(dbl & v) const { return false; }
    bool int64::to_double(double & v) const { return false; }

    bool int64::to_string(str & v) const { return false; }
    bool int64::to_string(ustr & v) const { return false; }
    bool int64::to_string(std::string & v) const { return false; }

    bool int64::to_binary(binry & v) const { return false; }
    bool int64::to_binary(unsigned char * v, size_t & sz) const { return false; }
    bool int64::to_binary(void * v, size_t & sz) const { return false; }

    bool int64::to_xdr(xdrbuf & b) const { return false; }
    bool int64::to_var(var & v) const { return false; }

    /* conversions from other types */
    bool int64::from_integer(const int64 & v) { return false; }
    bool int64::from_integer(long long v) { return false; }

    bool int64::from_double(const dbl & v) { return false; }
    bool int64::from_double(double v) { return false; }

    bool int64::from_string(const str & v) { return false; }
    bool int64::from_string(const ustr & v) { return false; }
    bool int64::from_string(const std::string & v) { return false; }
    bool int64::from_string(const char * v) { return false; }
    bool int64::from_string(const wchar_t * v) { return false; }

    bool int64::from_binary(const binry & v) { return false; }
    bool int64::from_binary(const unsigned char * v,size_t sz) { return false; }
    bool int64::from_binary(const void * v,size_t sz) { return false; }

    bool int64::from_xdr(const xdrbuf & v) { return false; }
    bool int64::from_var(const var & v) { return false; }
  };
};

/* EOF */
