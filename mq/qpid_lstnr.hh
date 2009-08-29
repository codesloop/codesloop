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

#ifndef _csl_mq_qpid_lstnr_hh_included_
#define _csl_mq_qpid_lstnr_hh_included_

/**
   @file session.hh
   @brief session interface for q'ing
 */

#include "obj.hh"
#include "sess.hh"
#include "tbuf.hh"
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
    
    class qpid_lstnr : public lstnr, public qpid::client::MessageListener
    {
      CSL_OBJ(csl::mq,qpid_lstnr);
    
    public:
      virtual void received(qpid::client::Message& message)
      {
        qpid_msg m;
        csl::common::tbuf<512> buf( message.getData().c_str() );
        m.set_tbuf( &buf );

        receive( m );
      }

    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_mq_qpid_lstnr_hh_included_ */

