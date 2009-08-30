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

#ifndef _csl_mq_lstnr_hh_included_
#define _csl_mq_lstnr_hh_included_

/**
   @file lstnr.hh
   @brief listener interface for message queues
 */

#include "obj.hh"
#include "msg.hh"

#ifdef __cplusplus
namespace csl
{
  namespace mq
  {
    /**
    @brief Message queue listener

    listen and process mq messages. 
    */
    class lstnr : public csl::common::obj
    {
      CSL_OBJ(csl::mq,lstnr);      

    public:
      /** standard constructor */
      lstnr() 
      {}

      /** standard constructor with session */
      lstnr(sess & s) 
      {
        set_session(s);
      }

      virtual ~lstnr() {};

      /** @brief receives a message from queue
      * 
      * receive an mq message - should be override 
      * by client implementation. Until the successfull
      * function return the message will be not committed
      * (= the function is transactional safe)
      */
      virtual void receive(msg & m) = 0;

      virtual inline void set_session(sess & s)
      {
        sess_ = &s;
      }

      /** subscribe to a message channel */
      virtual inline void subscribe(const char * q)
      {
        sess_->subscribe(*this,q);
      }

      /** unsubscribe to a message channel */
      virtual inline void unsubscribe(const char * q)
      {
        sess_->unsubscribe(*this,q);
      }

      /** listen for incoming messages (blocking) */
      virtual inline void listen()
      {
        sess_->listen();
      }

    protected: 
      sess * sess_;
    };
  }
}


#endif /* __cplusplus */
#endif /* _csl_mq_lstnr_hh_included_ */

