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

#include <stdio.h>
#include <stdlib.h>
#include "test_timer.h"

/**
  @file test_timer.c
  @brief implementation of performance timers
*/

#ifndef MAX_SAMPLING_INTERVAL_MS
#define MAX_SAMPLING_INTERVAL_MS 1700.0
#endif /* MAX_SAMPLING_INTERVAL_MS */

static double calc_elapsed( const struct timeval * tv )
{
  long sec_diff = 0;
  long usec_diff = 0;
  double ms_diff = 0.0;

  struct timeval tv_new;
  gettimeofday( &tv_new,NULL );

  sec_diff    = tv_new.tv_sec  - tv->tv_sec;
  usec_diff   = tv_new.tv_usec - tv->tv_usec;
  ms_diff = (sec_diff*1000000.0 + usec_diff*1.0)/1000.0;

  return ms_diff;
}

CSL_CDECL
struct csl_common_timer_result
csl_common_test_timer_v0( void (*test_function)(void) )
{
  struct csl_common_timer_result rs;
  struct timeval start;
  int loop = 2;
  int i = 0;
  size_t nl = 0;
  double ela = 0.0;

  gettimeofday( &start,NULL );

  while( (ela = calc_elapsed(&start)) < MAX_SAMPLING_INTERVAL_MS )
  {
    for( i=0;i<loop;++i )
    {
      test_function();
      nl++;
    }
    loop *= 2;
  }
  rs.n_loops = nl;
  rs.total_ms = ela;
  rs.ms_per_call = ela/(double)nl;
  rs.call_per_sec = nl*1000.0/ela;
  return rs;
}

CSL_CDECL
struct csl_common_timer_result
csl_common_test_timer_i1( void (*test_function)(int), int param )
{
  struct csl_common_timer_result rs;
  struct timeval start;
  int loop = 2;
  int i = 0;
  size_t nl = 0;
  double ela = 0.0;

  gettimeofday( &start,NULL );

  while( (ela = calc_elapsed(&start)) < MAX_SAMPLING_INTERVAL_MS )
  {
    for( i=0;i<loop;++i )
    {
      test_function(param);
      nl++;
    }
    loop *= 2;
  }
  rs.n_loops = nl;
  rs.total_ms = ela;
  rs.ms_per_call = ela/(double)nl;
  rs.call_per_sec = nl*1000.0/ela;
  return rs;
}

CSL_CDECL
void
csl_common_print_results(
        const char * prefix,
        struct csl_common_timer_result tr,
        const char * postfix )
{
  printf("%s   %6.3f ms, %8ld calls,   %f ms/call,   %f calls/sec   %s\n",
    prefix,
    tr.total_ms,
    (unsigned long)tr.n_loops,
    tr.ms_per_call,
    tr.call_per_sec,
    postfix );
}

/* EOF */
