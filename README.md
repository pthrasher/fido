Fido
====

A multi-threaded bit-array server written in ANSI C.


A what?
-------

Fido is really two things in one. It's a simple bit-array implementation, and
a multi-threaded socket server implementation mashed into one. The latest
versions of Redis also offer this functionality in the form of Redis Bitmaps,
however, Redis also does quite a bit more. If you just need bitmaps, and the
ability to query / manipulate them across a network, then Fido is for you.


Current Status
--------------

Fido is currently not even in alpha. It's in dev. There's still a few things on
the todo list, and it currently really only works in testing. There's no error
handling in the app whatsoever. Also, currently options like port, and bitmap
size are hard coded. These will eventually be command line opts.


Why?
----

I needed something very simple, and very very fast. In it's current dev stage,
this bitmap server can handle 500,000 queries per second. It's very
unoptimized, and can be greatly improved upon. Redis is the only other
mainstream viable option, and it offers too much. Additionally, I wanted
something ultra-portable. It's written in ANSI C with no external dependencies.
It's also such a small codebase (and will continue to be) that this is an easy
goal to keep alive.

In short, I wanted to follow the unix philosophy of "Do one thing well" and
there are currently no other viable options out there that fit the bill for
this task.


Notes
-----

There is a basic API wrapper in fido.py that you can look at to see how you
would interact with the protocol. There's also a test in there that creates
enough queries to fill a packet, and then ships the packet to the server
(batching requests on the client side.)

### How to use:

    cd fido
    make
    ./fido


Todo
----

* Create new thread to persist bitmap to disk.
* Handle command line arguments.
* Error handling
  * Don't allocate more sockets than available memory.
  * Don't allocate a larger bitmap than available memory.
  * Handle case where invalid input is given.
* Batch sending of responses on the server side.


Acknowledgements
----------------

Thanks to Sam Hocevar for the bit-array code, and for such a liberal license

* Code: https://github.com/kchmck/c-bit-array
* License: http://sam.zoy.org/wtfpl/COPYING


License
=======

The MIT License (MIT)
Copyright (c) 2012 Philip Thrasher

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
