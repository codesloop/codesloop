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

#ifndef _csl_comm_udp_recvr_hh_included_
#define _csl_comm_udp_recvr_hh_included_

#include "csl_nthread.hh"
#include "csl_common.hh"
#include "ecdh_key.hh"
#ifdef __cplusplus

namespace csl
{
  using nthread::event;
  using nthread::mutex;
  using nthread::thread;
  using nthread::thrpool;

  using sec::bignum;
  using sec::ecdh_key;

  namespace comm
  {
    namespace udp
    {
#ifndef CSL_COMM_SAI_DEFINED
#define CSL_COMM_SAI_DEFINED
      typedef struct sockaddr_in SAI;
#endif

      struct msg
      {
        enum { max_len_v = 65536 };

        enum {
          hello_p = 1,
          olleh_p,
          result_p,
          unicast_auth_p,
          unicast_htua_p,
          multicast_auth_p,
          multicast_htua_p,
          data_p,
          salt_p
        };

        unsigned char  data_[max_len_v];
        unsigned int   size_;
        SAI            sender_;

        inline unsigned int max_len() const
        {
          return max_len_v;
        }

        inline void copy_to(msg & other)
        {
          if( size_ )
          {
            memcpy( other.data_, data_, size_ );
            other.size_ = size_;
          }
          memcpy( &(other.sender_),&sender_,sizeof(sender_) );
        }
      };

      class recvr : public thread::callback, public csl::common::obj
      {
        public:
          class msgs : public common::circbuf<msg,30>
          {
            public:
              virtual void on_new_item() { ev_.notify(); }
              virtual ~msgs() { }

              mutex   mtx_;
              event   ev_;
          };

          class msg_handler : public thread::callback, public csl::common::obj
          {
            public:
              /* this must lock/unlock msgs_.mtx_ */
              virtual void operator()(void) = 0;

              inline msg_handler() : msgs_(0), debug_(false), socket_(-1) {}

              /* NOTE: all the setter functions are supposed to be called once during
                 initialization. for this reason they are not protected by the mutex. if
                 this assumption is ever be broken, then both setters and getters must be
                 protected because this class is called by multiple threads in the threadpool.
              */

              /* */
              inline virtual void set_msgs(msgs & m) { msgs_ = &m; }

              /* socket */
              inline int socket() const    { return socket_; }
              inline void socket(int sock) { socket_ = sock; }

              /* debug ? */
              inline bool debug() const      { return debug_;  }
              inline void debug(bool yesno)  { debug_ = yesno; }

              /* private key */
              inline const bignum & private_key() const { return private_key_; }
              inline void private_key(const bignum & v) { private_key_ = v; }

              /* public key */
              inline const ecdh_key & public_key() const { return public_key_; }
              inline void public_key(const ecdh_key & v) { public_key_ = v; }

            protected:
              mutex     mtx_;
              msgs *    msgs_;
              bool      debug_;
              int       socket_;
              bignum    private_key_;
              ecdh_key  public_key_;

              CSL_OBJ(csl::comm::udp,recvr::msg_handler);
          };

        private:
          thrpool       thread_pool_;
          msgs          msgs_;
          SAI           addr_;
          int           socket_;
          bool          stop_me_;
          bool          debug_;
          mutex         mtx_;

        public:
          inline int socket() { return socket_; }

          bool start( unsigned int min_threads,
                      unsigned int max_threads,
                      unsigned int timeout_ms,
                      unsigned int attempts,
                      msg_handler & cb,
                      int sock=-1 );

          bool stop();

          SAI addr();
          void addr(const SAI & a);

          bool stop_me();
          void stop_me(bool yesno);

          virtual void operator()(void);
          virtual ~recvr();
          recvr();

          /* debug ? */
          inline void debug(bool yesno) { debug_ = yesno; }
          inline bool debug() const     { return debug_;  }

          CSL_OBJ(csl::comm::udp,recvr);
      };

    } /* end of udp namespace */
  } /* end of comm namespace */
} /* end of csl namespace */

#endif /* __cplusplus */
#endif /* _csl_comm_udp_recvr_hh_included_ */
