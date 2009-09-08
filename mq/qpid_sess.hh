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

#ifndef _csl_mq_qpid_sess_hh_included_
#define _csl_mq_qpid_sess_hh_included_

/**
   @file session.hh
   @brief session interface for q'ing
 */

#include "codesloop/common/obj.hh"
#include "codesloop/mq/sess.hh"
#include <qpid/client/Connection.h>
#include <qpid/client/Session.h>
#include <qpid/client/Message.h>
#include <qpid/client/MessageListener.h>
#include <qpid/client/SubscriptionManager.h>


#ifdef __cplusplus
namespace csl
{
  namespace mq
  {
    
    class qpid_sess : public sess
    {
      friend class lstnr;

      CSL_OBJ(csl::mq,qpid_sess);

      /** standard constructor */
      qpid_sess();
      /** constructor with connection to broker */
      qpid_sess(const char * url);
      virtual ~qpid_sess();

      /** connects to the specified url to the broker server */
      virtual void connect(const char * url);
      /** disconnet from the broker server */
      virtual void disconnect(); 
    
      /** defines a queue in broker server */
      virtual void add_q(const char * q);
      /** defines a queue in broker server */
      virtual void del_q(const char * q);
    
      /** defines a queue in broker server */
      virtual void add_xchg(const char * xchg);
      /** defines a queue in broker server */
      virtual void del_xchg(const char * xchg);

      /** adds a route to brokers routing table 
       *
       * xchanges are automatically created when they 
       * do not exists when add_route invoked
       */
      virtual void add_route(const char * xchg,
                     const char * q,
                     const char * routing_key);

      /** removes a route from brokers routing table */
      virtual void del_route(const char * xchg,
                     const char * q,
                     const char * routing_key);

      /** returns qpid session for test purposes */
      qpid::client::Session get_native_sess() 
      {
        return impl_;
      }

    protected:
      virtual void subscribe(lstnr & lst, const char * q);
      virtual void unsubscribe(lstnr & lst, const char * q);
      virtual void listen();

    private:
      qpid::client::Session impl_; 
      qpid::client::Connection conn_;
      qpid::client::SubscriptionManager * subs_;
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_mq_qpid_sess_hh_included_ */

