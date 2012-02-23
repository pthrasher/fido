Multi-threaded bit-array server
===============================

This is a server I've created that's multi-threaded, and allows you to have
a bit-array to query against. It's very fast. With the server running on my
machine, and the client as well, I did 30,000 requests per second.

Take a look at bitmap.py... That's the client I wrote.

The bit array code I stole from Sam Hocevar. Thanks!

TODO:
check for mem leaks
handle batched network IO on the server side.
learn C better.

