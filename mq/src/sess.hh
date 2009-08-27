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

#ifndef _csl_mq_sess_hh_included_
#define _csl_mq_sess_hh_included_

/**
   @file session.hh
   @brief session interface for q'ing 
 */

#include "obj.hh"

#ifdef __cplusplus
namespace csl
{
  namespace mq
  {
    class lstnr;

    /**
    @brief Message queue session

    interface for basic, broker based functinalities like
    queue and exchange management, routing
    */
    class sess : public csl::common::obj
    {
      friend class lstnr;

      CSL_OBJ(csl::mq,sess);      

    protected:
      /** standard constructor */
      sess() {};
      /** constructor with connection to broker */
      sess(const char * url)  {};

      virtual ~sess() {};

    public:
      /** connects to the specified url to the broker server */
      virtual void connect(const char * url)    = 0;
      /** disconnet from the broker server */
      virtual void disconnect()                 = 0; 
    
      /** defines a queue in broker server */
      virtual void add_q(const char * q)        = 0;
      /** defines a queue in broker server */
      virtual void del_q(const char * q)        = 0;
    
      /** defines a queue in broker server */
      virtual void add_xchg(const char * xchg)  = 0;
      /** defines a queue in broker server */
      virtual void del_xchg(const char * xchg)  = 0;

      /** adds a route to brokers routing table 
       *
       * xchanges are automatically created when they 
       * do not exists when add_route invoked
       */
      virtual void add_route(const char * xchg,
                     const char * q,
                     const char * routing_key) = 0;

      /** removes a route from brokers routing table */
      virtual void del_route(const char * xchg,
                     const char * q,
                     const char * routing_key) = 0;

    protected:
      /** subscribe to a message channel */
      virtual void subscribe(lstnr & l, const char * q) = 0;
      /** unsubscribe to a message channel */
      virtual void unsubscribe(lstnr & l, const char * q) = 0;
      /** listen for incoming messages (blocking) */
      virtual void listen() = 0;

    };
  }
}


#endif /* __cplusplus */
#endif /* _csl_mq_sess_hh_included_ */

