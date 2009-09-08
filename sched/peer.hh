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

#ifndef _csl_sched_peer_hh_included_
#define _csl_sched_peer_hh_included_

/**
   @file peer.hh
   @brief peer @todo peer
 */

#include "codesloop/sec/ecdh_key.hh"
#include "codesloop/sec/bignum.hh"
#include "codesloop/db/conn.hh"
#include "codesloop/db/reg.hh"
#include "codesloop/db/obj.hh"
#include "codesloop/db/var.hh"
#include "codesloop/common/str.hh"
#ifdef __cplusplus

#ifndef SCHED_PEER_DB_NAME
#define SCHED_PEER_DB_NAME "sched_peer"
#endif /*SCHED_PEER_DB_NAME*/

#ifndef SCHED_PEER_TABLE_NAME
#define SCHED_PEER_TABLE_NAME "peers"
#endif /*SCHED_PEER_TABLE_NAME*/

#ifndef SCHED_PEER_DB_DEFAULT_PATH
#define SCHED_PEER_DB_DEFAULT_PATH "peer.db"
#endif /*SCHED_PEER_DB_DEFAULT_PATH*/

namespace csl
{
  namespace sched
  {
    using sec::ecdh_key;
    using sec::bignum;
    using slt3::conn;
    using slt3::reg;
    using slt3::sql;

    /** @todo document me */
    class peer : public slt3::obj
    {
      public:
        inline peer() :
              /* database fields */
              id_(           "id",          *this, "PRIMARY KEY ASC AUTOINCREMENT"),
              common_name_(  "common_name", *this, "UNIQUE NOT NULL"),
              public_key_(   "public_key",  *this, "UNIQUE NOT NULL"),
              private_key_(  "private_key", *this) { }

        inline ~peer() { }

        /* ORM mapper's stuff */
        static reg::helper reg_;
        static sql::helper  sql_helper_;
        virtual conn & db() { return reg_.db(); }
        virtual sql::helper & sql_helper() const { return sql_helper_; }
        virtual void on_load();
        virtual void on_change();

        /* public key */
        const ecdh_key & public_key() const;
        void public_key(const ecdh_key & pubk);

        /* private key */
        const bignum & private_key() const;
        void private_key(const bignum & privk);

        /* copy */
        peer & operator=(const peer & x);

      private:
        /* tmp variables */
        ecdh_key          tmp_public_;
        bignum            tmp_private_;
        /*no copy construct*/
        peer(const peer & x);

      public:
        /* db fields */
        slt3::intvar      id_;
        slt3::strvar      common_name_;
        slt3::blobvar     public_key_;
        slt3::blobvar     private_key_;

        CSL_OBJ(csl::sched,peer_state);
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_sched_peer_hh_included_ */
