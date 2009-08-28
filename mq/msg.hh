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
THEORY OF LIABILITY, WHETHER IN CONTRACT, objICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _csl_mq_msg_hh_included_
#define _csl_mq_msg_hh_included_

/**
   @file session.hh
   @brief session interface for q'ing 
 */

#include "obj.hh"
#include "tbuf.hh"
#include "sess.hh"

#ifdef __cplusplus
namespace csl
{
  namespace mq
  {
    /**
    @brief Message queue message

    represents a transportable message 
    */
    class msg : public csl::common::obj
    {
      CSL_OBJ(csl::mq,msg);      

    public:
      /** standard constructor */
      msg() {}
      /** standard constructor with session */
      msg(sess & s) 
      {
        set_session(s);
      }

      virtual ~msg() {};

      virtual inline void set_tbuf(const csl::common::tbuf<512> * buf)
      {
        buf_ = buf;
      }

      virtual inline const csl::common::tbuf<512> * get_tbuf() const
      {
        return buf_;
      }

      virtual inline void set_session(sess & s)
      {
        sess_ = &s;
      }

      virtual inline void set_routing_key(const char * key)
      {
        routing_key_ = key;
      }

      virtual inline const char * get_routing_key() const
      {
        return routing_key_;
      }

      virtual void send(const char * xchg) = 0;
      virtual void send(const char * xchg,const char * routing_key) = 0;

    protected: 
      sess * sess_;
      const csl::common::tbuf<512> * buf_;
      const char * routing_key_;
    };
  }
}


#endif /* __cplusplus */
#endif /* _csl_mq_msg_hh_included_ */

