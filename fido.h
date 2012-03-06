/*
    Copyright 2012 Philip Thrasher
    Distributed under the MIT License (see accompanying file LICENSE
    or a copy at http://www.opensource.org/licenses/MIT
*/

// Standard stuff for header files. If it's already defined, don't do it again.
#ifndef _fido_h
#define _fido_h

#include <fcntl.h>
#include <signal.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>   // Needed for threading.
#include <arpa/inet.h> // Needed for socket stuff.
#include <string.h>    // Needed for memset
#include <stdarg.h>
#include <ev.h>

#include "bitmap.h"
#include "args.h"

typedef struct fido_server {
  uint32_t nbo_port;
  uint32_t nbo_address;
} fido_server;



typedef struct socket_args {
    int *client_socket;
    bit_array_t *bits;
} socket_args;

// For incoming messages from a client, you need to send the following struct.
// The server properly handles network byte order to host byte order
// conversions.
// The protocol is as follows:
// For reads:
//  client must send the offset, immediately followed by the state where state
//  is one of the following:
//  * 'r' read
//  * 's' set
//  * 'u' unset
//  offset must conform to bit_array_num_t (currently uint32_t)
typedef struct fido_message {
  bit_array_num_t offset;
  char state;
} fido_message;

// I know this is a one-item struct, and that's silly. But, I want to have some
// place to specify how we return data to the client.
//
// basically, the response goes like this:
// For reads:
//   's' is returned if the bit is set.
//   'u' is returned if the bit is unset.
//   'e' is returned if there was an error with the request.
// For writes:
//   's' is returned to denote success if you asked to set it.
//   'u' is returned to denote success if you asked to unset it.
//   'e' is returned for all failures, and errors.
typedef struct fido_response {
  char state;
} fido_response;



// End of `#ifndef _fido_h`
#endif
