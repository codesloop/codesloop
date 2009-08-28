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

Most classes here are created for a specific usage scenario. They are not intended to solve abstract problems. The test files in the module's directory is the best source of information. We run and improve these tests regularly in order to see if modifications will not break the interface and the semantics behind them.

classes in the common module are not thread safe. The reason for that threading primitives are in the nthread module which has dependencies for the common module. If threading dependencies would be introduced here then circular dependencies would have been created. This is to be avoided by design. The easier would be to let users lock() common classes themselves.

List of pointer vectors: pvlist
-------------------------------

This class is used for storing pointers for objects. The purpose of this class is to cheaply store those pointers as a registry and when not they are not needed free all of them at one shot. This is useful when many small objects are created and they may be kept up to the work needed. I checked boost pools and a handmade pool based on std::list and they both waste too much time during both construction and usage time.

a simple pvlist usage can be:

    pvlist< 32,char,free_destructor<char> > pv;
    pv.push_back( strdup("apple") );
    pv.push_back( strdup("pine") );
    pv.push_back( strdup("peach") );

pv now contains tree pointers to strings which will be automatically freed during destruction of pv. A simple iterator is also defined, so they may be iteratod over like this:

    pvlist< 32,char,free_destructor<char> >::iterator it = pv.begin();
    
    for( ;it!=pv.end();++it )
    {
      printf("Item: %s\n",*it);
    }

typical container like facilities like size() and push_back() are provided.

There are other functions that are provided for completeness but they have O(n) speed and thus care must be taken when to use them. Examples of they are: set_at(), free_one() and free().

Generic memory pool: mpool
--------------------------

Mpool is a wrapper over pvlist for one specific case when void pointers are to be stored and they are all malloced. A typical usage is:

    mpool<>  pool;
    void * data = pool.allocate(10);

data now contains 10 bytes of malloced memory. it will be automatically freed when pool is destroyed.

Paged buffer: pbuf
------------------

pbuf is created to represent paged memory regions. A typical usage is when reading a stream of data in 2k blocks and we cannot tell the exact amount we will receive. For this case pbuf provides 2k data buffers and provide iterator to read the data stored. pbuf is based on pvlist to store the data chunks. Data chunks are freed when destructed or instructed to do so.

A typical usage scenario is:

    pbuf pb;
    pb.append("Hello ");
    pb.append("World!");
    
    pbuf pb2;
    pb2.append((unsigned char *)"Hello world!",13);

The append call in case of pb checks wether there is space allocate in the last buffer. If there is space then it will not allocate any new buffer, but puts the new data into that.

Iterators over pbuf will iterate over pbuf chunks not bytes. So users must take that into account. An iterator usage scenario would be:

    pbuf::iterator it(pb.begin());
    for( ;it!=pb.end();++it )
    {
      printf( "Size: %d Free space: %d Data pointer: %p\n",(*it)->size_,(*it)->free_space(),(*it)->data_ );
    }

XDR helper: xdrbuf
------------------

xdrbuf helps in serializing basic types into and from XDR. xdrbuf is based on pbuf, so data will be stored in pbuf and read from pbuf. A typical usage scenario is:

    pbuf pb;
    xdrbuf xb(pb);
    
    xb << (int32_t)123;
    xb << "Hello world!";

The above example stores the two variable in serialized XDR form into pb. Reading back from pb would be like this:

    xdrbuf xo(pb);
    int32_t i;
    std::string s;
    
    xo >> i;
    xo >> s;

Now i and s contains 123 and "Hello world!" respectively.

Compressed file helper: zfile
-----------------------------

zfile is based on zlib. It can read and write plain and compressed files. If plain data is placed into zfile than it can compress and write it to a file in a the filesystem. If only in-memory compression is needed then zfile will happily do that for you.

Compress in memory data:

    pbuf pb;
    pb.append("Hello world!");
    
    zfile zf;
    zf.put_data( pb );
    
    pbuf zpb;
    zf.get_zdata( zpb );

zpb now contains the compressed "Hello world!" string. Data can be put and get into and from zfrom in many forms that includes pbuf as in the example, unsigned char pointers and files. The z prefix means compressed and if z is not there it is uncompressed:

    /* uncompressed file ops */
    read_file
    write_file
    
    /* compressed file ops */
    read_zfile
    write_zfile
    
    /* uncompressed other ops */
    get_data
    get_data_const
    get_buff
    get_size
    
    /* compressed other ops */
    get_zdata
    get_zdata_const
    get_zbuff
    get_zsize

Templated buffer: tbuf
----------------------

The tbuf class is used for the case when a buffer needed to handle dynamic data and the typical size of this data is small and the known in advance for the majority of cases. One such case is a string that stores passwords or login names. They are usually a few tens of bytes and complex string manipulation is not needed for them. For this case tbuf can be a better option then std::string because:

* initialization of tbuf is a lot cheaper
* adding data to tbuf does not allocate dynamic data if it fits into the size specified in the template parameter
* complex string manipulation is not needed

tbuf in general is faster than std::string because it uses statically allocated memory. This static allocation is handled on the stack and way faster than std::string's dynamic allocation.

    tbuf<64>  password;
    tbuf<64>  login;

The user has to take care of the trailing zero if needed. tbuf is a generic buffer class that treats its data as opaque array of unsigned chars. This makes it suitable for using in the security module's bignum. The bignum class is used in the elliptic curve related functions. The typical size is known in advance. In case of bignum it is declared as:

    typedef common::tbuf<128> buf_t;

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

The event variable will help handlers to determine when new message arrives and the mutex will guard internal variables. The event variable works in concert with nthread::thrpool that is able to manage thread pools. In order to utilize it we have to create an event handler callback:

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

    thrpool       thread_pool;
    buffers       bfrs;
    msg_handler   handler(bfrs);
    
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

The catch here that effects the design of circbuf is that I don't want the circular buffer to be locked while waiting for new messages. For this reason the push() may be split to prepare()/commit()/rollback() steps. The prepare() call reserves space for the new message and the commit() call pushes that into the circular buffer. The rollback() call undos the prepare() step, thus putting the buffer to the free-list of the circular buffer.


