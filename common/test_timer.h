/*
Copyright (c) 2008,2009,2010, CodeSLoop Team

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

#ifndef _csl_common_test_timer_h_included_
#define _csl_common_test_timer_h_included_

#include "common.h"

/**
   @file test_timer.h
   @brief Performance testing helpers

   These function runs the given callback function iteratively until
   the predefined MAX_SAMPLING_INTERVAL_MS is not reached. In each iteration
   the number of loops will double.

   The default value for MAX_SAMPLING_INTERVAL_MS is 1700 ms. This can be
   changed at compile time by -DMAX_SAMPLING_INTERVAL_MS=othervalue

   @struct csl_common_timer_result
   @brief Performance testing results are returned in this struct
 */

struct csl_common_timer_result
{
  double   ms_per_call;  /**< @brief avarage ms spent in the function  */
  double   call_per_sec; /**< @brief avarage function calls per second */
  double   total_ms;     /**< @brief total ms spent in testing         */
  size_t   n_loops;      /**< @brief number of test loops executed     */
};

/**
   @brief Tests a parameterless function
   @param test_function The function to be tested
   @return Measurement results
 */
CSL_CDECL
struct csl_common_timer_result
csl_common_test_timer_v0( void (*test_function)(void) );

/**
   @brief Tests a single parameter function
   @param test_function The function to be tested
   @param param This will be given to the test_function in each iteration
   @return Measurement results
 */
CSL_CDECL
struct csl_common_timer_result
csl_common_test_timer_i1( void (*test_function)(int), int param );

/**
   @brief Very simple print function for the lazy
   @param prefix Each line will be prefixed with this string
   @param tr The performance measurement results
   @param postfix This will be appended to each line
 */
CSL_CDECL
void
csl_common_print_results(
      const char * prefix,
      struct csl_common_timer_result tr,
      const char * postfix );

#endif /* _csl_common_test_timer_h_included_ */
