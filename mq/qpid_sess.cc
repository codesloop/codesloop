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

/**
   @file qpid_sess.cc
   @brief impl_ interface for qpid based queueing
 */

#include "codesloop/common/logger.hh"
#include "codesloop/mq/qpid_sess.hh"
#include "codesloop/mq/sess.hh"
#include "codesloop/mq/lstnr.hh"

#include <qpid/client/Connection.h>
#include <qpid/client/Session.h>
#include <qpid/client/Message.h>
#include <qpid/client/MessageListener.h>
#include <qpid/client/SubscriptionManager.h>


using namespace qpid::client;
using namespace qpid::framing;

namespace csl 
{
  namespace mq 
  {

    qpid_sess::qpid_sess() 
      : subs_(NULL)
    {
    }

    qpid_sess::qpid_sess(const char * url)  
      : subs_(NULL)      
    {
      this->connect(url);
    }

    qpid_sess::~qpid_sess()
    {      
      this->disconnect();
    }

    void qpid_sess::connect(const char * url)
    {
      conn_.open("127.0.0.1", 5672); // TODO: parse
      impl_ = conn_.newSession();
      subs_ = new SubscriptionManager(impl_);
    }

    void qpid_sess::disconnect()
    {
      impl_.close();
      conn_.close();      
      if ( subs_ ) {
        delete subs_;
        subs_ = NULL;
      }
    }

    void qpid_sess::add_q(const char * q)
    {
      impl_.queueDeclare(arg::queue=q);
    }

    void qpid_sess::del_q(const char * q)
    {
      impl_.queueDelete(arg::queue=q);
    }

    void qpid_sess::add_xchg(const char * xchg)
    {
      impl_.exchangeDeclare(arg::exchange=xchg, arg::type="topic");
    }

    void qpid_sess::del_xchg(const char * xchg)
    {
      impl_.exchangeDeclare(arg::exchange=xchg);
    }

    void qpid_sess::add_route(const char * xchg, const char * q, 
        const char * routing_key)
    {
      impl_.exchangeBind(arg::exchange=xchg, arg::queue=q, arg::bindingKey=routing_key);
    }

    void qpid_sess::del_route(const char * xchg, const char * q, 
        const char * routing_key) 
    {
      impl_.exchangeUnbind(arg::exchange=xchg, arg::queue=q, arg::bindingKey=routing_key);
    }

    void qpid_sess::subscribe(lstnr & lst, const char * q)
    {
      ENTER_FUNCTION();
      
      subs_->subscribe(dynamic_cast<MessageListener&>(lst),q);

      LEAVE_FUNCTION();
    }

    void qpid_sess::unsubscribe(lstnr & l, const char * q) 
    {
      ENTER_FUNCTION();
      
      subs_->cancel(q);

      LEAVE_FUNCTION();
    }

    void qpid_sess::listen() 
    {
      ENTER_FUNCTION();

      subs_->run();

      LEAVE_FUNCTION();
    }


  }
}

