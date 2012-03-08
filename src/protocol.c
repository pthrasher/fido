/*
    Copyright 2012 Philip Thrasher
    Distributed under the MIT License (see accompanying file LICENSE
    or a copy at http://www.opensource.org/licenses/MIT
*/
#include "util/globals.h"
#include "fido/protocol.h"
#include "fido/socketz.h"
#include "fido/bitmap.h"
#include <stdint.h>

fidoServer fidoServerAlloc ( char * ipAddr, int port, bitmap_t* bits ) {
  int yes = 1;
  fidoServer _server;
  _server.fd = serverAlloc(port, ipAddr);

  // Global Server settings
  _server.bits = bits;

  // Shouldn't be greater than max fd's.
  _server.maxClients = 1024;

  // Internal counters and stats.
  _server.numClients = 0;
  _server.numRequests = 0;

  return _server;
}

