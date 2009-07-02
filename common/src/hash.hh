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

#ifndef _csl_common_hash_hh_included_
#define _csl_common_hash_hh_included_

/**
   @file hash.hh
   @brief generic hash table class
 */

#include "obj.hh"
#include "pvlist.hh"
#include "tbuf.hh"
#include "common.h"
#ifdef __cplusplus

namespace csl
{
  namespace common
  {
    unsigned long long default_hash_function(unsigned long long id, unsigned char nbits);

    unsigned long long hash_code(unsigned long long item);

    template <typename Obj>
    unsigned long long hash_code(const Obj & o)
    {
      return o.hash_code();
    }

    template < typename Obj,
               typename IndexHandler,
               typename PageHandler
             >
    class hash : public obj
    {
      public:
        typedef Obj            obj_t;
        typedef IndexHandler   index_handler_t;
        typedef PageHandler    page_handler_t;

        struct entry
        {
          unsigned long long next_item_;
          unsigned long long page_id_;

          inline void reset()
          {
            next_item_    = 0;
            page_id_      = 0;
          }
        };

        struct index
        {
          unsigned char        max_bits_;
          unsigned char        item_width_;
          unsigned long long   n_items_;
          unsigned long long   allocated_;
          unsigned char *      buffer_;
          index_handler_t &    index_handler_;

          index(index_handler_t & h)
            : max_bits_(1), item_width_(2), n_items_(0),
              allocated_(0), buffer_(0), index_handler_(h)
          {
            index_handler_.consruct(*this);
          }

          ~index()
          {
            index_handler_.destruct(*this);
          }
        };

      private:
        index_handler_t  index_handler_;
        page_handler_t   page_handler_;
        index            index_;

      public:
        /* consructor */
        hash() : index_(index_handler_), obj() { use_exc(true); }

        /* forwarders */

        /* call global hash_code function. if that is not suitable define one yourself */
        bool get(Obj & obj)        { return get( hash_code(obj) ,obj ); }
        bool set(const Obj & obj)  { return set( hash_code(obj) ,obj ); }

        bool get(unsigned long long id, Obj & obj)
        {
          entry e;
          unsigned long long entry_id;

          /* index handler is reponsible for looking up the entry associated
             with the given id. if that failed then no entry found for the given
             id. this indicates a serious error. every lookup must succeed, but
             e.page_id_ may be nil if no entry is there. */

          if( !index_handler_.get_entry(index_,id,e,entry_id) )
          {
            /* throw an exception telling the caller that this is a serious problem */
            THR(exc::rs_lookup_error,exc::cm_hash,false);
          }

          /* no page found for the given id, means there is no page and thus no data */
          if( e.page_id_ == 0 ) { return false; }

          /* sanity check: the returned entry must be a leaf entry (should be zero) */
          if( e.next_item_ != 0 ) { THR(exc::rs_invalid_state,exc::cm_hash,false); }

          /* lookup and return item */
          return page_handler_.get(e.page_id_,id,obj);
        }

        bool set(unsigned long long id, const Obj & obj)
        {
          entry e;
          unsigned long long entry_id;

          /* if set_entry returns false than it indicates a lookup error which
             should not happen. every lookup must succeed. this function is expected
             to return the given entry and an identifier that is used by the index_handler_
             to identify the given entry */

          if( !index_handler_.get_entry(index_,id,e,entry_id) )
          {
            /* this must be a serious problem, indicating an internal lookup error */
            THR(exc::rs_lookup_error,exc::cm_hash,false);
          }

          /* if the page_id_ in the entry is nil than the page_handler_ is expected
             to allocate a page and return the id in e.page_id_. if that is the
             case, we must tell the index handler later to update the index entry */

          bool need_index_update = false;

          if( e.page_id_ == 0 ) { need_index_update = true; }

          /* check if page handler can insert obj:
             - page handler is expected to return true if succeed and
             - false if the page is full */

          if( !page_handler_.insert(id,obj,e) )
          {
            /* the page is full so we need to split the entry.
               here we expect that e will be updated
            */

            if( !index_handler_.split(index_,entry_id,id,e) )
            {
              /// TODO fix this design issue
            }
          }
          else
          {
            /* page_handler_ could insert the object */
          }
        }
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_common_hash_hh_included_ */
