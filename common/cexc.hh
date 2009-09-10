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

#ifndef _csl_common_cexc_hh_included_
#define _csl_common_cexc_hh_included_

/**
   @file common/src/cexc.hh
   @brief common base exception class for codesloop classes
 */

#include "codesloop/common/str.hh"
#include "codesloop/common/logger.hh"
#ifdef __cplusplus

namespace csl
{
  namespace common
  {
    /**
    @brief common base exception class for codesloop classes

    this class is used as base class when a module implements its
    own exception
     */
    class cexc
    {
      public:
        /** @brief converts reason code to string */
        static const wchar_t * reason_string(int rc);

        /** @brief converts exception to string
        @param res string to store result */
        virtual void to_string(str & res);

        /** @brief converts exception to string */
        virtual str to_string();

        /** @brief constructor
        *   @param component that caused the exception
        */
        cexc(const wchar_t * component)
        : reason_(0), component_(component), line_(0) {}

        /** @brief constructor
        *   @param reason is to tell why
        *   @param component that cause the exception
        */
        cexc(int reason, const wchar_t * component)
        : reason_(reason), component_(component), line_(0) {}

        /** @brief constructor
        *   @param reason is to tell why
        *   @param component that cause the exception
        *   @param txt provides some explanation
        */
        cexc(int reason, const wchar_t * component, const wchar_t * txt)
        : reason_(reason), component_(component), text_(txt), line_(0) {}

        /** @brief constructor
        *   @param reason is to tell why
        *   @param component that cause the exception
        *   @param txt provides some explanation
        *   @param file tells which source file caused the error
        *   @param lin tells which line cause the error
        */
        cexc(int reason, const wchar_t * component, const wchar_t * txt, const wchar_t * file, unsigned int line)
        : reason_(reason), component_(component), text_(txt), file_(file), line_(line) {}

        virtual ~cexc();

        int reason_;        ///<reason code: one of rs_*
        str component_;     ///<component name
        str text_;          ///<error explanation
        str file_;          ///<error source file
        unsigned int line_; ///<error posintion in source file

      protected:
        cexc();
    };
  }
}

#ifdef __cplusplus

#ifndef THR
#define THR(REASON,RET) \
    do { \
      CSL_DEBUGF(L"Exception(%ls:%d): [%ls] [%ls]\n", \
                 L""__FILE__,__LINE__, \
                 get_class_name(), \
                 exc::reason_string(REASON)); \
      if( this->use_exc() ) \
            throw exc(REASON,get_class_name(),L"",L""__FILE__,__LINE__); \
      RETURN_FUNCTION( RET ); \
    } while(false);
#endif /*THR*/

#ifndef THRNORET
#define THRNORET(REASON) \
    do { \
      CSL_DEBUGF(L"Exception(%ls:%d): [%ls] [%ls]\n", \
                 L""__FILE__,__LINE__, \
                 get_class_name(), \
                 exc::reason_string(REASON)); \
      if( this->use_exc() )  \
          throw exc(REASON,get_class_name(),L"",L""__FILE__,__LINE__); \
    } while(false);
#endif /*THRNORET*/

#ifndef THRR
#define THRR(REASON,MSG,RET) \
    do { \
      CSL_DEBUGF(L"Exception(%ls:%d): [%ls] [%ls] [%ls]\n", \
                 L""__FILE__,__LINE__, \
                 get_class_name(), \
                 exc::reason_string(REASON), \
                 MSG ); \
      if( this->use_exc() ) \
            throw exc(REASON,get_class_name(),MSG,L""__FILE__,__LINE__); \
      RETURN_FUNCTION( RET ); \
    } while(false);
#endif /*THRR*/

#ifndef THRRNORET
#define THRRNORET(REASON,MSG) \
    do { \
      CSL_DEBUGF(L"Exception(%ls:%d): [%ls] [%ls] [%ls]\n", \
                 L""__FILE__,__LINE__, \
                 get_class_name(), \
                 exc::reason_string(REASON), \
                 MSG ); \
      if( this->use_exc() ) \
            throw exc(REASON,get_class_name(),MSG,L""__FILE__,__LINE__); \
    } while(false);
#endif /*THRRNORET*/

#ifndef THRC
#define THRC(REASON,RET) \
    do { \
      CSL_DEBUGF(L"Exception(%ls:%d): [%ls] [%ls]\n", \
                 L""__FILE__,__LINE__, \
                 get_class_name(), \
                 exc::reason_string(REASON)); \
      if( this->use_exc() ) \
      { \
        wchar_t errstr[256]; \
        mbstowcs( errstr,strerror(errno),255 ); \
        throw exc(REASON,get_class_name(),errstr,L""__FILE__,__LINE__); \
      } \
      RETURN_FUNCTION( RET ); \
    } while(false);
#endif /*THRC*/

#ifndef THREX
#define THREX(E,RET) \
    do { \
      CSL_DEBUGF(L"Exception(%ls:%d): [%ls] [%ls]\n", \
                 E.file_.c_str(),E.line_, \
                 get_class_name(), \
                 exc::reason_string(E.reason_)); \
      if( this->use_exc() ) { throw ( E ); } \
      RETURN_FUNCTION( RET ); \
    } while(false);
#endif /*THR*/

#endif /*__cplusplus*/

#endif /* __cplusplus */
#endif /* _csl_common_cexc_hh_included_ */
