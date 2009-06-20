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

#include "ecdh_key.hh"
#include "bignum.hh"
#include "csl_sec.h"
#include <openssl/ec.h>
#include <openssl/ecdh.h>
#include <openssl/bn.h>
#include <openssl/objects.h>
#include "ustr.hh"
#include "common.h"

/**
  @file ecdh_key.cc
  @brief ECDH key helper w/ XDR features
 */

using namespace csl::common;

namespace csl
{
  namespace
  {
    extern "C" void * save_to_pbuf_as_is_( const void  * in,
                                           size_t        inlen,
                                           void        * out,
                                           size_t      * outlen )
    {
      if( !in || !inlen || !out || !outlen ) return 0;

      pbuf * pbp = (pbuf *)out;
      pbp->append((unsigned char *)in,inlen);
      *outlen = inlen;

      return out;
    }

    struct oEXC
    {
      const char * reason_;
      unsigned int line_;
      oEXC(const char * reason,unsigned int l=0) : reason_(reason), line_(l) {}
    };

#define THROWEXC(M) throw oEXC(M,__LINE__)

    struct oBIGNUM
    {
      BIGNUM * bn_;

      oBIGNUM() : bn_(BN_new())
      {
        if( !bn_ ) THROWEXC("oBIGNUM cannot allocate memory!");
      }

      ~oBIGNUM() { reset(); }

      void reset()
      {
        if(bn_)
        {
          BN_free(bn_);
          bn_=0;
        }
      }

      void to_bignum(csl::sec::bignum & bn)
      {
        oBIGNUM::to_bignum(bn_,bn);
      }

      static void to_bignum(const BIGNUM * from, csl::sec::bignum & to)
      {
        if( !from ) { THROWEXC("oBIGNUM 'from' is NULL!"); }
        else
        {
          size_t sz = BN_num_bytes(from);
          to.is_negative(BN_is_negative(from));
          BN_bn2bin( from, reinterpret_cast<unsigned char *>(to.allocate(sz)) );
        }
      }

      void from_bignum(const csl::sec::bignum & bn)
      {
        oBIGNUM::from_bignum(bn,this);
      }

      static void from_bignum(const csl::sec::bignum & from, oBIGNUM * to)
      {
        if( !to ) { THROWEXC("oBIGNUM 'to' is NULL!"); }
        else if( !(from.size()) ) { THROWEXC("oBIGNUM 'from' has no data!"); }
        {
          to->reset();
          to->bn_ = BN_bin2bn( from.data(), from.size(),NULL );
          if( !to->bn_ ) { THROWEXC("oBIGNUM cannot convert bignum!"); }
          BN_set_negative( to->bn_, from.is_negative() );
        }
      }
    };

    struct oBIGNUM_CTX
    {
      BN_CTX * ctx_;
      oBIGNUM_CTX() : ctx_(BN_CTX_new())
      {
        if( !ctx_ ) THROWEXC("oBIGNUM_CTX cannot allocate memory!");
      }
      ~oBIGNUM_CTX() { if(ctx_) BN_CTX_free(ctx_); }
    };

    int name_to_nid_(const char * name)
    {
      int alg_nid = OBJ_txt2nid(name);

      if( alg_nid == NID_undef )
      {
        static int init_once = 0;
        if( !init_once )
        {
          init_once = 1;
          if( OBJ_NAME_init() )
          {
            alg_nid = OBJ_txt2nid(name);
          }
        }
      }
      return alg_nid;
    }

    struct oEC_KEY
    {
      EC_KEY * key_;

      oEC_KEY() : key_(0) {}

      void reset() { if( key_ ) { EC_KEY_free(key_); key_ = 0; } }

      bool gen_key(const common::ustr & algname)
      {
        int alg_nid = NID_undef;

        /* need an algorithm name */
        if( algname.size() == 0 )
        {
          THROWEXC("oEC_KEY algorithm name is empty!");
        }

        /* reset key */
        this->reset();

        /* get algorithm id */
        if( (alg_nid = name_to_nid_(algname.c_str())) != NID_undef ) // TODO
        {
          if( (key_=EC_KEY_new_by_curve_name(alg_nid)) == NULL )
          {
            THROWEXC("oEC_KEY cannot allocate key!");
          }

          if( (EC_KEY_generate_key(key_)) == 0 )
          {
            THROWEXC("oEC_KEY key generation failed!");
          }

          return true;
        }
        return false;
      }

      bool init_key(const common::ustr & algname)
      {
        int alg_nid = NID_undef;

        /* need an algorithm name */
        if( algname.size() == 0 )
        {
          THROWEXC("oEC_KEY algorithm name is empty!");
        }

        /* reset key */
        this->reset();

        /* get algorithm id */
        if( (alg_nid = name_to_nid_(algname.c_str())) != NID_undef )
        {
          if( (key_=EC_KEY_new_by_curve_name(alg_nid)) == NULL )
          {
            THROWEXC("oEC_KEY cannot allocate key!");
          }
          return true;
        }
        return false;
      }

      bool get_private_key(sec::bignum & bn)
      {
        const BIGNUM * privk = NULL;

        if( (privk = EC_KEY_get0_private_key(key_)) == NULL )
        {
          THROWEXC("oEC_KEY cannot get public key!");
        }

        oBIGNUM::to_bignum(privk,bn);
        return true;
      }

      bool privkey_from_bignum( const sec::bignum & bn, const common::ustr & algname )
      {
        if( !init_key(algname) ) return false;

        oBIGNUM bnp;
        bnp.from_bignum(bn);

        if( !EC_KEY_set_private_key(key_,bnp.bn_) )
        {
          THROWEXC("oEC_KEY cannot set private key!");
        }

        return true;
      }

      const EC_GROUP * get_group()
      {
        const EC_GROUP * group = 0;
        if( !key_ )
        {
          THROWEXC("oEC_KEY empty key!");
        }
        if( (group = EC_KEY_get0_group(key_))==NULL )
        {
          THROWEXC("oEC_KEY invalid group!");
        }
        return group;
      }

      bool get_coordinates(oBIGNUM & x, oBIGNUM & y)
      {
        const EC_GROUP * group      = 0;
        const EC_POINT * public_key = 0;

        if( !key_ ) { return false; }

        if( (group = EC_KEY_get0_group(key_))==NULL )
        {
          THROWEXC("oEC_KEY cannot get group of key!");
        }

        oBIGNUM_CTX ctx;

        if( (public_key = EC_KEY_get0_public_key(key_)) == NULL )
        {
          THROWEXC("oEC_KEY cannot get public key!");
        }

        if( EC_METHOD_get_field_type(EC_GROUP_method_of(group)) == NID_X9_62_prime_field )
        {
          if( EC_POINT_get_affine_coordinates_GFp(group, public_key, x.bn_, y.bn_, ctx.ctx_) == 0)
          {
            THROWEXC("oEC_KEY cannot get affine coordinates!");
          }
        }
        else
        {
          if( EC_POINT_get_affine_coordinates_GF2m(group, public_key, x.bn_, y.bn_, ctx.ctx_) == 0)
          {
            THROWEXC("oEC_KEY cannot get affine coordinates!");
          }
        }
        return true;
      }

      ~oEC_KEY() { reset(); }
    };

    struct oEC_POINT
    {
      EC_POINT * point_;

      oEC_POINT() : point_(0) {}

      oEC_POINT(const EC_GROUP * g) : point_(EC_POINT_new(g))
      {
        if( !point_ ) { THROWEXC("oEC_POINT cannot allocate memory!"); }
      }

      ~oEC_POINT() { reset(); }

      void reset() { if(point_) { EC_POINT_free(point_); point_=0; } }

      bool from_coordinates( const sec::bignum & x,
                             const sec::bignum & y,
                             oEC_KEY & k )
      {
        oBIGNUM xp, yp;
        oBIGNUM_CTX c;

        if( x.is_empty() ) { THROWEXC("oEC_POINT x-coord empty"); }
        if( y.is_empty() ) { THROWEXC("oEC_POINT y-coord empty"); }

        xp.from_bignum(x);
        yp.from_bignum(y);

        const EC_GROUP * group = k.get_group();

        if( !group ) { THROWEXC("oEC_POINT cannot get group!"); }

        reset();

        if( (point_=EC_POINT_new(group)) == NULL ) { THROWEXC("oEC_POINT cannot allocate!"); }

        /* convert peer keys */
        if( EC_METHOD_get_field_type(EC_GROUP_method_of(group)) == NID_X9_62_prime_field )
        {
          if( EC_POINT_set_affine_coordinates_GFp(group, point_, xp.bn_, yp.bn_, c.ctx_) == 0)
          {
            THROWEXC("oEC_POINT cannot convert coordinates!");
          }
        }
        else
        {
          if( EC_POINT_set_affine_coordinates_GF2m(group, point_, xp.bn_, yp.bn_, c.ctx_) == 0)
          {
            THROWEXC("oEC_POINT cannot convert coordinates!");
          }
        }
        return true;
      }
    };
  }

  namespace sec
  {
    unsigned int ecdh_key::algorithm_strength(const common::ustr & algname)
    {
      int nid = NID_undef;
      EC_GROUP * group = 0;
      unsigned int strength = 0;

      if( !algname.size() ) return 0;

      /* get algorithm id */
      if( (nid = name_to_nid_(algname.c_str())) == NID_undef )
        return 0;  /* unknown algorithm */

      if( (group = EC_GROUP_new_by_curve_name(nid)) == NULL )
        return 0;  /* cannot allocate memory ? */

      strength = EC_GROUP_get_degree( group );

      EC_GROUP_free( group );
      return strength;
    }

    unsigned int ecdh_key::strength() const
    {
      return algorithm_strength(algname_);
    }

    bool ecdh_key::gen_keypair(bignum & private_key)
    {
      try
      {
        oEC_KEY k;
        oBIGNUM xx,yy;

        if( k.gen_key(algname_) && k.get_coordinates(xx,yy) )
        {
          xx.to_bignum(x_);
          yy.to_bignum(y_);

          return (k.get_private_key(private_key));
        }
      }
      catch(oEXC xc)
      {
        fprintf(stderr,"ECDH_KEY exception: %s at [%s:%d]\n",xc.reason_,__FILE__,xc.line_);
      }
      return false;
    }

    bool ecdh_key::gen_sha1hex_shared_key(const bignum & peer_private_key, common::ustr & shared_key) const
    {
      try
      {
        oEC_POINT pub_key;
        oEC_KEY   priv_key;

        /* convert keys */
        priv_key.privkey_from_bignum( peer_private_key, algname_ );
        pub_key.from_coordinates( x_, y_, priv_key );

        char tmpdigest[SHA1_HEX_DIGEST_STR_LENGTH];

        if( (ECDH_compute_key( tmpdigest,
                               SHA1_HEX_DIGEST_STR_LENGTH,
                               pub_key.point_,
                               priv_key.key_,
                               csl_sec_sha1_conv )) == 0 )
        {
          return false;
        }
        shared_key = tmpdigest;
        return true;
      }
      catch(oEXC xc)
      {
        fprintf(stderr,"ECDH_KEY exception: %s at [%s:%d]\n",xc.reason_,__FILE__,xc.line_);
      }
      return false;
    }

    bool ecdh_key::gen_shared_key(const bignum & peer_private_key, pbuf & shared_key) const
    {
      try
      {
        oEC_POINT pub_key;
        oEC_KEY   priv_key;

        /* convert keys */
        priv_key.privkey_from_bignum( peer_private_key, algname_ );
        pub_key.from_coordinates( x_, y_, priv_key );

        if( (ECDH_compute_key( &shared_key,
                               pbuf::buf_size,
                               pub_key.point_,
                               priv_key.key_,
                               save_to_pbuf_as_is_ )) == 0 )
        {
          return false;
        }
        return true;
      }
      catch(oEXC xc)
      {
        fprintf(stderr,"ECDH_KEY exception: %s at [%s:%d]\n",xc.reason_,__FILE__,xc.line_);
      }
      return false;
    }

    bool ecdh_key::to_xdr(xdrbuf & buf) const
    {
      bool ret = true;
      try
      {
        buf << algname_;
        if( !(x_.to_xdr( buf )) ) return false;
        if( !(y_.to_xdr( buf )) ) return false;
      }
      catch( csl::common::exc e )
      {
        ret = false;
      }
      return ret;
    }

    bool ecdh_key::from_xdr(xdrbuf & buf)
    {
      bool ret = true;
      try
      {
        char tmp[64];
        tmp[63] = 0;

        unsigned int sz = 0;
        ret = buf.get_data( reinterpret_cast<unsigned char *>(tmp),sz,64 );

        if( ret )
        {
          tmp[sz] = 0;
          algname_ = tmp;

          if( !(x_.from_xdr(buf)) ) return false;
          if( !(y_.from_xdr(buf)) ) return false;

          return true;
        }
        else
        {
          return false;
        }
      }
      catch( csl::common::exc e )
      {
        ret = false;
      }
      return ret;
    }

    ecdh_key::ecdh_key() { }
    ecdh_key::~ecdh_key() { }

    ecdh_key::ecdh_key(const ecdh_key & other) : algname_(other.algname_), x_(other.x_), y_(other.y_) { }

    ecdh_key & ecdh_key::operator=(const ecdh_key & other)
    {
      this->set( other.algname_, other.x_, other.y_ );
      return *this;
    }

    void ecdh_key::print() const
    {
      PRINTF(L"ECDH_KEY[%s]:\n",(algname_.size() > 0 ? algname_.c_str() : "EMPTY ALG"));
      PRINTF(L"  X key: "); x_.print();
      PRINTF(L"  Y key: "); y_.print();
    }

    void ecdh_key::set(const common::ustr & algnamee, const bignum & xx, const bignum & yy)
    {
      algname_ = algnamee;
      x_       = xx;
      y_       = yy;
    }
  }
}

/* EOF */
