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

/**
   @file t__pt_mutex.cc
   @brief Tests to check pthread mutex behaviour
*/

#include "codesloop/common/test_timer.h"
#include <stdio.h>
#ifndef WIN32
# include <pthread.h> 
# include <unistd.h>
#endif /* WIN32 */
#include <memory>

/** @brief contains tests related to pthread mutexes */
namespace test_pt_mutex
{
  /** @test init */
  void test_init()
  {
    pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_init(&m,NULL);
    pthread_mutex_destroy(&m);
  }

  /** @test init w/ attributes */
  void test_init2()
  {
    pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutexattr_t a;

    pthread_mutexattr_init(&a);
    pthread_mutexattr_settype(&a,PTHREAD_MUTEX_NORMAL);
    pthread_mutex_init(&m,&a);
    pthread_mutex_destroy(&m);
  }

  /** @test lock/unlock */
  void test_lock_unlock()
  {
    pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutexattr_t a;

    pthread_mutexattr_init(&a);
    pthread_mutexattr_settype(&a,PTHREAD_MUTEX_NORMAL);
    pthread_mutex_init(&m,&a);
    pthread_mutex_lock(&m);
    pthread_mutex_unlock(&m);
    pthread_mutex_destroy(&m);
  }

  /** @test lock/unlock */
  void test_lock_unlock2()
  {
    pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&m);
    pthread_mutex_unlock(&m);
  }

  extern "C" void key_function(void * p)
  {
    /* dummy: do nothing */
  }

  /** @test lock/unlock and key create */
  void test_lck_key_create()
  {
    pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutexattr_t a;
    pthread_key_t k;

    pthread_mutexattr_init(&a);
    pthread_mutexattr_settype(&a,PTHREAD_MUTEX_NORMAL);
    pthread_mutex_init(&m,&a);
    pthread_key_create(&k,key_function);
    pthread_mutex_lock(&m);
    pthread_mutex_unlock(&m);
    pthread_mutex_destroy(&m);
    pthread_key_delete(k);
  }

  /** @test lock/unlock and key create */
  void test_lck_key_create2()
  {
    pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutexattr_t a;
    pthread_key_t k;

    pthread_mutexattr_init(&a);
    pthread_mutexattr_settype(&a,PTHREAD_MUTEX_NORMAL);
    pthread_mutex_init(&m,&a);
    pthread_key_create(&k,key_function);
    pthread_setspecific(k,NULL);
    pthread_mutex_lock(&m);
    pthread_mutex_unlock(&m);
    pthread_setspecific(k,NULL);
    pthread_mutex_destroy(&m);
    pthread_key_delete(k);
  }

  struct mtx1
  {
    pthread_mutex_t lock_;
    mtx1()  { pthread_mutex_init(&lock_,NULL); }
    ~mtx1() { pthread_mutex_destroy(&lock_); }
    void lock()   { pthread_mutex_lock(&lock_); }
    void unlock() { pthread_mutex_unlock(&lock_); }
  };

  /** @test test mtx1 */
  void test_mtx1()
  {
    mtx1 m;
    m.lock();
    m.unlock();
  }

  static mtx1 mtx1a__;

  /** @test test mtx1 lock/unlock */
  void test_mtx1a()
  {
    mtx1a__.lock();
    mtx1a__.unlock();
  }

  class mtx2
  {
  private:
    struct impl
    {
      pthread_mutex_t lock_;
      impl()        { pthread_mutex_init(&lock_,NULL); }
      ~impl()       { pthread_mutex_destroy(&lock_); }
      void lock()   { pthread_mutex_lock(&lock_); }
      void unlock() { pthread_mutex_unlock(&lock_); }
    };

    std::auto_ptr<impl> impl_;

  public:
    mtx2() : impl_(new impl) {}

    void lock()   { impl_->lock();   }
    void unlock() { impl_->unlock(); }
  };

  /** @test test mtx2 */
  void test_mtx2()
  {
    mtx2 m;
    m.lock();
    m.unlock();
  }

  static mtx2 mtx2a__;

  /** @test test mtx2a lock/unlock */
  void test_mtx2a()
  {
    mtx2a__.lock();
    mtx2a__.unlock();
  }
}

using namespace test_pt_mutex;

int main()
{
  csl_common_print_results(
    "test_init                   ",
    csl_common_test_timer_v0(test_init),"" );

  csl_common_print_results(
    "test_init2                  ",
    csl_common_test_timer_v0(test_init2),"" );

  csl_common_print_results(
    "test_lock_unlock            ",
    csl_common_test_timer_v0(test_lock_unlock),"" );

  csl_common_print_results(
    "test_lock_unlock2           ",
    csl_common_test_timer_v0(test_lock_unlock2),"" );

  csl_common_print_results(
    "test_lck_key_create         ",
    csl_common_test_timer_v0(test_lck_key_create),"" );

  csl_common_print_results(
    "test_lck_key_create2        ",
    csl_common_test_timer_v0(test_lck_key_create2),"" );

  csl_common_print_results(
    "test_mtx1                   ",
    csl_common_test_timer_v0(test_mtx1),"" );

  csl_common_print_results(
    "test_mtx1a                  ",
    csl_common_test_timer_v0(test_mtx1a),"" );

  csl_common_print_results(
    "test_mtx2                   ",
    csl_common_test_timer_v0(test_mtx2),"" );

  csl_common_print_results(
    "test_mtx2a                  ",
    csl_common_test_timer_v0(test_mtx2a),"" );

  return 0;
}

// -- EOF --
