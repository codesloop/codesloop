Codesloop
=========

Codesloop project aimed at collecting useful C++ code that will later be used in distributed applications. There are several pieces already written and the it grows dinamically.

Code structure
--------------
The code is structured into modules. These modules are used as children namespaces of the main codesloop namespace: csl. So classes in the sec module are placed into the csl::sec namespace.

Each module has a src directory for the sources and a test directory for the corresponding tests. New code only gets into the main branch if it is well tested. Doxygen documentation is also created for each class.

Modules
-------

*  *common module* : common code including performance sensible memory pooling and other memory helpers, XDR helper, test timer, zlib compressed file handling
*  *nthread module* : cross platform threading primitives like thread, event and mutex
*  *sec module* : security functions that includes Elliptic Curve Diffie Hellman key exchange, AES/UMAC based encrypted packets and Blowfish/SHA1 base encrypted buffers
*  *comm module* : _under construction_ (the first to complete: UDP based secured unicast and multicast communication)
*  *slt3 module* : SQLite3 helpers including a simple Object Relational Mapper (ORM)
*  *sched module* : _under construction_ (distributed schedule)

[Codesloop home](http://codesloop.blogspot.com/)
