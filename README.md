mcoap
=====

A modestly portable (win32/linux) implementation of CoAP in C.

Currently in initial development. If you are interested in CoAP, 
[libcoap](https://libcoap.net/) seems to be the library of choice, but
appears to require cygwin on Windows.

I'm building this specifically to move gracefully between some really old Win32 
systems that I have to use and Linux.

Things I'm in the progress of doing before creating an initial release:

  * Revisit Win32 builds and fix. Includes adding Win32 specific settings to new CMake files.
  * Complete a reasonable subset of the [CoAP Plugtest](https://github.com/cabo/td-coap4/) tests.
  * Write a minimal programmers' guide.


See [RFC 7252](http://tools.ietf.org/html/rfc7252 "RFC 7252") for the CoAP specification.
