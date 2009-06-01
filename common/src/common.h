/*
Copyright (c) 2008,2009, David Beck

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
# ifndef ATOLL
#  define ATOLL _atoi64
# endif /*ATOLL*/
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
# ifndef ATOLL
#  define ATOLL atoll
# endif /*ATOLL*/
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
        throw exc(REASON,COMPONENT,"",__FILE__,__LINE__); \
        return RET; } \
      else { \
        fprintf(stderr,"Exception(%s:%d): [%s] [%s]\n", \
            __FILE__,__LINE__, \
            exc::component_string(COMPONENT), \
            exc::reason_string(REASON)); \
        return RET; } } while(false);
#endif /*THR*/

#ifndef THRC
#define THRC(REASON,COMPONENT,RET) \
    do { \
      if( this->use_exc() ) { \
        char errstr[256]; \
        strncpy(errstr,strerror(errno),255); errstr[255]=0; \
        throw exc(REASON,COMPONENT,errstr,__FILE__,__LINE__); \
        return RET; } \
      else { \
        fprintf(stderr,"Exception(%s:%d): [%s] [%s]\n", \
            __FILE__,__LINE__, \
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
        fprintf(stderr,"Exception(%s:%d): [%s] [%s]\n", \
            E.file_.c_str(),E.line_, \
            exc::component_string(E.component_), \
            exc::reason_string(E.reason_)); \
            return RET; } } while(false);
#endif /*THR*/

#ifndef THRNORET
#define THRNORET(REASON,COMPONENT) \
    do { \
      if( this->use_exc() ) { \
        throw exc(REASON,COMPONENT,"",__FILE__,__LINE__); } \
      else { \
        fprintf(stderr,"Exception(%s:%d): [%s] [%s]\n", \
            __FILE__,__LINE__, \
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
# define CSL_SYS_SOCKET_H_INCLUDED
#include <sys/socket.h>
# endif /*CSL_SYS_SOCKET_H_INCLUDED*/
# ifndef CSL_SYS_TIME_H_INCLUDED
# define CSL_SYS_TIME_H_INCLUDED
#include <sys/time.h>
#endif /*CSL_SYS_TIME_H_INCLUDED*/
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

#ifndef CSL_STRING_H_INCLUDED
# define CSL_STRING_H_INCLUDED
# include <string.h>
#endif /*CSL_STRING_H_INCLUDED*/

#ifndef CSL_ERRNO_H_INCLUDED
# define CSL_ERRNO_H_INCLUDED
# include <errno.h>
#endif /*CSL_ERRNO_H_INCLUDED*/

#ifdef WIN32
# define getpid()  0
#endif /* WIN32 */

#endif /* _csl_common_common_h_included_ */
