Codesloop
=========

Codesloop project aimed at collecting useful C++ code that will later be used in distributed applications. There are several pieces already written and the it grows dinamically.

Code structure
--------------
The code is structured into modules. These modules are used as children namespaces of the main codesloop namespace: csl. So classes in the sec module are placed into the csl::sec namespace.

Each module has a src directory for the sources and a test directory for the corresponding tests. New code only gets into the main branch if it is well tested. Doxygen documentation is also created for each class.

Modules
-------

*  [common module](common/)
*  [nthread module](nthread/)
*  [sec module](sec/)
*  [comm module](comm/)
*  [slt3 module](slt3/)
*  [sched module](sched/)

[Codesloop home](http://codesloop.blogspot.com/)
