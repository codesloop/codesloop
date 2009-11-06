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

#if 0
#ifndef DEBUG
#define DEBUG
#define DEBUG_ENABLE_INDENT
//#define DEBUG_VERBOSE
#endif /* DEBUG */
#endif //0

#include "codesloop/nthread/exc.hh"
#include "codesloop/nthread/thrpool.hh"
#include "codesloop/common/common.h"
#include "codesloop/common/str.hh"
#include "codesloop/common/logger.hh"
#include <assert.h>

/**
  @file thrpool.cc
  @brief implementation of thrpool
 */

namespace csl
{
  namespace nthread
  {
    thrpool::thrpool()
      : count_(0), min_(1), max_(4), timeout_(1000), attempts_(20), use_exc_(true), stop_me_(false), ev_(0), handler_(0)
    {
    }

    thrpool::~thrpool()
    {
      ENTER_FUNCTION();
      if( graceful_stop() == false )
      {
        CSL_DEBUGF( L"graceful stop has failed. wait 1 sec and do an unpolite_stop." );
        SleepSeconds(1);
        unpolite_stop();
      }
      LEAVE_FUNCTION();
    }

    namespace
    {
      class entry :  public thread::callback
      {
        public:
          virtual void operator()(void)
          {
            pool_->on_entry();
            unsigned int n_attempts = 0;

            while( stop_me() == false )
            {
              if( ev_->wait(timeout_) == false )
              {
                n_attempts++;

                /* no data so far, so we migh not be needed */
                if( n_attempts > attempts_ )
                {
                  /* ensure that the minimum amount of threads will run */
                  if( pool_->count() > min_ )
                  {
                    break;
                  }
                }
              }
              else if( stop_me() == true )
              {
                break;
              }
              else
              {
                /* clear unsuccessful attempts */
                n_attempts = 0;

                /* shall we start a brother to help ? */
                if( ev_->available_count() > 2 && pool_->count() < max_ && pool_->cleanup() < max_ )
                {
                  if( pool_->start_one() == false )
                  {
                      //XXX: todo, handle exc - THRNORET(nthread::exc::rs_start_error);
                  }
                }

                /* call data handler */
                (*handler_)();
              }
              pool_->cleanup();
            }
            pool_->on_exit();
          }

          virtual ~entry() { }

          inline entry(thrpool * pool, event * ev, thread::callback * handler)
          : pool_(pool), ev_(ev), handler_(handler), stop_me_(false)
          {
            timeout_  = pool->timeout();
            attempts_ = pool->attempts();
            min_      = pool->min();
            max_      = pool->max();
          }

          inline void stop_me(bool yesno)
          {
            ENTER_FUNCTION();
            scoped_mutex m(mtx_);
            CSL_DEBUGF( L"stop_me(%s)",(yesno==true?"TRUE":"FALSE") );
            stop_me_ = yesno;
            LEAVE_FUNCTION();
          }

          inline bool stop_me()
          {
            ENTER_FUNCTION();
            bool ret = false;
            {
              scoped_mutex m(mtx_);
              ret = stop_me_;
              CSL_DEBUGF( L"stop_me() => %s",(ret==true?"TRUE":"FALSE") );
            }
            RETURN_FUNCTION( ret );
          }

          inline bool use_exc() { return false; }

        private:
          thrpool *           pool_;
          event *             ev_;
          thread::callback *  handler_;
          bool                stop_me_;

          /* copy from pool at construction time */
          unsigned int        timeout_;
          unsigned int        attempts_;
          unsigned int        min_;
          unsigned int        max_;

          /* */
          mutex mtx_;

          entry() {}
          entry(const entry & other) {}
          entry & operator=(const entry & other) { return *this; }

          CSL_OBJ(csl::nthread::anonymous,entry);
      };
    }

    event & thrpool::exit_event()
    {
      return exit_event_;
    }

    event & thrpool::start_event()
    {
      return start_event_;
    }

    bool thrpool::start_one()
    {
      ENTER_FUNCTION();
      thread * t = 0;
      entry *  e = 0;

      {
        scoped_mutex m(mtx_);

        /* are there too many ? */
        if( count() > max() ) RETURN_FUNCTION( false );

        /* create new thread */
        t = new thread();
        e = new entry(this, ev_, handler_);
        thr_t th(t,e);

        /* launch thread */
        t->set_entry(*e);
        if( t->start() == false ) RETURN_FUNCTION( false );

        /* register new thread */
        threads_.push_back(th);
      }

      RETURN_FUNCTION( t->start_event().wait(10000) );
    }

    bool thrpool::init( unsigned int mint, unsigned int maxt,
                        unsigned int timeoutt, unsigned int attemptst,
                        event & ev, thread::callback & handler )
    {
      ENTER_FUNCTION();

      if( maxt < mint )             { THR(nthread::exc::rs_invalid_param, false); }
      if( mint < 1 || maxt > 2000 ) { THR(nthread::exc::rs_invalid_param, false); }
      if( attemptst == 0 )          { THR(nthread::exc::rs_invalid_param, false); }
      if( timeoutt == 0 )           { THR(nthread::exc::rs_invalid_param, false); }

      if( count() > 0 )
      {
        if( graceful_stop() == false )
        {
          if( unpolite_stop() == false )
          {
            THR(nthread::exc::rs_stop_error, false);
          }
        }
      }

      {
        scoped_mutex m(mtx_);

        min_         = mint;
        max_         = maxt;
        timeout_     = timeoutt;
        attempts_    = attemptst;
        ev_          = &ev;
        handler_     = &handler;
      }

      for( unsigned int i=0;i<mint;++i )
      {
        if( start_one() == false )
        {
          THR(nthread::exc::rs_start_error, false);
        }
      }

      RETURN_FUNCTION( true );
    }

    unsigned int thrpool::cleanup()
    {
      ENTER_FUNCTION();
      unsigned int ret = 0;

      std::list<thread *> thrs;
      std::list<thread::callback *> cbs;

      {
        scoped_mutex m(mtx_);

        thrlist_t newlist;

        for( thrlist_t::iterator it = threads_.begin() ;it!=threads_.end();++it )
        {
          if( (*it).first && (*it).first->exit_event().is_permanent() == false )
          {
            newlist.push_back( *it );
          }
          else
          {
            thrs.push_back( (*it).first );
            cbs.push_back( (*it).second );
          }
        }

        ret = newlist.size();
        threads_.swap( newlist );
      }

      for( std::list<thread *>::iterator it=thrs.begin();it!=thrs.end();++it )
      {
        delete *it;
      }

      for( std::list<thread::callback *>::iterator it=cbs.begin();it!=cbs.end();++it )
      {
        delete *it;
      }

      RETURN_FUNCTION( ret );
    }

    bool thrpool::graceful_stop()
    {
      ENTER_FUNCTION();
      bool ret = false;

      unsigned int i = 0;
      {
        scoped_mutex m(mtx_);
        thrlist_t::iterator it = threads_.begin();
        for( ;it!=threads_.end();++it )
        {
          CSL_DEBUGF( L"set stop_me flag on thread: %p [%d]",((*it).second),i );
          (dynamic_cast<entry *>((*it).second))->stop_me(true);
          ++i;
        }
      }

      if( i > 0 )
      {
        CSL_DEBUGF( L"send %d notification to the worker threads",i );
        ev_->notify( i );
      }

      unsigned int failed_count = 0;

      for( unsigned int j=0; j<i; ++j )
      {
        CSL_DEBUGF( L"wait thread %d/%d available:%d waiting:%d",
                     j,
                     i,
                     exit_event_.available_count(),
                     exit_event_.waiting_count() );

        if( exit_event_.wait(timeout_*3) == false )
        {
          ++failed_count;
        }
      }

      cleanup();

      if( count() > 0 && failed_count > 0 )
      {
        CSL_DEBUGF( L"do a second pass cleanup.... [failed:%d]",failed_count );
        ev_->notify( failed_count );

        for( unsigned int j=0; j<failed_count; ++j )
        {
          CSL_DEBUGF( L"wait thread %d/%d available:%d waiting:%d",
                       j,
                       failed_count,
                       exit_event_.available_count(),
                       exit_event_.waiting_count() );

          exit_event_.wait(timeout_*5);
        }
      }

      ret = (count() == 0);
      exit_event_.clear_available();

      RETURN_FUNCTION( ret );
    }

    bool thrpool::unpolite_stop()
    {
      ENTER_FUNCTION();
      bool ret = true;
      {
        scoped_mutex m(mtx_);
        thrlist_t::iterator it = threads_.begin();
        for( ;it!=threads_.end();++it )
        {
          CSL_DEBUGF( L"stoping thread: %p",((*it).second) );
          (*it).first->stop();
          if( (*it).first->exit_event().wait(timeout_) == false ) ret = false;
          delete (*it).first;
          delete (*it).second;
        }
        threads_.clear();
      }
      RETURN_FUNCTION( ret );
    }

    void thrpool::on_entry()
    {
      ENTER_FUNCTION();
      {
        scoped_mutex m(mtx_);
        ++count_;
      }
      start_event().notify();
      LEAVE_FUNCTION();
    }

    void thrpool::on_exit()
    {
      ENTER_FUNCTION();
      {
        scoped_mutex m(mtx_);
        --count_;
      }
      exit_event().notify();
      LEAVE_FUNCTION();
    }

    unsigned int thrpool::count()
    {
      ENTER_FUNCTION();
      unsigned int ret = 0;
      {
        scoped_mutex m(mtx_);
        ret = count_;
      }
      CSL_DEBUGF( L"count() => %d",ret );
      RETURN_FUNCTION( ret );
    }

    unsigned int thrpool::min()
    {
      ENTER_FUNCTION();
      unsigned int ret = 0;
      {
        scoped_mutex m(mtx_);
        ret = min_;
      }
      CSL_DEBUGF( L"min() => %d",ret );
      RETURN_FUNCTION( ret );
    }

    unsigned int thrpool::max()
    {
      ENTER_FUNCTION();
      unsigned int ret = 0;
      {
        scoped_mutex m(mtx_);
        ret = max_;
      }
      CSL_DEBUGF( L"max() => %d",ret );
      RETURN_FUNCTION( ret );
    }

    unsigned int thrpool::timeout()
    {
      ENTER_FUNCTION();
      unsigned int ret = 0;
      {
        scoped_mutex m(mtx_);
        ret = timeout_;
      }
      CSL_DEBUGF( L"timeout() => %d",ret );
      RETURN_FUNCTION( ret );
    }

    unsigned int thrpool::attempts()
    {
      ENTER_FUNCTION();
      unsigned int ret = 0;
      {
        scoped_mutex m(mtx_);
        ret = attempts_;
      }
      CSL_DEBUGF( L"attempts() => %d",ret );
      RETURN_FUNCTION( ret );
    }

    bool thrpool::use_exc()
    {
      bool ret = false;
      {
        scoped_mutex m(mtx_);
        ret = use_exc_;
      }
      return ret;
    }

    void thrpool::use_exc(bool yesno)
    {
      scoped_mutex m(mtx_);
      use_exc_ = yesno;
    }
  }
}

/* EOF */
