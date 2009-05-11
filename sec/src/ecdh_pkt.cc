/*
Copyright (c) 2008,2009, David Beck

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

#include <mpool.hh>
#include <openssl/ec.h>
#include <openssl/ecdh.h>
#include <openssl/bn.h>
#include <openssl/objects.h>
#include <map>
#include <stdlib.h>
#include <stdio.h>
#include "ecdh_pkt.hh"
#include "zfile.hh"
#include "csl_sec.h"
#include "crypt_buf.hh"
#include "xdr_challenge.hh"
#include "xdr_response.hh"
#include "bignum.hh"
#include "ecdh_key.hh"
#include "tbuf.hh"

/**
  @file ecdh_pkt.cc
  @brief implementation of ecdh_pkt
*/

using csl::common::zfile;
using csl::common::tbuf;
using csl::common::mpool;
using csl::common::pvlist;
using csl::common::delete_destructor;

namespace csl
{
  namespace sec
  {
    /** @brief Private implementation of ecdh_pkt */
    struct ecdh_pkt::impl
    {
      /* typedef helpers */
      struct priv_key
      {
        char *       algorithm_;
        unsigned int strength_;
        bignum       private_key_;

        priv_key() : algorithm_(0), strength_(0) { }
      };

      typedef pvlist< 16, priv_key, delete_destructor<priv_key> > pkeys_t;

      /* variables */
      mpool<>                   p_gen_;
      unsigned char *           sesskey_;
      unsigned char *           peer_cert_buf_;
      size_t                    peer_cert_sz_;
      std::string               login_;
      std::string               pass_;
      priv_key *                use_private_key_;
      pkeys_t                   private_keys_;
      pbuf                      peer_cert_;

      /* XDR structures */
      xdr_challenge             challenge_;
      xdr_response              response_;

      /* initialization */
      impl() :
        sesskey_(0),
        peer_cert_buf_(0),
        peer_cert_sz_(0),
        use_private_key_(0) { }

      /* private functions */
      void gen_free(void * ptr)
      {
        p_gen_.free(ptr);
      }

      char * gen_alloc(size_t len)
      {
        if( !len ) return 0;
        return (char *)p_gen_.allocate(len);
      }

      unsigned char * gen_ualloc(size_t len)
      {
        if( !len ) return 0;
        return (unsigned char *)gen_alloc(len);
      }

      char * str_dup( const char * str )
      {
        size_t sz = strlen( str );
        char * ret = (char *)gen_alloc( sz + 1 );
        memcpy(ret,str,sz);
        ret[sz] = 0;
        return ret;
      }

      unsigned int get_algorithm_strength( const std::string & alg )
      {
        return ecdh_key::algorithm_strength( alg );
      }

      unsigned char * get_session_key()
      {
        if( !sesskey_ )
        {
          sesskey_ = gen_ualloc(SHA1_HEX_DIGEST_STR_LENGTH);
          sesskey_[0] = 0;
        }
        return sesskey_;
      }

      /* interface */
      bool add_algorithms( const char ** algs )
      {
        const char ** a = algs;
        bool ret = false;

        if( !algs || !(*algs) ) return false;

        while( *a )
        {
          unsigned int strength = get_algorithm_strength( *a );

          if( strength > 100 )
          {
            /* must be a mistake, too weak key... */
            priv_key * pk = new priv_key();
            pk->strength_ = strength;
            pk->algorithm_ = str_dup( *a );
            private_keys_.push_back( pk );
            ret = true;
          }

          ++a;
        }
        return ret;
      }

      bool add_keys_to_challenge( int min_strength, int max_strength )
      {
        int  i   = 0;
        bool ret = false;

        if( min_strength == 0 || max_strength == 0 ) return false;
        if( min_strength > max_strength ) return false;

        pkeys_t::iterator it(private_keys_.begin());
        pkeys_t::iterator end(private_keys_.end());

        for( ;(it!=end && i<xdr_challenge::max_keys_offered);++it )
        {
          priv_key * pkit = (*it);
          if( pkit )
          {
            if( (pkit->strength_) >= (unsigned int)min_strength &&
                (pkit->strength_) <= (unsigned int)max_strength )
            {
              ecdh_key k;
              k.algname(pkit->algorithm_);
              if( k.gen_keypair(pkit->private_key_) )
              {
                challenge_.set(k,i);
                ++i;
                ret = true;
              }
            }
          }
        }
        return ret;
      }

      bool add_key_to_response( int min_strength, int max_strength )
      {
        if( min_strength == 0 || max_strength == 0 ) return false;
        if( min_strength > max_strength ) return false;

        for( size_t i=0;i<xdr_challenge::max_keys_offered;++i )
        {
          const ecdh_key & ekch(challenge_[i]);
          if( ekch.has_data() )
          {
            unsigned int strength = get_algorithm_strength( ekch.algname() );

            if( strength >= (unsigned int)min_strength && strength <= (unsigned int)max_strength )
            {
              pkeys_t::iterator it(private_keys_.begin());
              pkeys_t::iterator end(private_keys_.end());

              for( ;it!=end;++it )
              {
                priv_key * pkit = (*it);
                if( pkit && ekch.algname() == pkit->algorithm_ )
                {
                  use_private_key_ = (*it);
                  ecdh_key k;
                  k.algname(pkit->algorithm_);
                  if( k.gen_keypair( (*it)->private_key_) )
                  {
                    response_.key(k);
                    std::string sesskey;

                    if( ekch.gen_sha1hex_shared_key( (*it)->private_key_, sesskey ) )
                    {
                      if( sesskey.size() > 10 )
                      {
                        ::strncpy((char *)get_session_key(),sesskey.c_str(),SHA1_HEX_DIGEST_STR_LENGTH);
                        return true;
                      }
                    }
                  }
                }
              }
            }
          }
        }
        return false;
      }

      bool
      add_cert_to_challenge( const bindata_t & data )
      {
        if( data.size() > 0 )
        {
          zfile & zf(challenge_.cert());
          zf.custom_zlib_allocator(true);
          zf.init_custom_memory(true); /* to make valgrind happy */
          if( !zf.put_data( &(data[0]), data.size() ) ) return false;
          return true;
        }
        return false;
      }

      bool
      add_cert_to_response(
              const bindata_t & data )
      {
        if( data.size() > 0 )
        {
          zfile & zf(response_.cert());
          zf.custom_zlib_allocator(true);
          zf.init_custom_memory(true); /* to make valgrind happy */
          if( !zf.put_data( &(data[0]), data.size() ) ) return false;
          return true;
        }
        return false;
      }

      bool
      add_cert_to_challenge( const_bin_ptr_t p, size_t len )
      {
        if( len > 0 && p != NULL )
        {
          zfile & zf(challenge_.cert());
          zf.custom_zlib_allocator(true);
          zf.init_custom_memory(true); /* to make valgrind happy */
          if( !zf.put_data( p, len ) ) return false;
          return true;
        }
        return false;
      }

      bool
      add_cert_to_response( const_bin_ptr_t p, size_t len )
      {
        if( len > 0 && p != NULL )
        {
          zfile & zf(response_.cert());
          zf.custom_zlib_allocator(true);
          zf.init_custom_memory(true); /* to make valgrind happy */
          if( !zf.put_data( p, len ) ) return false;
          return true;
        }
        return false;
      }

      bool
      add_cert_file_to_challenge( const char * filename )
      {
        if( filename != NULL )
        {
          zfile & zf(challenge_.cert());
          zf.custom_zlib_allocator(true);
          zf.init_custom_memory(true); /* to make valgrind happy */
          if( !zf.read_file(filename) ) return false;
          return true;
        }
        return false;
      }

      bool
      add_cert_file_to_response(
              const char * filename )
      {
        if( filename != NULL )
        {
          zfile & zf(response_.cert());
          zf.custom_zlib_allocator(true);
          zf.init_custom_memory(true); /* to make valgrind happy */
          if( !zf.read_file(filename) ) return false;
          return true;
        }
        return false;
      }

      bool
      get_xdr_challenge(
              bin_ptr_t & data_ptr, size_t & data_len )
      {
        pbuf pb;
        xdrbuf xb(pb);
        if( !challenge_.to_xdr(xb) ) return false;
        if( pb.size() > xdr_challenge::max_challenge_len ) return false;

        size_t sz = pb.size();
        unsigned char * ret = gen_ualloc( sz );
        data_len = sz;
        data_ptr = ret;
        return pb.copy_to( data_ptr );
      }

      bool
      get_xdr_response(
              bin_ptr_t & data_ptr, size_t & data_len )
      {
        pbuf pb;
        xdrbuf xb(pb);
        if( !response_.to_xdr(xb) ) return false;
        if( pb.size() > xdr_response::max_response_len ) return false;

        size_t sz = pb.size();
        unsigned char * ret = gen_ualloc( sz );
        data_len = sz;
        data_ptr = ret;
        return pb.copy_to( data_ptr );
      }

      bool
      set_xdr_challenge(
              bin_ptr_t data_ptr,
              size_t data_len)
      {
        if( !data_ptr || !data_len ) return false;
        pbuf pb;
        if( !pb.append(data_ptr,data_len) ) return false;
        xdrbuf xb(pb);
        if( !challenge_.from_xdr(xb) ) return false;
        if( challenge_.cert().get_size() > 0 )
        {
          peer_cert_sz_  = challenge_.cert().get_size();
          peer_cert_buf_ = gen_ualloc( peer_cert_sz_ );
          challenge_.cert().get_data( peer_cert_buf_ );
        }
        return true;
      }

      bool
      set_xdr_response(
              bin_ptr_t data_ptr,
              size_t data_len )
      {
        if( !data_ptr || !data_len ) return false;
        pbuf pb;
        if( !pb.append(data_ptr,data_len) ) return false;
        xdrbuf xb(pb);
        if( !response_.from_xdr(xb) ) return false;
        if( response_.cert().get_size() > 0 )
        {
          peer_cert_sz_  = response_.cert().get_size();
          peer_cert_buf_ = gen_ualloc( peer_cert_sz_ );
          response_.cert().get_data( peer_cert_buf_ );
        }

        std::string sesskey;

        pkeys_t::iterator it(private_keys_.begin());
        pkeys_t::iterator end(private_keys_.end());

        for( ;it!=end;++it )
        {
          if( response_.key().algname() == (*it)->algorithm_ &&
              (*it)->private_key_.is_empty()==false )
          {
            use_private_key_ = (*it);
            if( response_.key().gen_sha1hex_shared_key( (*it)->private_key_, sesskey ) )
            {
              if( sesskey.size() > 10 )
              {
                ::strncpy((char *)get_session_key(),sesskey.c_str(),SHA1_HEX_DIGEST_STR_LENGTH);
                break;
              }
            }
          }
        }

        crypt_buf cb;

        size_t hl = cb.get_header_len();

        // no encrypted data has been set
        if( response_.encrypted_creds().size() < hl ) return true;

        size_t sz = response_.encrypted_creds().size() - hl;

        if( sz > xdr_response::max_creds_len ) return false;

        unsigned char * buf = gen_ualloc( sz+hl );
        if( !response_.encrypted_creds().copy_to(buf) ) return false;

        cb.init_crypt( (unsigned char *)buf, (const char *)get_session_key(), false );
        cb.add_data( (unsigned char *)buf+hl, sz, false );

        // XDR decrypt
        try
        {
          pbuf pc;
          if( !pc.append(buf+hl,sz) ) return false;
          xdrbuf xc(pc);

          tbuf<xdr_response::max_login_len+2>  log;
          tbuf<xdr_response::max_pass_len+2>   pas;

          unsigned int log_sz,pas_sz;

          xc.get_data(log,log_sz,xdr_response::max_login_len);
          xc.get_data(pas,pas_sz,xdr_response::max_pass_len);

          if( log_sz > xdr_response::max_login_len ||
              pas_sz > xdr_response::max_pass_len ) return false;

          log.append(0);
          pas.append(0);

          login_ = (const char *)log.data();
          pass_  = (const char *)pas.data();

        }
        catch( csl::common::exc e )
        {
          return false;
        }
        return true;
      }

      bool add_creds_to_response( const char * login, const char * pass)
      {
        unsigned char * seskey = get_session_key();

        if( !login || !seskey || !*seskey ) { return false; }

        crypt_buf  cb;
        size_t     hl      = cb.get_header_len();
        size_t     maxlen  = hl + xdr_response::max_login_len + xdr_response::max_pass_len;


        unsigned char * buf  = gen_ualloc( maxlen );
        unsigned char * obuf = (unsigned char *)buf;

        pbuf pb;

        try
        {
          xdrbuf xb(pb);
          xb << login;
          xb << pass;
        }
        catch( csl::common::exc e )
        {
          return false;
        }

        size_t sz = pb.size();

        if( pb.size() > maxlen ) return false;

        cb.init_crypt( buf, (const char *)seskey, true );
        buf    += hl;

        pb.copy_to( buf );
        cb.add_data( buf, sz, true );

        response_.encrypted_creds().append( obuf, sz+hl );

        return true;
      }

      const unsigned char *
      get_peer_cert(size_t & len) const
      {
        if( !peer_cert_sz_ || !peer_cert_buf_ ) return 0;
        len = peer_cert_sz_;
        return (const unsigned char *)peer_cert_buf_;
      }

      const char *
      get_peer_login() const
      {
        return login_.c_str();
      }

      const char *
      get_peer_password() const
      {
        return pass_.c_str();
      }
    };

    /* public interface */
    const unsigned char *
    ecdh_pkt::get_peer_cert(size_t & len) const
    {
      return impl_->get_peer_cert(len);
    }

    const char *
    ecdh_pkt::get_peer_login() const
    {
      return impl_->get_peer_login();
    }

    const char *
    ecdh_pkt::get_peer_password() const
    {
      return impl_->get_peer_password();
    }

    bool
    ecdh_pkt::add_algorithms( const char ** algs )
    {
      return impl_->add_algorithms( algs );
    }

    bool
    ecdh_pkt::add_keys_to_challenge(
              int min_strength,
              int max_strength )
    {
      return impl_->add_keys_to_challenge( min_strength, max_strength );
    }

    bool
    ecdh_pkt::add_cert_to_challenge(
              const bindata_t & data )
    {
      return impl_->add_cert_to_challenge(data);
    }

    bool ecdh_pkt::add_cert_to_challenge(
              const_bin_ptr_t p,
              size_t len )
    {
      return impl_->add_cert_to_challenge(p,len);
    }

    bool
    ecdh_pkt::add_cert_file_to_challenge(
              const char * filename )
    {
      return impl_->add_cert_file_to_challenge(filename);
    }

    bool
    ecdh_pkt::get_xdr_challenge(
              bin_ptr_t & data_ptr,
              size_t & data_len )
    {
      return impl_->get_xdr_challenge(data_ptr,data_len);
    }

    bool
    ecdh_pkt::set_xdr_challenge(
              bin_ptr_t data_ptr,
              size_t data_len)
    {
      return impl_->set_xdr_challenge(data_ptr, data_len);
    }

    bool
    ecdh_pkt::set_xdr_challenge(
              bindata_t & dta)
    {
      if( dta.size() )
        return impl_->set_xdr_challenge(&(dta[0]),dta.size());
      else
        return false;
    }

    bool
    ecdh_pkt::add_cert_to_response(
              const bindata_t & data )
    {
      return impl_->add_cert_to_response(data);
    }

    bool ecdh_pkt::add_cert_to_response(
              const_bin_ptr_t p,
              size_t len )
    {
      return impl_->add_cert_to_response(p,len);
    }

    bool
    ecdh_pkt::add_cert_file_to_response(
              const char * filename )
    {
      return impl_->add_cert_file_to_response(filename);
    }

    bool
    ecdh_pkt::add_key_to_response(
              int min_strength,
              int max_strength )
    {
      return impl_->add_key_to_response( min_strength, max_strength );
    }

    bool
    ecdh_pkt::add_creds_to_response(
              const char * login,
              const char * pass)
    {
      return impl_->add_creds_to_response( login, pass );
    }

    unsigned char *
    ecdh_pkt::get_session_key()
    {
      return impl_->get_session_key();
    }

    bool
    ecdh_pkt::get_xdr_response(
              bin_ptr_t & data_ptr,
              size_t & data_len )
    {
      return impl_->get_xdr_response( data_ptr, data_len );
    }

    bool
    ecdh_pkt::set_xdr_response(
              bin_ptr_t data_ptr,
              size_t data_len )
    {
      return impl_->set_xdr_response( data_ptr, data_len );
    }

    bool
    ecdh_pkt::set_xdr_response(
              bindata_t & dta)
    {
      if( dta.size() )
        return impl_->set_xdr_response(&(dta[0]),dta.size());
      else
        return false;
    }

    ecdh_pkt::ecdh_pkt()
      : impl_(new impl) {}

    ecdh_pkt::~ecdh_pkt() {}

    /* private functions, copying not allowed */
    ecdh_pkt::ecdh_pkt(const ecdh_pkt & other)
      : impl_((impl *)0) {throw std::string("should never be called"); }

    ecdh_pkt &
    ecdh_pkt::operator=(const ecdh_pkt & other)
    {
      throw std::string("should never be called");
      return *this;
    }
  };
};

/* EOF */
