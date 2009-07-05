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

#include "csl_slt3.hh"
#include "peer.hh"
#include "exc.hh"
#include "xdrbuf.hh"
#include "pbuf.hh"
#include <memory>

/**
  @file peer.cc
  @brief implementation of sched::peer
 */

using namespace csl::common;
using namespace csl::sec;

namespace csl
{
  namespace sched
  {
    using namespace csl::slt3;

    /* ORM mapper */
    reg::helper peer::reg_(SCHED_PEER_DB_NAME,SCHED_PEER_DB_DEFAULT_PATH);
    sql::helper peer::sql_helper_(SCHED_PEER_TABLE_NAME);

    void peer::on_load()
    {
      {
        // convert pubkey
        if( public_key_.get().size() > 0 )
        {
          pbuf pb;
          pb.append(public_key_.get().data(),public_key_.get().size());
          xdrbuf xb(pb);
          if( !tmp_public_.from_xdr(xb) ) THRNORET(exc::rs_xdr);
        }
      }

      {
        // convert privkey
        if( private_key_.get().size() > 0 )
        {
          pbuf pb;
          pb.append(private_key_.get().data(),private_key_.get().size());
          xdrbuf xb(pb);
          if( !tmp_private_.from_xdr(xb) ) THRNORET(exc::rs_xdr);
        }
      }
    }

    void peer::on_change()
    {
      obj::on_change();
      on_load();
    }

    /* public key */
    const ecdh_key & peer::public_key() const
    {
      return tmp_public_;
    }

    void peer::public_key(const ecdh_key & pubk)
    {
      tmp_public_ = pubk;
      pbuf pb;
      xdrbuf xb(pb);
      if( tmp_public_.to_xdr(xb) == false ) THRNORET(exc::rs_xdr);
      common::binry::buf_t v;
      if( pb.t_copy_to(v) == false ) THRNORET(exc::rs_internal);
      public_key_ = v;
    }

    /* private key */
    const bignum & peer::private_key() const
    {
      return tmp_private_;
    }

    void peer::private_key(const bignum & privk)
    {
      tmp_private_ = privk;
      pbuf pb;
      xdrbuf xb(pb);
      if( !tmp_private_.to_xdr(xb) ) THRNORET(exc::rs_xdr);
      common::binry::buf_t v;
      if( pb.t_copy_to(v) == false ) THRNORET(exc::rs_internal);
      private_key_ = v;
    }

    /* copy */
    peer & peer::operator=(const peer & x)
    {
      id_           = x.id_;
      common_name_  = x.common_name_;
      public_key_   = x.public_key_;
      private_key_  = x.private_key_;
      use_exc(x.use_exc());
      return *this;
    }

    peer::peer(const peer & x)
      : id_(0,*this), common_name_(0,*this), public_key_(0,*this), private_key_(0,*this)
    {
      throw exc(exc::rs_private_fun,get_class_name());
    }
  }
}

/* EOF */
