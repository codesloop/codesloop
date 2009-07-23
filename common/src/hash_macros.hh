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

#ifndef _csl_common_hash_macros_hh_included_
#define _csl_common_hash_macros_hh_included_
#ifdef __cplusplus

#define RCUP( P ) (reinterpret_cast<unsigned char *>((P)))
#define SCUCH( P ) (static_cast<unsigned char>((P)))
#define SCUSH( P ) (static_cast<unsigned short>((P)))
#define SCUIN( P ) (static_cast<unsigned int>((P)))
#define SCULL( P ) (static_cast<unsigned long long>((P)))

#define CTOULL1( A ) \
  (( SCULL((A)&0xFF) ))

#define PTOULL1( P, R ) do { (R) = CTOULL1( *(RCUP(P)) ); } while(0)

#define CTOULL2( A, B ) \
  ( (SCULL(A) << 8) + \
     SCULL(B) )

#define PTOULL2( P, R ) \
  do { (R) = CTOULL2( *(RCUP(P)),*(RCUP(P)+1) ); } while(0)

#define CTOULL3( A, B, C ) \
  ( (SCULL(A) << 16) + \
    (SCULL(B) << 8) + \
     SCULL(C) )

#define PTOULL3( P, R ) \
  do { (R) = CTOULL3( *(RCUP(P)),*(RCUP(P)+1),*(RCUP(P)+2) ); } while(0)

#define CTOULL4( A, B, C, D ) \
  ( (SCULL(A) << 24) + \
    (SCULL(B) << 16) + \
    (SCULL(C) << 8) + \
     SCULL(D) )

#define PTOULL4( P, R ) \
  do { \
    (R) = CTOULL4( *(RCUP(P)),*(RCUP(P)+1),*(RCUP(P)+2), *(RCUP(P)+3) ); \
  } while(0)

#define CTOULL5( A, B, C, D, E ) \
  ( (SCULL(A) << 32) + \
    (SCULL(B) << 24) + \
    (SCULL(C) << 16) + \
    (SCULL(D) << 8) + \
     SCULL(E) )

#define PTOULL5( P, R ) \
  do { \
    (R) = CTOULL5( *(RCUP(P)),*(RCUP(P)+1),*(RCUP(P)+2), \
             *(RCUP(P)+3),*(RCUP(P)+4) ); \
  } while(0)

#define CTOULL6( A, B, C, D, E, F ) \
  ( (SCULL(A) << 40) + \
    (SCULL(B) << 32) + \
    (SCULL(C) << 24) + \
    (SCULL(D) << 16) + \
    (SCULL(E) << 8) + \
     SCULL(F) )

#define PTOULL6( P, R ) \
  do { \
    (R) = CTOULL6( *(RCUP(P)),*(RCUP(P)+1),*(RCUP(P)+2), \
             *(RCUP(P)+3),*(RCUP(P)+4),*(RCUP(P)+5) ); \
  } while(0)


#define CTOULL7( A, B, C, D, E, F, G ) \
  ( (SCULL(A) << 48) + \
    (SCULL(B) << 40) + \
    (SCULL(C) << 32) + \
    (SCULL(D) << 24) + \
    (SCULL(E) << 16) + \
    (SCULL(F) << 8) + \
     SCULL(G) )

#define PTOULL7( P, R ) \
  do { \
    (R) = CTOULL7( *(RCUP(P)),*(RCUP(P)+1),*(RCUP(P)+2), \
             *(RCUP(P)+3),*(RCUP(P)+4),*(RCUP(P)+5), \
             *(RCUP(P)+6) ); \
  } while(0)

#define CTOULL8( A, B, C, D, E, F, G, H ) \
  ( (SCULL(A) << 56) + \
    (SCULL(B) << 48) + \
    (SCULL(C) << 40) + \
    (SCULL(D) << 32) + \
    (SCULL(E) << 24) + \
    (SCULL(F) << 16) + \
    (SCULL(G) << 8) + \
    (SCULL(H) ) )

#define PTOULL8( P, R ) \
  do { \
    (R) = CTOULL8( *(RCUP(P)),*(RCUP(P)+1),*(RCUP(P)+2), \
             *(RCUP(P)+3),*(RCUP(P)+4),*(RCUP(P)+5), \
             *(RCUP(P)+6),*(RCUP(P)+7) ); \
  } while(0)

#define ULLTOC2( PTR, POS, L ) \
  (PTR)[(POS)]   = SCUCH( (L)>>8 ); \
  (PTR)[(POS)+1] = SCUCH( (L) );

#define ULLTOC3( PTR, POS, L ) \
  (PTR)[(POS)]   = SCUCH( ((L)>>16) ); \
  (PTR)[(POS)+1] = SCUCH( ((L)>>8) ); \
  (PTR)[(POS)+2] = SCUCH( (L) );

#define ULLTOC4( PTR, POS, L ) \
  (PTR)[(POS)]   = SCUCH( ((L)>>24) ); \
  (PTR)[(POS)+1] = SCUCH( ((L)>>16) ); \
  (PTR)[(POS)+2] = SCUCH( ((L)>>8) ); \
  (PTR)[(POS)+3] = SCUCH( (L) );

#define ULLTOC5( PTR, POS, L ) \
  (PTR)[(POS)]   = SCUCH( ((L)>>32) ); \
  (PTR)[(POS)+1] = SCUCH( ((L)>>24) ); \
  (PTR)[(POS)+2] = SCUCH( ((L)>>16) ); \
  (PTR)[(POS)+3] = SCUCH( ((L)>>8) ); \
  (PTR)[(POS)+4] = SCUCH( (L) );

#define ULLTOC6( PTR, POS, L ) \
  (PTR)[(POS)]   = SCUCH( ((L)>>40) ); \
  (PTR)[(POS)+1] = SCUCH( ((L)>>32) ); \
  (PTR)[(POS)+2] = SCUCH( ((L)>>24) ); \
  (PTR)[(POS)+3] = SCUCH( ((L)>>16) ); \
  (PTR)[(POS)+4] = SCUCH( ((L)>>8) ); \
  (PTR)[(POS)+5] = SCUCH( (L) );

#define ULLTOC7( PTR, POS, L ) \
  (PTR)[(POS)]   = SCUCH( ((L)>>48) ); \
  (PTR)[(POS)+1] = SCUCH( ((L)>>40) ); \
  (PTR)[(POS)+2] = SCUCH( ((L)>>32) ); \
  (PTR)[(POS)+3] = SCUCH( ((L)>>24) ); \
  (PTR)[(POS)+4] = SCUCH( ((L)>>16) ); \
  (PTR)[(POS)+5] = SCUCH( ((L)>>8) ); \
  (PTR)[(POS)+6] = SCUCH( (L) );

#define ULLTOC8( PTR, POS, L ) \
  (PTR)[(POS)]   = SCUCH( ((L)>>56) ); \
  (PTR)[(POS)+1] = SCUCH( ((L)>>48) ); \
  (PTR)[(POS)+2] = SCUCH( ((L)>>40) ); \
  (PTR)[(POS)+3] = SCUCH( ((L)>>32) ); \
  (PTR)[(POS)+4] = SCUCH( ((L)>>24) ); \
  (PTR)[(POS)+5] = SCUCH( ((L)>>16) ); \
  (PTR)[(POS)+6] = SCUCH( ((L)>>8) ); \
  (PTR)[(POS)+7] = SCUCH( (L) );

#define EXGET1( PTR,POS,JMP,RES ) \
  do { \
    unsigned long long __tmp_ex1__; PTOULL1( (PTR)+(POS),__tmp_ex1__ ); \
    (JMP) = ((__tmp_ex1__ & 1ULL) == 1ULL); \
    (RES) = (__tmp_ex1__ >> 1); \
  } while(0)

#define EXPUT1( PTR,POS,JMP,RES ) \
  do { \
    unsigned char __tmp_ex1__ = SCUCH(RES<<1)+(JMP&1); \
    (PTR)[(POS)] = __tmp_ex1__; \
  } while(0)

#define EXGET2( PTR,POS,JMP,RES ) \
  do { \
    unsigned long long __tmp_ex2__; PTOULL2( (PTR)+((POS)<<1),__tmp_ex2__ ); \
    (JMP) = ((__tmp_ex2__ & 1ULL) == 1ULL); \
    (RES) = (__tmp_ex2__ >> 1); \
  } while(0)

#define EXPUT2( PTR,POS,JMP,RES ) \
  do { \
    unsigned short __tmp_ex2__ = SCUSH(RES<<1)+(JMP&1); \
    ULLTOC2( (PTR),((POS)<<1), __tmp_ex2__ ); \
  } while(0)

#define EXGET3( PTR,POS,JMP,RES ) \
  do { \
    unsigned long long __tmp_ex3__; PTOULL3( (PTR)+((POS)*3),__tmp_ex3__ ); \
    (JMP) = ((__tmp_ex3__ & 1ULL) == 1ULL); \
    (RES) = ( __tmp_ex3__ >> 1); \
  } while(0)

#define EXPUT3( PTR,POS,JMP,RES ) \
  do { \
    unsigned int __tmp_ex3__ = SCUIN(RES<<1)+(JMP&1); \
    ULLTOC3( (PTR),((POS)*3), __tmp_ex3__ ); \
  } while(0)

#define EXGET4( PTR,POS,JMP,RES ) \
  do { \
    unsigned long long __tmp_ex4__; PTOULL4( (PTR)+(((POS)<<2)),__tmp_ex4__ ); \
    (JMP) = ((__tmp_ex4__ & 1ULL) == 1ULL); \
    (RES) = (__tmp_ex4__ >> 1); \
  } while(0)

#define EXPUT4( PTR,POS,JMP,RES ) \
  do { \
    unsigned int __tmp_ex4__ = SCUIN(RES<<1)+(JMP&1); \
    ULLTOC4( (PTR),((POS)<<2), __tmp_ex4__ ); \
  } while(0)

#define EXGET5( PTR,POS,JMP,RES ) \
  do { \
    unsigned long long __tmp_ex5__; PTOULL5( (PTR)+((POS)*5),__tmp_ex5__ ); \
    (JMP) = ((__tmp_ex5__ & 1ULL) == 1ULL); \
    (RES) = (__tmp_ex5__ >> 1); \
  } while(0)

#define EXPUT5( PTR,POS,JMP,RES ) \
  do { \
    unsigned long long __tmp_ex5__ = SCULL(RES<<1)+(JMP&1); \
    ULLTOC5( (PTR),((POS)*5), __tmp_ex5__ ); \
  } while(0)

#define EXGET6( PTR,POS,JMP,RES ) \
  do { \
    unsigned long long __tmp_ex6__; PTOULL6( (PTR)+((POS)*6),__tmp_ex6__ ); \
    (JMP) = ((__tmp_ex6__ & 1ULL) == 1ULL); \
    (RES) = (__tmp_ex6__ >> 1); \
  } while(0)

#define EXPUT6( PTR,POS,JMP,RES ) \
  do { \
    unsigned long long __tmp_ex6__ = SCULL(RES<<1)+(JMP&1); \
    ULLTOC6( (PTR),((POS)*6), __tmp_ex6__ ); \
  } while(0)

#define EXGET7( PTR,POS,JMP,RES ) \
  do { \
    unsigned long long __tmp_ex7__; PTOULL7( (PTR)+((POS)*7),__tmp_ex7__ ); \
    (JMP) = ((__tmp_ex7__ & 1ULL) == 1ULL); \
    (RES) = (__tmp_ex7__ >> 1); \
  } while(0)

#define EXPUT7( PTR,POS,JMP,RES ) \
  do { \
    unsigned long long __tmp_ex7__ = SCULL(RES<<1)+(JMP&1); \
    ULLTOC7( (PTR),((POS)*7), __tmp_ex7__ ); \
  } while(0)

#define EXGET8( PTR,POS,JMP,RES ) \
  do { \
    unsigned long long __tmp_ex8__; PTOULL8( (PTR)+((POS)<<3),__tmp_ex8__ ); \
    (JMP) = ((__tmp_ex8__ & 1ULL) == 1ULL); \
    (RES) = (__tmp_ex8__ >> 1); \
  } while(0)

#define EXPUT8( PTR,POS,JMP,RES ) \
  do { \
    unsigned long long __tmp_ex8__ = SCULL(RES<<1)+(JMP&1); \
    ULLTOC8( (PTR),((POS)<<3), __tmp_ex8__ ); \
  } while(0)

#define DTOFF1( PTR,POS,OFF ) PTOULL1( (PTR)+(POS*2), OFF )
#define DTSZ1( PTR,POS,SZ )   PTOULL1( (PTR)+((POS*2)+1), SZ )

#define DTOFF2( PTR,POS,OFF ) PTOULL2( (PTR)+(POS*4), OFF )
#define DTSZ2( PTR,POS,SZ )   PTOULL2( (PTR)+((POS*4)+2), SZ )

#define DTOFF3( PTR,POS,OFF ) PTOULL3( (PTR)+(POS*6), OFF )
#define DTSZ3( PTR,POS,SZ )   PTOULL3( (PTR)+((POS*6)+3), SZ )

#define DTOFF4( PTR,POS,OFF ) PTOULL4( (PTR)+(POS*8), OFF )
#define DTSZ4( PTR,POS,SZ )   PTOULL4( (PTR)+((POS*8)+4), SZ )

#define DTOFF5( PTR,POS,OFF ) PTOULL5( (PTR)+(POS*10), OFF )
#define DTSZ5( PTR,POS,SZ )   PTOULL5( (PTR)+((POS*10)+5), SZ )

#define DTOFF6( PTR,POS,OFF ) PTOULL6( (PTR)+(POS*12), OFF )
#define DTSZ6( PTR,POS,SZ )   PTOULL6( (PTR)+((POS*12)+6), SZ )

#define DTOFF7( PTR,POS,OFF ) PTOULL7( (PTR)+(POS*14), OFF )
#define DTSZ7( PTR,POS,SZ )   PTOULL7( (PTR)+((POS*14)+7), SZ )

#define DTOFF8( PTR,POS,OFF ) PTOULL8( (PTR)+(POS*16), OFF )
#define DTSZ8( PTR,POS,SZ )   PTOULL8( (PTR)+((POS*16)+8), SZ )

#endif /* __cplusplus */
#endif /* _csl_common_hash_macros_hh_included_ */
