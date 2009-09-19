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

#ifndef _csl_nthread_mutex_hh_included_
#define _csl_nthread_mutex_hh_included_

/**
   @file mutex.hh
   @brief pthread mutex class with functional improvements

   Implementation is based on pthread (posix threads)
*/

#include "codesloop/common/common.h"
#include "codesloop/common/obj.hh"

#ifdef __cplusplus
#include <memory>

namespace csl
{
  namespace nthread
  {
    /**
       @brief works like pthread mutexes with additional features

       the additional features include:

       @li the ability to query the number of waiting threads
       @li the ability to recursively lock and unlock the mutex
       @li if a thread locks the mutex than it unlocks that when the thread exits
       @li the ability to tell wether the mutex is locked without actually locking it

       the design decision here is not to expose any pthread details, so
       the underlying implementation may change if needed. this is also true
       for the underlying pthread concepts. this is intended to be a higher
       level abstraction, than what pthread provides.
      */
    class mutex
    {
    public:
      /** @brief constructor */
      mutex();

      /** @brief destructor */
      ~mutex();

      /**
         @brief locks the mutex
         @param timeout_ms is the timeout in milliseconds
         @return true if succeed and false if timeout or other error happened

         apart from initializing the pthread structures it also registers a cleanup function
         that unlocks the thread when it exits, this helps to be on the safe side regarding some
         pthread issues

         if the current thread locked the mutex than it succeeds and increases the locked_count
        */
      bool lock(unsigned long timeout_ms=0);


      /**
         @brief tries locking the mutex
         @return true if succeed and false if error happened

         this function does not block, both indicates wether it could lock the mutex or not

         it registers the emrgency unlock function just like mutex::lock()
         */
      bool try_lock();

      /**
         @brief unlocks the mutex
         @return true if succeed and false if error happened

         errors may include:

         @li mutex is not locked
         @li mutex is not locked by this thread

         if succeeds and locked count goes down to zero than it unregisters the
         emergency unlock function
         */
      bool unlock();

      /**
          @brief recursively unlocks the mutex

          works like unlock(), but it zeros the locked count and unregisters the
          emergency unlock function immediately (rather than decreasing by one)
         */
      bool recursive_unlock();

      /** @brief tells wether a mutex is locked */
      bool is_locked();

      /** @brief access the number of waiting threds */
      unsigned int waiting_count();

      /** @brief access the number of locks held */
      unsigned int locked_count();

    private:
      struct impl;
      std::auto_ptr<impl> impl_;

      // no-copy
      mutex(const mutex & other);
      mutex & operator=(const mutex & other);

      CSL_OBJ(csl::nthread, mutex);
      USE_EXC();
    };

    /** @brief scoped mutex template */
    template <typename T=mutex>
    class scoped_mutex_template
    {
    public:
      /** @brief constructor */
      scoped_mutex_template(T & mtx) : mtx_(&mtx)
      {
        mtx_->lock();
      }

      /** @brief destructor */
      ~scoped_mutex_template()
      {
        mtx_->unlock();
      }

    private:
      T * mtx_;

      // no default construction and copying
      scoped_mutex_template() : mtx_(0) {}
      scoped_mutex_template(const scoped_mutex_template &) : mtx_(0) {}
      scoped_mutex_template & operator=(const scoped_mutex_template &) { return *this; }

      CSL_OBJ(csl::nthread, scoped_mutex_template);
    };

    /** @brief scoped mutex */
    typedef scoped_mutex_template<mutex> scoped_mutex;

    /** @brief on-demand mutex
        @li only allocates the mutex structure when it is used
        @li to be used when the instantiation of the object should be fast */
    class ondemand_mutex
    {
    public:
      ondemand_mutex() : mtx_(0) {}
      ~ondemand_mutex() { if( mtx_ ) { delete mtx_; }; mtx_ = 0; }

      /** @brief same as mutex::lock() except it initializes the internal mutex on demand
          @see mutex::lock() */
      bool lock(unsigned long timeout_ms=0) { init(); return mtx_->lock(timeout_ms); }

      /** @brief same as mutex::try_lock() except it initializes the internal mutex on demand
          @see mutex::try_lock() */
      bool try_lock() { init(); return mtx_->try_lock(); }

      /** @brief same as mutex::unlock() except it initializes the internal mutex on demand
          @see mutex::unlock() */
      bool unlock() { init(); return mtx_->unlock(); }

      /** @brief same as mutex::recursive_unlock() except it initializes the internal mutex on demand
          @see mutex::recursive_unlock() */
      bool recursive_unlock() { init(); return mtx_->recursive_unlock(); }

      /** @brief same as mutex::is_locked() except it initializes the internal mutex on demand
          @see mutex::is_locked() */
      bool is_locked() { init(); return mtx_->is_locked(); }

      /** @brief same as mutex::waiting_count() except it initializes the internal mutex on demand
          @see mutex::waiting_count() */
      unsigned int waiting_count() { init(); return mtx_->waiting_count(); }

      /** @brief same as mutex::locked_count() except it initializes the internal mutex on demand
          @see mutex::locked_count() */
      unsigned int locked_count() { init(); return mtx_->locked_count(); }

    private:
      void init() { if( !mtx_ ) mtx_ = new mutex(); }

      mutex * mtx_;

      // no copying
      ondemand_mutex(const ondemand_mutex &) : mtx_(0) {}
      ondemand_mutex & operator=(const ondemand_mutex &) { return *this; }

      CSL_OBJ(csl::nthread, scoped_mutex_template);
    };

    /** @brief scoped mutex w/ ondemand_mutex */
    typedef scoped_mutex_template<ondemand_mutex> scoped_ondemand_mutex;
  }
}

#endif /* __cplusplus */
#endif /* _csl_nthread_mutex_hh_included_ */

/* EOF */
