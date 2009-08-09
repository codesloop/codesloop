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

/**
   @file t__derive_ec_params.c
   @brief tests to see how EC and OpenSSL works
*/

#ifdef WIN32
int main() { return 0; }
#else /* WIN32 */
#include <openssl/ec.h>
#include <openssl/ecdh.h>
#include <openssl/objects.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>

static struct timeval tv_;

static double print_elapsed( const char * prefix )
{
  struct timeval old = tv_;
  gettimeofday( &tv_,NULL );

  long sec_diff    = tv_.tv_sec  - old.tv_sec;
  long usec_diff   = tv_.tv_usec - old.tv_usec;
  double ms_diff = (sec_diff*1000000.0 + usec_diff*1.0)/1000.0;
  printf("%s : %f ms",prefix,ms_diff);
  return ms_diff;
}

static void * sha1_conv(const void *in, size_t inlen, void *out, size_t *outlen)
{
  if (*outlen < SHA_DIGEST_LENGTH)
    return NULL;
  else
    *outlen = SHA_DIGEST_LENGTH;
  return SHA1(in, inlen, out);
}

struct test_results
{
  long degree;
  size_t x_sz_hx;
  size_t y_sz_hx;
  size_t pubkey_sz_hx;
};

static size_t get_len_and_openssl_free( char * disposable_string )
{
  size_t ret = 0;
  assert( disposable_string != NULL );
  if( !disposable_string ) return 0;
  ret = strlen( disposable_string );
  OPENSSL_free( disposable_string );
  return ret;
}

static struct test_results do_ec_test(const char * ec_alg)
{
  int               rc        = 0;
  int               alg_nid   = NID_undef;
  EC_KEY *          eckey_a   = NULL;
  EC_KEY *          eckey_b   = NULL;
  const EC_GROUP *  group     = NULL;
  long              degree    = 0;
  BIGNUM *          x_a       = NULL;
  BIGNUM *          y_a       = NULL;
  BIGNUM *          x_b       = NULL;
  BIGNUM *          y_b       = NULL;
  BN_CTX *          ctx       = NULL;
  char              sesskey_a[SHA_DIGEST_LENGTH];
  char              sesskey_b[SHA_DIGEST_LENGTH];
  char              sesskey_a2[SHA_DIGEST_LENGTH];
  char              sesskey_b2[SHA_DIGEST_LENGTH];
  const EC_POINT *  pubkey_a = NULL;
  const EC_POINT *  pubkey_b = NULL;
  EC_POINT *        pubkey_a2 = NULL;
  EC_POINT *        pubkey_b2 = NULL;

  struct test_results retval;

  memset( sesskey_a,1,sizeof(sesskey_a) );
  memset( sesskey_b,2,sizeof(sesskey_b) );
  memset( sesskey_a2,3,sizeof(sesskey_a2) );
  memset( sesskey_b2,4,sizeof(sesskey_b2) );

  assert( (ctx = BN_CTX_new()) != NULL );
  assert( (alg_nid = OBJ_txt2nid(ec_alg) ) != NID_undef );

  assert( (eckey_a = EC_KEY_new_by_curve_name(alg_nid)) != NULL );
  assert( (eckey_b = EC_KEY_new_by_curve_name(alg_nid)) != NULL );

  assert( (group = EC_KEY_get0_group(eckey_a)) );

  assert( (x_a = BN_new()) != NULL);
  assert( (y_a = BN_new()) != NULL);
  assert( (x_b = BN_new()) != NULL);
  assert( (y_b = BN_new()) != NULL);

  assert( (degree = EC_GROUP_get_degree(group)) >= 160 );
  assert( (degree = EC_GROUP_get_degree(EC_KEY_get0_group(eckey_a))) >= 160 );
  assert( (degree = EC_GROUP_get_degree(EC_KEY_get0_group(eckey_b))) >= 160 );

  assert( (EC_KEY_generate_key(eckey_a)) != 0 );
  assert( (EC_KEY_generate_key(eckey_b)) != 0 );

  assert( (pubkey_a = EC_KEY_get0_public_key(eckey_a)) != NULL );
  assert( (pubkey_b = EC_KEY_get0_public_key(eckey_b)) != NULL );

  assert( (pubkey_a2 = EC_POINT_new(group)) != NULL );
  assert( (pubkey_b2 = EC_POINT_new(group)) != NULL );


  if( EC_METHOD_get_field_type(EC_GROUP_method_of(group)) == NID_X9_62_prime_field)
  {
    assert( (EC_POINT_get_affine_coordinates_GFp(group, pubkey_a, x_a, y_a, ctx)) != 0 );
    assert( (EC_POINT_get_affine_coordinates_GFp(group, pubkey_b, x_b, y_b, ctx)) != 0 );

    assert( (EC_POINT_set_affine_coordinates_GFp(group, pubkey_a2, x_a, y_a, ctx)) != 0 );
    assert( (EC_POINT_set_affine_coordinates_GFp(group, pubkey_b2, x_b, y_b, ctx)) != 0 );
  }
  else
  {
    assert( (EC_POINT_get_affine_coordinates_GF2m(group, pubkey_a, x_a, y_a, ctx)) != 0 );
    assert( (EC_POINT_get_affine_coordinates_GF2m(group, pubkey_b, x_b, y_b, ctx)) != 0 );

    assert( (EC_POINT_set_affine_coordinates_GF2m(group, pubkey_a2, x_a, y_a, ctx)) != 0 );
    assert( (EC_POINT_set_affine_coordinates_GF2m(group, pubkey_b2, x_b, y_b, ctx)) != 0 );
  }

  assert( (rc = ECDH_compute_key(sesskey_a, SHA_DIGEST_LENGTH, pubkey_b, eckey_a, sha1_conv)) != 0 );
  assert( (rc = ECDH_compute_key(sesskey_b, SHA_DIGEST_LENGTH, pubkey_a, eckey_b, sha1_conv)) != 0 );

  assert( memcmp( sesskey_a, sesskey_b, SHA_DIGEST_LENGTH ) == 0 );

  assert( (rc = ECDH_compute_key(sesskey_a2, SHA_DIGEST_LENGTH, pubkey_b2, eckey_a, sha1_conv)) != 0 );
  assert( (rc = ECDH_compute_key(sesskey_b2, SHA_DIGEST_LENGTH, pubkey_a2, eckey_b, sha1_conv)) != 0 );

  assert( memcmp( sesskey_a2, sesskey_b2, SHA_DIGEST_LENGTH ) == 0 );
  assert( memcmp( sesskey_a, sesskey_a2, SHA_DIGEST_LENGTH ) == 0 );

  retval.degree = degree;
  retval.x_sz_hx = BN_num_bytes(x_a); /* get_len_and_openssl_free(BN_bn2hex(x_a)); */
  retval.y_sz_hx = BN_num_bytes(y_a); /* get_len_and_openssl_free(BN_bn2hex(y_a)); */
  retval.pubkey_sz_hx = get_len_and_openssl_free(EC_POINT_point2hex( group, pubkey_a, POINT_CONVERSION_UNCOMPRESSED, ctx ));

  EC_KEY_free( eckey_a );
  EC_KEY_free( eckey_b );

  EC_POINT_free( pubkey_a2 );
  EC_POINT_free( pubkey_b2 );

  BN_free(x_a);
  BN_free(y_a);
  BN_free(x_b);
  BN_free(y_b);

  BN_CTX_free(ctx);
  return retval;
}

char * algs_subset[] = {
  "prime192v3", // rel speed  7 : >80b
  "prime256v1", // rel speed 13 : 128b
  "secp384r1",  // rel speed 33 : 192b
  "secp521r1",  // rel speed 65 : 256b
  NULL };


char * algs_all[] = {
 "secp160k1",	 "secp160r1",	 "secp160r2",	 "secp192k1",	 "secp224k1",
 "secp224r1",	 "secp256k1",	 "secp384r1",	 "secp521r1",	 "prime192v1",
 "prime192v2",	 "prime192v3",	 "prime239v1",	 "prime239v2",	 "prime239v3",
 "prime256v1",	 "sect163k1",	 "sect163r1",	 "sect163r2",	 "sect193r1",
 "sect193r2",	 "sect233k1",	 "sect233r1",	 "sect239k1",	 "sect283k1",
 "sect283r1",	 "sect409k1",	 "sect409r1",	 "sect571k1",	 "sect571r1",
 "c2pnb163v1",	 "c2pnb163v2",	 "c2pnb163v3",	 "c2pnb176v1",	 "c2tnb191v1",
 "c2tnb191v2",	 "c2tnb191v3",	 "c2pnb208w1",	 "c2tnb239v1",	 "c2tnb239v2",
 "c2tnb239v3",	 "c2pnb272w1",	 "c2pnb304w1",	 "c2tnb359v1",	 "c2pnb368w1",
 "c2tnb431r1", NULL };

int main()
{
  char **  a      = algs_all;
  int      i      = 0;
  long     degree = 0;
  long     pub_sz = 0;
  long     x_sz   = 0;
  long     y_sz   = 0;
  double   ms     = 0.0;
  char     tmp[1024];

  struct test_results res;

  gettimeofday( &tv_,NULL );

  while( *a )
  {
    degree = 0;
    pub_sz = 0;
    x_sz   = 0;
    y_sz   = 0;

    printf("Testing %s ", *a );

    for( i=0;i<8;++i )
    {
      res     = do_ec_test( *a );
      degree += res.degree;
      pub_sz += res.pubkey_sz_hx;
      x_sz   += res.x_sz_hx;
      y_sz   += res.y_sz_hx;
    }
    ms = print_elapsed( "result" );

    sprintf(tmp,"[%f avg ms] degree: %ld degree/ms: %f "
                " [pub/x/y: %ld/%ld/%ld] hex bytes ",
      ms/(double)i,
      degree/i,
      (double)degree/ms/(double)i,
      pub_sz/i,
      x_sz/i,
      y_sz/i );

    if( degree > 520*i )
      printf(" %s [sym256]\n", tmp );
    else if( degree > 383*i )
      printf(" %s [sym192]\n", tmp );
    else if( degree > 255*i )
      printf(" %s [sym128]\n", tmp );
    else if( degree > 160*i )
      printf(" %s [sym80]\n", tmp );
    else
      printf(" %s [WEAK]\n", tmp );

    ++a;
  }

  return 0;
}

#endif /* WIN32 */

/* EOF */
