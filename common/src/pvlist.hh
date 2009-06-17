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

#ifndef _csl_common_pvlist_hh_included_
#define _csl_common_pvlist_hh_included_

/**
   @file pvlist.hh
   @brief Very simple pointer store (a list of pointer vectors/arrays)

   pvlist is intended for uses when many small memory allocation is done
   and they are not deallocated up to the end of the task. Then these memory
   blocks could be deallocated together. The memory blocks would not be
   iterated over. Free function is provided but not recommended, because it is
   slow.

   pvlist is a template class that could use different template policies for
   freeing its data. 3 are provided: nop_destructor that does nothing,
   free_destructor and delete_destructor. 

   the design goal of pvlist is to be fast for the outlined specific case, thus:

   1) should have minimal overhead when instantiated
   2) should have minimal overhead when adding new pointers
   3) should have minimal overhead when destroyed

   on the downside pvlist has minimal functionality, thus:

   1) fast lookup of pointers is not possible, because that involves list traversal
   2) changing pointers can be slow for large pointer collections
*/

#include <stdlib.h>
#include <stdio.h>
#ifdef __cplusplus

namespace csl
{
  namespace common
  {

    /**
     @brief policy for pvlist template

     This policy is handful when pvlist stores pointers that should not be
     freed, like pointers to static data or pointers that are owned by other
     instance.
     */
    template <typename T>
    struct nop_destructor
    {
      inline void destruct(T ** p, size_t n) { }
      inline void destruct(T * p) { }
    };

    /**
     @brief policy for pvlist template

     This policy is handful when pvlist stores pointers that are malloced.
     */
    template <typename T>
    struct free_destructor
    {
      inline void destruct(T * p)
      {
        if( p ) ::free( p );
      }

      inline void destruct(T ** p, size_t n) 
      {
        for( size_t i=0;i<n;++i )
          if( p[i] ) 
            ::free( p[i] ); 
      }
    };

    /**
     @brief policy for pvlist template

     This policy is handful when pvlist stores pointers that are allocated
     by new.
     */
    template <typename T>
    struct delete_destructor
    {
      inline void destruct(T * p)
      {
        if( p ) delete p;
      }

      inline void destruct(T ** p, size_t n) 
      {
        for( size_t i=0;i<n;++i )
          if( p[i] ) 
            delete p[i]; 
      }
    };

    /**
     @brief template class to collect many allocations that should be freed together

     The design of this class is driven by the need of a very simple pool class
     that can collect a few dynamically allocated pointers and free them together.

     The overhead of this object should be minimal for a few cases, while others
     possible uses are not optimized. 

     It should be fast to declare this object alone, so if not used its overhead
     should be minimal. Also if only a few pointers are stored, it should be 
     pretty fast too. The I parameter can be used to tune how many pointers
     should be statically allocated. They are allocated on the stack rather
     then on the heap.

     To summarize these benefits:

        1) should have minimal overhead when instantiated
        2) should have minimal overhead when adding new pointers
        3) should have minimal overhead when destroyed

     On the downside:

        It is possible to get or set pointers in pvlist but its performance is 
        slow for large collections. O(n/I)

     The T parameter is used for setting what type of pointer will be stored.
     If T is set to type_t then pvlist receives type_t*-s.

     */
    template <
      size_t   I=64,
      typename T=void,
      typename D=nop_destructor<T> >
    class pvlist
    {
    public:
      enum { block_size = I };
      typedef T item_type_t;

      /** @brief returns the blocksize=I that was statically declared */
      inline size_t bs() { return block_size; }

      struct iterator;
      friend struct iterator;

    private:
      struct item
      {
        T *       ptrs_[block_size];
        size_t    used_;
        item *    next_;

        inline item( ) : used_(0), next_(0) { }
      };

      item   head_;
      item * tail_;
      size_t n_items_;

      inline item * ensure_item()
      {
        if( tail_->used_ == block_size )
        {
          item * old_tail = tail_;
          tail_           = new item();
          old_tail->next_ = tail_;
        }
        return tail_;
      }

    public:
      /**
      @brief pvlist iterator

      very simple, forward only iterator
      */
      class iterator
      {
      private:
        item * i_;
        size_t pos_;

        iterator() {}
      public:
        ~iterator() {}

        /** @brief initializer constructor */
        iterator(item * i, size_t pos) : i_(i), pos_(pos) {}

        /** @brief copy constructor */
        iterator(const iterator & other) : i_(other.i_), pos_(other.pos_) {}

        /** @brief copy operator */
        iterator & operator=(const iterator & other)
        {
          i_   = other.i_;
          pos_ = other.pos_;
          return *this;
        }

        /** @brief creates an iterator of ls
            @param ls is the pvlist to be iterated over */
        iterator(pvlist & ls) : i_(&(ls.head_)), pos_(0)
        {
          {
            if(!ls.n_items_) i_ = 0;
          }
        }

        /** @brief checks equality */
        bool operator==(const iterator & other)
        {
          return ((i_ == other.i_ && pos_ == other.pos_) ? true : false );
        }

        /** @brief checks if not equal */
        bool operator!=(const iterator & other)
        {
          return (!(operator==(other)));
        }

        /** @brief step forward */
        void operator++()
        {
          if( i_ == 0 )
          {
            i_   = 0;
            pos_ = 0;
          }
          else if( pos_ == (i_->used_-1) )
          {
            i_   = i_->next_;
            pos_ = 0;
          }
          else
          {
            ++pos_;
          }
        }

        /** @brief sets the iterator to end */
        void zero() { i_ = 0; pos_ = 0; }

        /** @brief returns the pointed item */
        item_type_t * operator*()
        {
          if( i_ ) { return i_->ptrs_[pos_]; }
          else     { return 0;               }
        }
      };


      /**
      @brief pvlist const iterator

      very simple, forward only constant iterator
       */
      class const_iterator
      {
        private:
          const item * i_;
          size_t pos_;

          const_iterator() {}
        public:
          ~const_iterator() {}

          /** @brief initializer constructor */
          const_iterator(const item * i, size_t pos) : i_(i), pos_(pos) {}

          /** @brief copy constructor */
          const_iterator(const const_iterator & other) : i_(other.i_), pos_(other.pos_) {}

          /** @brief copy operator */
          const_iterator & operator=(const const_iterator & other)
          {
            i_   = other.i_;
            pos_ = other.pos_;
            return *this;
          }

          /** @brief creates an iterator of ls
              @param ls is the pvlist to be iterated over */
          const_iterator(const pvlist & ls) : i_(&(ls.head_)), pos_(0)
          {
            if(!ls.n_items_) i_ = 0;
          }

          /** @brief checks equality */
          bool operator==(const const_iterator & other) const
          {
            return ((i_ == other.i_ && pos_ == other.pos_) ? true : false );
          }

          /** @brief checks if not equal */
          bool operator!=(const const_iterator & other) const
          {
            return (!(operator==(other)));
          }

          /** @brief step forward */
          void operator++()
          {
            if( i_ == 0 )
            {
              i_   = 0;
              pos_ = 0;
            }
            else if( pos_ == (i_->used_-1) )
            {
              i_   = i_->next_;
              pos_ = 0;
            }
            else
            {
              ++pos_;
            }
          }

          /** @brief sets the iterator to end */
          void zero() { i_ = 0; pos_ = 0; }

          /** @brief returns the pointed item */
          const item_type_t * operator*() const
          {
            if( i_ ) { return i_->ptrs_[pos_]; }
            else     { return 0;               }
          }
      };

      /** @brief returns iterator pointed at the beginning of the container */
      iterator begin()
      {
        iterator ret(*this);
        return ret;
      }

      /** @brief returns iterator represents the end of this container */
      iterator end()
      {
        iterator ret(*this);
        ret.zero();
        return ret;
      }

      /** @brief returns iterator represents the last item in this container */
      iterator last()
      {
        if( !n_items_ || !tail_ || !(tail_->used_) )
        {
          return end();
        }
        else
        {
          iterator ret(tail_,tail_->used_-1);
          return ret;
        }
      }

      /** @brief returns a constant iterator pointed at the beginning of the container */
      const_iterator const_begin() const
      {
        const_iterator ret(*this);
        return ret;
      }

      /** @brief returns a constant iterator pointed at the beginning of the container */
      const_iterator begin() const { return const_begin(); }

      /** @brief returns a constant iterator represents the end of this container */
      const_iterator const_end() const
      {
        const_iterator ret(*this);
        ret.zero();
        return ret;
      }

      /** @brief returns a constant iterator represents the end of this container */
      const_iterator end() const { return const_end(); }

      /** @brief returns a constant iterator represents the last item in this container */
      const_iterator const_last() const
      {
        if( !n_items_ || !tail_ || !(tail_->used_) )
        {
          return end();
        }
        else
        {
          const_iterator ret(tail_,tail_->used_-1);
          return ret;
        }
      }

      /** @brief returns a constant iterator represents the last item in this container */
      const_iterator last() const { return const_last(); }

    public:
      /** @brief constructor */
      inline pvlist() : tail_(&head_), n_items_(0) { }

      /** @brief destructor */
      inline ~pvlist() 
      {
        free_all();
      }

      /** 
       @brief number of items stored 
       @return the number of items
       */
      inline size_t n_items() const { return n_items_; }

      /** 
      @brief number of items stored 
      @return the number of items

      same as n_items()
       */
      inline size_t size() const { return n_items_; }

      /** @brief call the destruct() function on each items and frees internal memory */
      inline void free_all()
      {
        D d;
        d.destruct( head_.ptrs_, head_.used_ );
        item * i = head_.next_;
        while( i )
        {
          item * o = i;
          i = i->next_;
          d.destruct( o->ptrs_, o->used_ );
          delete o;
        }
        head_.used_ = 0;
        head_.next_ = 0;
        tail_ = &head_;
        n_items_ = 0;
      }

      /** 
       @brief search for a pointer in pvlist
       @param p the pointer to be searched
       */
      inline bool find(T * p)
      {
        item * i = &head_;

        while( i )
        {
          for( size_t ii=0;ii<(i->used_); ++ii )
          {
            if( i->ptrs_[ii] == p )
            {
              return true;
            }
          }
          i = i->next_;
        }
        return false;
      }

      /** 
       @brief free()-s p and set all p's occurence to NULL
       @param p the pointer to be destructed
       @return true if pointer found and freed

       this function iterates through the whole collection, the first occurence of p
       will be destructed() and if more found then they will be set to NULL

       this function does not touch the number of items, it rather replaces
       the destructed pointers with NULL
       */
      inline bool free(T * p)
      {
        bool freed=false;
        D d;
        item * i = &head_;

        while( i )
        {
          for( size_t ii=0;ii<(i->used_); ++ii )
          {
            if( i->ptrs_[ii] == p )
            {
              if( !freed )
              {
                d.destruct( i->ptrs_[ii] );
                freed = true;
              }
              i->ptrs_[ii] = 0;
            }
          }
          i = i->next_;
        }
        return freed;
      }

      /** 
       @brief free()-s p first occurence and returns
       @param p the pointer to be destructed
       @return true if pointer found and freed

       this function iterates through the collection untill p was found, then
       it destructs that, sets that occurence to NULL  and returns immediately

       if more then one occuernce of p exists in the collection they wont be
       touched so they may lead to memory corruption problems. care must be
       taken....

       this function does not touch the number of items, it rather replaces
       the destructed pointer with NULL
       */
      inline bool free_one(T * p)
      {
        D d;
        item * i = &head_;

        while( i )
        {
          for( size_t ii=0;ii<(i->used_); ++ii )
          {
            if( i->ptrs_[ii] == p )
            {
              d.destruct( i->ptrs_[ii] );
              i->ptrs_[ii] = 0;
              return true;
            }
          }    
          i = i->next_;
        }
        return false;
      }

      /** 
       @brief adds an item to the end of the list
       @param p the pointer to be appended

       the number of stored items will be increased by 1 after 
       calling this function
       */
      inline T * push_back(T * p)
      {
        item * i = ensure_item();
        i->ptrs_[i->used_] = p;
        ++(i->used_);
        ++n_items_;
        return p;
      }

      /**
       @brief gets the item at the specified position
       @param which is the desired position
       @return T * which is the item at that position or NULL of the position is invalid
       */
      inline T * get_at(size_t which) const
      {
        if( which >= n_items_ ) return 0;
        
        size_t pn = which/block_size;
        
        /* first page go fast */
        if( !pn  ) 
          return (which < head_.used_ ? head_.ptrs_[which] : 0);
        
        /* iterate through pages */
        const item * pt = &head_;
        while( pt->next_ && pn > 0 )
        {
          --pn; 
          pt = pt->next_; 
        }
        
        /* not enough pages */
        if( pn ) 
          return 0;
        
        size_t rm = which%block_size;
        
        return (rm < pt->used_ ? pt->ptrs_[rm] : 0);
      }
      
      /**
       @brief sets the item at the specified position
       @param which is the desired position
       @param ptr is the pointer to be set
       @return true if successful, false if the position is invalid
       */
      inline bool set_at(size_t which, T * ptr)
      {
        if( which >= n_items_ ) return false;
        
        size_t pn = which/block_size;
        
        /* first page go fast */
        if( !pn  ) 
        {
          if( which >= head_.used_ ) return false;
          head_.ptrs_[which] = ptr;
          return true;
        }
        
        /* iterate through pages */
        item * pt = &head_;
        while( pt->next_ && pn > 0 )
        {
          --pn; 
          pt = pt->next_; 
        }
        
        /* not enough pages */
        if( pn ) 
          return false;
        
        size_t rm = which%block_size;
        
        if( rm < pt->used_ )
        {
          pt->ptrs_[rm] = ptr;
          return true;
        }
        else
        {
          return false;
        }
      }
      
      /** @brief prints some debug information to STDOUT */
      inline void debug()
      {
        printf("== pvlist::debug ==\n");
        item * pt = &head_;
        while( pt )
        {
          printf(
            "  item:\n"
            "  -- ptrs : %p\n"
            "  -- used : %ld\n"
            "  -- next : %p\n"
            "  ---- : ",
                 pt->ptrs_,
                 (unsigned long)pt->used_,
                 pt->next_ );
          for( size_t i=0;i<pt->used_;++i ) printf("%p ",pt->ptrs_[i]);
          printf("\n");
          pt = pt->next_;
        }
      }
    };
  }
}

#endif /* __cplusplus */
#endif /* _csl_common_pvlist_hh_included_ */
