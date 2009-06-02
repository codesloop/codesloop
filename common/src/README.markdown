common module sources
=====================

common uses the csl::common namespace and includes the following classes:

* [circbuf.hh](./circbuf.hh) : circular buffer class
* [pvlist.hh](./pvlist.hh) : pvlist is a template class container for pointers, the D template parameter may be used to tell the object how to destruct the contained pointers. there are 3 destructors supplied (nop, delete and free) but others may be supplied if needed
* [mpool.hh](./mpool.hh) : memory pooling based on pvlist. memory allocation is done with malloc() and the pointers are free()'d on destruct
* [pbuf.hh](./pbuf.hh) : pbuf is a paged buffer implementation. memory is stored in fixed length chunks.
* [tbuf.hh](./tbuf.hh) : templated memory buffer, where the template argument tells how much memory should be statically preallocated. this strategy provides huge performance gains in certain cases
* [test_timer.hh](./test_timer.hh) : performance measurement helpers
* [xdrbuf.hh](./xdrbuf.hh) : XDR encoding and decoding helper based on pbuf
* [zfile.hh](./zfile.hh) : zlib compressed file helper

Generic notes
-------------

Most classes here are created for specific usage scenario. They are not intended to solve abstract problems. The test files in the module's directory is the best source of information. We run and improve these tests regularly in order to see if modifications will not break the interface and the semantics behind them.


Circular buffer: circbuf
------------------------

This class is first used in the comm module to store incoming UDP packets. The design goals for this simple class are:

* the memory of the items used in the list is managed by the circbuf instance (rather then the caller)
* there should be an upper bound for the incoming UDP packets to be stored
* event handlers are called when new item arrived, item popped form cicrcbuf, the circbuf become empty or full
* the event handlers are virtual functions so to use them one must subclass circbuf
* the class itself is not thread safe (because of circular dependencies between modules), but it can be made thread safe easily by subclassing

Basic usage of circbuf
----------------------

    // create circular buffer of at most 40 integers
    circbuf<int,40> cb;
    
    // push an integer
    int & i = cb.push()
    i = 1234;
    
    // pop an integer
    int & it = cb.pop();

This at first site makes little sense for an integer, but if larger buffers are allocated then it saves a memory allocation and copy.

    // memory buffer and arrival timestamp
    struct buffer
    {
      char buff[65536];
      time_t arrived_at;
      unsigned short size;
    };
    
    // now use a circular buffer for this beast
    circbuf<buffer,50> cbb;
    
    // receive packets
    for( ;; )
    {
      buffer & bf(cbb.push());
      bf.size = receive(bf.buff,sizeof(bf.buff));
    }
    
    // the new item can be read this way
    buffer & bx = cbb.pop();


In a real world scenario this is still useless. The real usecase is a multithreaded server where one thread pushes items into the circular buffer and an other pops them. This is especially useful when message storms are expected and the server may need to handle more messages then it is able to process. For this scenario a threadpool may launch more worker threads to help processing the messages. A simple multithreaded scenario follows here. (For a more complex one see udp::recvr)

Multithreaded usage of circbuf
------------------------------

First lets create a subclass of circbuf that will send event notifications to handler threads using nthread::event.

    class buffers : public common::circbuf<buffer,30>
    {
    public:
      virtual void on_new_item() { ev_.notify(); }
      virtual ~buffers() { }
      
      mutex   mtx_;
      event   ev_;
    };

The ev_ variable will help handlers to determine when new message arrives and the mtx_ mutex will guard internal variables. The event variable works in concert with nthread::thrpool that is able to manage thread pools. In order to utilize it we have to create an event handler callback:

    class msg_handler : public thread::callback
    {
    private:
      buffers * bfrs_;
      
    public:
       msg_handler(buffers & bfrs) : bfrs_(&bfrs) {}
       
       virtual void operator()(void)
       {
          buffer b;
          
          {
            scoped_mutex mm(bfrs_->mtx_);
            
            /* pop message from the circular buffer */
            buffer & t(bfrs_->pop());
            
            /* copy message so we don't block others */
            memcpy( &b,&t,sizeof(t) );
          }
          
          /* process b buffer here after unlock() */
          
       }
    };

Now setup thread pool and the circular buffer.

    msg_handler   handler;
    thrpool       thread_pool;
    buffers       bfrs;
    
    /* initialize thread pool */
    
    thread_pool.init( MIN_THREADS, MAX_THREADS, TIMEOUT, RETRIES, bfrs.ev_, handler );
    
    /* two step push messages into buffers */
    
    buffer * tmp = 0;
    {
      scoped_mutex mm(bfrs.mtx_);
      tmp = &(bfrs_.prepare());
    }
    
    /* try to receive data */
    
    if( receive_data( tmp ) )
    {
      /* successfully received data */
      scoped_mutex mm(bfrs.mtx_);
      bfrs_.commit( *tmp );
    }
    else
    {
      scoped_mutex mm(bfrs.mtx_);
      bfrs_.rollback( *tmp );
    }

The catch here that effects the design of circbuf is that I don't want the circular buffer to be locked while waiting for new messages. For this reason the push() may be devided to prepare()/commit()/rollback() steps. The prepare() call reserves space for the new message and the commit() call pushes that into the circular buffer. The rollback() call undos the prepare() step, thus putting the buffer to the free-list of the circular buffer.


