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

#include "logger.hh"
#include "common.h"
#include "exc.hh"


#include <iostream>
#include <fstream>

/**
  @file csl_common/src/logger.cc
  @brief implementation of csl::common::logger
 */

using std::string; /* TODO move to common::str */

namespace csl
{
  namespace common
  {
    namespace
    {
      static wchar_t * wgetenv(const char * e)
      {
        static wchar_t res[1024]; // TODO this is not thread safe
        mbstowcs( res,getenv(e),1024 );
        return res;
      }
    }

    // initalize to default logfile
    tbuf<256> logger::logfile_ = CSL_LOGFILE;
#if DEBUG
    str logger::class_to_trace_= ( getenv(CSL_TRACE_SCOPE) == NULL ?
                                        L"all" : wgetenv(CSL_TRACE_SCOPE) );
    bool   logger::enable_trace_  = ( getenv(CSL_TRACE_ENABLE) == NULL ?
                                        false : true );
    bool   logger::enable_stderr_ = ( getenv(CSL_TRACE_STDERR) == NULL ?
                                        false : true ) ;
#endif

    // type name to string helper 
    static const wchar_t * LOGTYPE_NAMES [] = { 
      L"UNKNOWN", L"DEBUG", L"INFO", L"AUTH", L"WARNING", L"ERROR", L"CRITICAL"
    };

    // sets output filename
    void logger::set_log_file( const char * logfile )
    {
      logfile_ = logfile;
    }


    // logs one message to the file
    void logger::log( logger_types type, const str & st )
    {
      char   szDateBuf[128];
      time_t ostime;

#ifndef DEBUG
      if ( type == LOG_DEBUG )
        return;
#endif

      if ( (int)type >= (int)LOG_LAST || (int)type <= LOG_UNKNOWN )
        throw exc(exc::rs_invalid_param,exc::cm_logger,L"Unknown log type");

      try {
        // set date and time
        time( &ostime );
        strftime( szDateBuf, sizeof(szDateBuf), "%b %d %H:%M:%S", localtime( &ostime ) );

        // append line-by-line
        std::fstream fs_log ( (const char *)logfile_.data(), std::ios_base::out | std::ios_base::app );

        // print header + string like: [2006-12-24 23:59] DEBUG: hello world!
        fs_log << szDateBuf 
               << L" ("
               <<  getpid()
               << L") ["
               << LOGTYPE_NAMES[ (int) type ] 
               << L"] "
               << st.c_str()
               << std::endl;

        fs_log.close();
#ifdef DEBUG
        if ( enable_stderr_ )
          std::cerr  << szDateBuf
                     << L" ("
                     << getpid()
                     << L") ["
                     << LOGTYPE_NAMES[ (int) type ]
                     << L"] "
                     << st.c_str()
                     << std::endl;
#endif

      } catch ( std::exception ex ) {
        std::cerr << ex.what();
      }
    } /* public interface */
  };
};
/* EOF */
