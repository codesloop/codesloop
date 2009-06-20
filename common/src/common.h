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

#ifndef _csl_common_common_h_included_
#define _csl_common_common_h_included_

/**
   @file common.h
   @brief Common definitions used at many places

   @def CSL_CDECL This macro provides C/C++ compatibility
 */

/* for: size_t, NULL */
#include <stddef.h>

#ifndef CSL_CDECL
# ifndef __cplusplus
#  define CSL_CDECL
#  define CSL_OPAQ_TYPE(A) void
# else
#  define CSL_CDECL extern "C"
#  define CSL_OPAQ_TYPE(A) A
# endif
#endif /* CSL_CDECL */

#ifndef PARAM_NOT_NULL
# define PARAM_NOT_NULL(P) \
    if( (P) == NULL ) return -1
#endif

#ifndef PARAM_NOT_ZERO
# define PARAM_NOT_ZERO(P) \
    if( (P) == 0 ) return -1
#endif

#ifndef PARAM_NOT_NULL2
# define PARAM_NOT_NULL2(P,R) \
    if( (P) == NULL ) return (R)
#endif

#ifndef PARAM_NOT_ZERO2
# define PARAM_NOT_ZERO2(P,R) \
    if( (P) == 0 ) return (R)
#endif

#ifdef WIN32
# ifndef SNPRINTF
#  define SNPRINTF _snprintf
# endif /*SNPRINTF*/
# ifndef SWPRINTF
#  ifdef __MINGW32__
/* brain dead mingw does not support buffer size ... */
#   define SWPRINTF(BUF,SZ,FMT,...) swprintf(BUF,FMT,__VA_ARGS__)
#  else /*!__MINGW32__ && SWPRINTF && WIN32 */
#   define SWPRINTF swprintf
#  endif /*__MINGW32__ && SWPRINTF && WIN32 */
# endif /*SWPRINTF*/
# ifndef FPRINTF
#  define FPRINTF fwprintf
# endif /*FPRINTF*/
# ifndef PRINTF
#  define PRINTF wprintf
# endif /*PRINTF*/
# ifndef ATOLL
#  define ATOLL _atoi64
# endif /*ATOLL*/
# ifndef WCSTOLL
#  define WCSTOLL wcstoll
# endif /*WCSTOLL*/
# ifndef WCSTOLD
#  define WCSTOLD wcstold
# endif /*WCSTOLD*/
# ifndef WCSTOD
#  define WCSTOD wcstod
# endif /*WCSTOD*/
# ifndef UNLINK
#  define UNLINK _unlink
# endif /*UNLINK*/
# ifndef STRDUP
#  define STRDUP _strdup
# endif /*STRDUP*/
#else /* WIN32 */
# ifndef SNPRINTF
#  define SNPRINTF snprintf
# endif /*SNPRINTF*/
# ifndef SWPRINTF
#  define SWPRINTF swprintf
# endif /*SWPRINTF*/
# ifndef FPRINTF
#  define FPRINTF fwprintf
# endif /*FPRINTF*/
# ifndef PRINTF
#  define PRINTF wprintf
# endif /*PRINTF*/
# ifndef ATOLL
#  define ATOLL atoll
# endif /*ATOLL*/
# ifndef WCSTOLL
#  define WCSTOLL wcstoll
# endif /*WCSTOLL*/
# ifndef WCSTOLD
#  define WCSTOLD wcstold
# endif /*WCSTOLD*/
# ifndef WCSTOD
#  define WCSTOD wcstod
# endif /*WCSTOD*/
# ifndef UNLINK
#  define UNLINK unlink
# endif /*UNLINK*/
# ifndef STRDUP
#  define STRDUP strdup
# endif /*STRDUP*/
#endif

#ifdef __cplusplus
#ifndef THR
#define THR(REASON,COMPONENT,RET) \
    do { \
      if( this->use_exc() ) { \
        throw exc(REASON,COMPONENT,L"",L""__FILE__,__LINE__); \
        return RET; } \
      else { \
        FPRINTF(stderr,L"Exception(%ls:%d): [%ls] [%ls]\n", \
            L""__FILE__,__LINE__, \
            exc::component_string(COMPONENT), \
            exc::reason_string(REASON)); \
        return RET; } } while(false);
#endif /*THR*/

#ifndef THRR
#define THRR(REASON,COMPONENT,MSG,RET) \
    do { \
      if( this->use_exc() ) { \
        throw exc(REASON,COMPONENT,MSG,L""__FILE__,__LINE__); \
        return RET; } \
      else { \
        FPRINTF(stderr,L"Exception(%ls:%d): [%ls] [%ls] [%ls]\n", \
            L""__FILE__,__LINE__, \
            exc::component_string(COMPONENT), \
            exc::reason_string(REASON), \
            MSG ); \
        return RET; } } while(false);
#endif /*THRR*/

#ifndef THRC
#define THRC(REASON,COMPONENT,RET) \
    do { \
      if( this->use_exc() ) { \
        wchar_t errstr[256]; \
        mbstowcs( errstr,strerror(errno),255 ); \
        throw exc(REASON,COMPONENT,errstr,L""__FILE__,__LINE__); \
        return RET; } \
      else { \
        FPRINTF(stderr,L"Exception(%ls:%d): [%ls] [%ls]\n", \
            L""__FILE__,__LINE__, \
            exc::component_string(COMPONENT), \
            exc::reason_string(REASON)); \
        return RET; } } while(false);
#endif /*THRC*/

#ifndef THREX
#define THREX(E,RET) \
    do { \
      if( this->use_exc() ) { \
        throw E; \
        return RET; } \
      else { \
        FPRINTF(stderr,L"Exception(%ls:%d): [%ls] [%ls]\n", \
            E.file_.c_str(),E.line_, \
            exc::component_string(E.component_), \
            exc::reason_string(E.reason_)); \
            return RET; } } while(false);
#endif /*THR*/

#ifndef THRNORET
#define THRNORET(REASON,COMPONENT) \
    do { \
      if( this->use_exc() ) { \
        throw exc(REASON,COMPONENT,L"",L""__FILE__,__LINE__); } \
      else { \
        FPRINTF(stderr,L"Exception(%ls:%d): [%ls] [%ls]\n", \
            L""__FILE__,__LINE__, \
            exc::component_string(COMPONENT), \
            exc::reason_string(REASON)); } } while(false);
#endif /*THRNORET*/
#endif /*__cplusplus*/

#ifndef WIN32
# ifndef CSL_ARPA_INET_H_INCLUDED
#  define CSL_ARPA_INET_H_INCLUDED
#  include <arpa/inet.h>
# endif /*CSL_ARPA_INET_H_INCLUDED*/
# ifndef CSL_NETINET_IN_H_INCLUDED
#  define CSL_NETINET_IN_H_INCLUDED
#  include <netinet/in.h>
# endif /*CSL_NETINET_IN_H_INCLUDED*/
# ifndef CSL_UNISTD_H_INCLUDED
#  define CSL_UNISTD_H_INCLUDED
#  include <unistd.h>
# endif /*CSL_UNISTD_H_INCLUDED*/
# ifndef CSL_SYS_SOCKET_H_INCLUDED
#  define CSL_SYS_SOCKET_H_INCLUDED
#  include <sys/socket.h>
# endif /*CSL_SYS_SOCKET_H_INCLUDED*/
# ifndef CSL_SYS_TIME_H_INCLUDED
#  define CSL_SYS_TIME_H_INCLUDED
#  include <sys/time.h>
# endif /*CSL_SYS_TIME_H_INCLUDED*/
# ifndef SleepSeconds
#  define SleepSeconds(A) ::sleep(A)
# endif /*SleepSeconds*/
# ifndef SleepMiliseconds
#  define SleepMiliseconds(A) ::usleep(A*1000)
# endif /*SleepMiliseconds*/
# ifndef ShutdownCloseSocket
#  define ShutdownCloseSocket(S) { ::shutdown(S,2); ::close(S); }
# endif /*SleepMiliseconds*/
#else
# ifdef __MINGW32__
#  ifndef CSL_UNISTD_H_INCLUDED
#   define CSL_UNISTD_H_INCLUDED
#   include <unistd.h>
#  endif /*CSL_UNISTD_H_INCLUDED*/
#  ifndef CSL_SYS_TIME_H_INCLUDED
#   define CSL_SYS_TIME_H_INCLUDED
#   include <sys/time.h>
#  endif /*CSL_SYS_TIME_H_INCLUDED*/
#  ifndef CSL_TIME_H_INCLUDED
#   define CSL_TIME_H_INCLUDED
#   include <time.h>
#  endif /*CSL_TIME_H_INCLUDED*/
# endif /*__MINGW32__*/
# ifndef ShutdownCloseSocket
#  define ShutdownCloseSocket(S) { ::shutdown(S,2); ::closesocket(S); }
# endif /*SleepMiliseconds*/
# ifndef CSL_WINDOWS_H_INCLUDED
#  define CSL_WINDOWS_H_INCLUDED
#  include <windows.h>
# endif /*CSL_WINDOWS_H_INCLUDED*/
# ifndef SleepSeconds
#  define SleepSeconds(A) ::Sleep(A*1000)
# endif /*SleepSeconds*/
# ifndef SleepMiliseconds
#  define SleepMiliseconds(A) ::Sleep(A)
# endif /*SleepMiliseconds*/
# ifndef CSL_BASETSD_H_INCLUDED
#  define CSL_BASETSD_H_INCLUDED
#  include <BaseTsd.h>
# endif /*CSL_BASETSD_H_INCLUDED*/
# ifndef CSL_INT32_T_DEFINED
#  define CSL_INT32_T_DEFINED
   typedef INT32 int32_t;
# endif /*CSL_INT32_T_DEFINED*/
# ifndef CSL_UINT32_T_DEFINED
#  define CSL_UINT32_T_DEFINED
   typedef DWORD32 uint32_t;
# endif /*CSL_UINT32_T_DEFINED*/
# ifndef CSL_SOCKLEN_T_DEFINED
# define CSL_SOCKLEN_T_DEFINED
   typedef int socklen_t;
# endif /*CSL_SOCKLEN_T_DEFINED*/
#endif /* WIN32 */

#ifndef CSL_STDIO_H_INCLUDED
# define CSL_STDIO_H_INCLUDED
# include <stdio.h>
#endif /*CSL_STDIO_H_INCLUDED*/

#ifndef CSL_STDLIB_H_INCLUDED
# define CSL_STDLIB_H_INCLUDED
# include <stdlib.h>
#endif /*CSL_STDLIB_H_INCLUDED*/

#ifndef CSL_STRING_H_INCLUDED
# define CSL_STRING_H_INCLUDED
# include <string.h>
#endif /*CSL_STRING_H_INCLUDED*/

#ifndef CSL_ERRNO_H_INCLUDED
# define CSL_ERRNO_H_INCLUDED
# include <errno.h>
#endif /*CSL_ERRNO_H_INCLUDED*/

#ifndef CSL_WCHAR_H_INCLUDED
#define CSL_WCHAR_H_INCLUDED
#include <wchar.h>
#endif /*CSL_WCHAR_H_INCLUDED*/

#ifdef WIN32
# ifndef __MINGW32__
#  define getpid()  0
# endif
#endif /* WIN32 */

/* types */
#ifndef CSL_TYPE_UNKNOWN
#define CSL_TYPE_UNKNOWN 0
#endif /*CSL_TYPE_UNKNOWN*/

#ifndef CSL_TYPE_NULL
#define CSL_TYPE_NULL 0
#endif /*CSL_TYPE_NULL*/

#ifndef CSL_TYPE_INT64
#define CSL_TYPE_INT64 1
#endif /*CSL_TYPE_INT64*/

#ifndef CSL_TYPE_DOUBLE
#define CSL_TYPE_DOUBLE 2
#endif /*CSL_TYPE_DOUBLE*/

#ifndef CSL_TYPE_STR
#define CSL_TYPE_STR 3
#endif /*CSL_TYPE_STR*/

#ifndef CSL_TYPE_USTR
#define CSL_TYPE_USTR 4
#endif /*CSL_TYPE_USTR*/

#ifndef CSL_TYPE_BIN
#define CSL_TYPE_BIN 5
#endif /*CSL_TYPE_BIN*/

#endif /* _csl_common_common_h_included_ */
