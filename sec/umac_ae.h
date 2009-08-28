/* --------------------------------------------------------------------------
 * UMAC_AE Implementation by Ted Krovetz (tdk@acm.org).
 * Version: 0.01 (14 July 2006)
 * This implementation is herby placed in the public domain.
 * The author offers no warranty. Use at your own risk.
 * Please send bug reports to the author.
 * ----------------------------------------------------------------------- */

#ifndef HEADER_UMAC_AE_H
#define HEADER_UMAC_AE_H

#include "umac.h"

/* NOTES
 *
 * -- There is no coresponding umac_ae.c. Instead, include umac.c and ensure
 * in that file that UMAC_AE_SUPPORT is set to a non-zero value.
 *
 * -- This implementation only supports 64-bit nonces
 *
 * -- This implementation only supports 32-bit byte-length strings
 */

#define UMAC_AE_USE_STDINT 1  /* Set to zero if system has no stdint.h        */
 
#if UMAC_AE_USE_STDINT && !_MSC_VER /* Try stdint.h if non-Microsoft          */
#ifdef  __cplusplus
#define __STDC_CONSTANT_MACROS
#endif
#include <stdint.h>
#elif (_MSC_VER)                  /* Microsoft C does not have stdint.h    */
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#define UINT64_C(v) v ## UI64
#else                             /* Guess sensibly - may need adaptation  */
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
#define UINT64_C(v) v ## ULL
#endif


typedef struct {
	unsigned int lengths[3];  /* hbytes, tbytes, fbytes */
    char         nonce[8];
	umac_ctx_t   umac_ctx;
} umac_ae_ctx_t;


/* ----------------------------------------------------------------------- */
#ifdef  __cplusplus
extern "C" {
#endif
/* ----------------------------------------------------------------------- */

void umac_ae_set_key(char user_key[], umac_ae_ctx_t *ctx);
/* Alloates a umac_ctx structure and fills *ctx */

void umac_ae_done(umac_ae_ctx_t *ctx);
/* frees the umac_ctx structure and zeros *ctx */

void umac_ae_abort(umac_ae_ctx_t *ctx);
/* Resets internal variables without finishing current work */


/* Given Header H, Plaintext P and Footer F, you must call in the
 * following sequence: umac_ae_header(H) [if H non-empty], umac_ae_encrypt(P),
 * umac_ae_footer(F) [if F non-empty]. If H or F is empty, do not call the
 * corresponding function.
 */

void umac_ae_header(char *h,
         unsigned int hbytes,
         umac_ae_ctx_t *ctx);

void umac_ae_footer(char *f,
         unsigned int fbytes,
         umac_ae_ctx_t *ctx);

void umac_ae_crypt(char *it, 		/* in text  */
		 char *ot,          		/* out text */
		 char *at,           		/* authentication text (it/ot) */
         unsigned int tbytes,       /* text byte-length */
         char n[8],          		/* 8-byte nonces only */
		 umac_ae_ctx_t *ctx);

void umac_ae_finalize(char *tag, umac_ae_ctx_t *ctx);
/* Returns the authentication tag as a string of length UMAC_OUTPUT_LEN */

/* ----------------------------------------------------------------------- */
#ifdef  __cplusplus
}
#endif
/* ----------------------------------------------------------------------- */

/* Encryption and decryption differ only in what gets authenticated */
#define umac_ae_encrypt(pt,ct,tbytes,n,ctx) \
           umac_ae_crypt(pt,ct,ct,tbytes,n,ctx) 

#define umac_ae_decrypt(ct,pt,tbytes,n,ctx) \
           umac_ae_crypt(ct,pt,ct,tbytes,n,ctx) 

/* ----------------------------------------------------------------------- */
#endif /* HEADER_UMAC_AE_H */
