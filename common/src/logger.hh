
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

#ifndef _csl_common_logger_hh_included_
#define _csl_common_logger_hh_included_

/**
   @file csl_common/src/logger.hh
   @brief common logger class
 */

#include "str.hh"
#include "tbuf.hh"
#ifdef __cplusplus

// default logfile
#define CSL_LOGFILE        "csl.log"


// env variables
#define CSL_TRACE_ENABLE   "CSL_TRACE_ENABLE"
#define CSL_TRACE_STDERR   "CSL_TRACE_STDERR"
#define CSL_TRACE_SCOPE    "CSL_TRACE_SCOPE"

#ifdef DEBUG
#define CSL_DEBUG(str)     csl::common::logger::debug( str, __class_name )
#else
#define CSL_DEBUG(str)
#endif

#if __GCC__ || __GNUC__
#define STORE_FUNC_NAME()  \
  csl::common::str __function_name = csl::common::tbuf<256>(__PRETTY_FUNCTION__);             \
  csl::common::str __class_name;                                                   \
  __function_name = __function_name.substr( 0, __function_name.find(L'('));    \
  __function_name = __function_name.substr( (__function_name.rfind(L' ') ==    \
          str::npos ) ?  0 : __function_name.rfind(L' ') + 1, -1 );    \
  if ( __function_name.find( L"csl::" ) != str::npos)             \
    __function_name = __function_name.substr(10,-1);                          \
  if ( __function_name.find( ':' ) !=  str::npos)                     \
    __class_name = __function_name.substr( 0, __function_name.find( L"::" ) )
#else 
#define STORE_FUNC_NAME()                   \
  str __function_name = L""__func__; \
  const wchar_t   __class_name[] = L"class"
#endif

/* in debug mode these are optimized out by preprocessor */
#ifdef DEBUG
 #define ENTER_FUNCTION()  \
   STORE_FUNC_NAME();    \
   if ( csl::common::logger::enable_trace_ )                     \
     csl::common::logger::debug(L">>> Entering function: " + __function_name,__class_name)
 #define LEAVE_FUNCTION()                                        \
   {                                                             \
   if ( csl::common::logger::enable_trace_ )                     \
     csl::common::logger::debug(L"<<< Leaving function: " + __function_name,__class_name); \
   return;                                                       \
   }
 #define RETURN_FUNCTION(ret)                                    \
   {                                                             \
   if ( csl::common::logger::enable_trace_ )                     \
     csl::common::logger::debug(L"<<< Leaving function: " + __function_name,__class_name); \
   return(ret);                                                  \
   }

 #define THROW_EXCEPTION(e)                                      \
   {                                                             \
   if ( csl::common::logger::enable_trace_ )                     \
     csl::common::logger::debug(L"<<< Leaving function: " + __function_name +  \
                   L"; Exception: " + e.to_string());             \
   throw(e);                                                     \
   }

#else /* !DEBUG */
 #define ENTER_FUNCTION()
 #define LEAVE_FUNCTION()       return
 #define RETURN_FUNCTION(ret)   return(ret)
 #define THROW_EXCEPTION(e)     throw(e)
#endif /* DEBUG */


namespace csl {
  namespace common {

    /**
    @brief log level identifiers

    log levels are defined from DEBUG to CRITICAL
     */
    typedef enum logger_types 
    {
      LOG_UNKNOWN     = 0,
      LOG_DEBUG       = 1,
      LOG_INFO        = 2,
      LOG_AUTH        = 3,
      LOG_WARNING     = 4,
      LOG_ERROR       = 5,
      LOG_CRITICAL    = 6,
      LOG_LAST     
    } logger_types;


    /**
    @brief logger class used for tracing and printf style debugging

    this class is used by common classes for logging purpose. DEBUG
    messages are only available in DEBUG builds. 
     */
    class logger 
    {

      public:
        /** @brief main logger function 
        @param type log level (debug, error, etc.)
        @param str  message to log */
        static void             log( logger_types type, const str & str );

        /** @brief override default log file name and location 
        @param logfile full path of demanded file name */
        static void             set_log_file( const char * logfile );

        /** @brief shortcut function for critical errors  
        @param str  message to log */
        static inline void      critical( const str & str ) 
        {
          log( LOG_CRITICAL, str );
        }
        /** @brief shortcut function for info messages 
        @param str  message to log */
        static inline void      info ( const str & str ) {
          log( LOG_INFO, str );
        }
        /** @brief shortcut function for warning messages
        @param str  message to log */
        static inline void      warning( const str & str ) {
          log( LOG_WARNING, str );
        }
        /** @brief shortcut function for authentication logs
        @param str  message to log */
        static inline void      auth( const str & str ) {
          log( LOG_AUTH, str );
        }
        /** @brief shortcut function for normal errors  
        @param str  message to log */
        static inline void      error( const str & str ) {
          log( LOG_ERROR, str );
        }

        /** @brief shortcut function for debug messages

        Debug messages are only available in DEBUG builds, otherwise
        compiler optimizes out the debug related log macros
        @param str  message to log */
#if defined __GNUC__ && !defined DEBUG
        static inline void      debug( const str & str,
                                       const str & invoker = L"" )
        {
#else
        static inline void      debug( const str & st,
                                       const str & invoker = L"" )
        {
#endif
#ifdef DEBUG
          if ( class_to_trace_ == L"all" ||
              class_to_trace_.find( invoker ) != str::npos )
          {
            log( LOG_DEBUG, st );
          }
#endif
        }

        private:
        static tbuf<256>     logfile_;
#ifdef DEBUG
        public:
        static bool          enable_trace_;
        static str           class_to_trace_;
        static bool          enable_stderr_;
#endif
    }; /* class */
  }; /* namespace common */
}; /* namespace csl */

#endif /* __cplusplus */
#endif /* _csl_common_logger_hh_included_ */ 
