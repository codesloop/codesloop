common module sources
=====================

common uses the csl::common namespace and includes the following classes:


* [pvlist.hh](./pvlist.hh) : pvlist is a template class container for pointers, the D template parameter may be used to tell the object how to destruct the contained pointers. there are 3 destructors supplied (nop, delete and free) but others may be supplied if needed
* [mpool.hh](./mpool.hh) : memory pooling based on pvlist. memory allocation is done with malloc() and the pointers are free()'d on destruct
* [pbuf.hh](./pbuf.hh) : pbuf is a paged buffer implementation. memory is stored in fixed length chunks.
* [tbuf.hh](./tbuf.hh) : templated memory buffer, where the template argument tells how much memory should be statically preallocated. this strategy provides huge performance gains in certain cases
* [test_timer.hh](./test_timer.hh) : performance measurement helpers
* [xdrbuf.hh](./xdrbuf.hh) : XDR encoding and decoding helper based on pbuf
* [zfile.hh](./zfile.hh) : zlib compressed file helper
