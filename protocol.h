/*
    Copyright 2012 Philip Thrasher
    Distributed under the MIT License (see accompanying file LICENSE
    or a copy at http://www.opensource.org/licenses/MIT
*/

#ifndef _protocol_h
#define _protocol_h

#include <stdbool.h>

#include "bit-array.h"

#ifdef __cplusplus
extern "C" {
#endif

// The message that the api recieves, as well as responds with.
typedef struct fido_message_t {

  bit_array_num_t offset;
  char state;

} fido_message_t;

// The sole arg passed into the socket handler.
typedef struct socket_and_bits_t {

  int *socket;
  bit_array_t *bits;

} socket_and_bits_t;

// Handler for all client sockets.
void fido_socket_handler(socket_and_bits_t sab);

// Safely destroy the socket and free the memory.
void destroy_socket(int *socket);


#ifdef __cplusplus
}
#endif

#endif
