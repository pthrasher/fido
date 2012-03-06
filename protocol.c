#include <stdlib.h>     // Needed for lots of stuff, including free, and malloc.
#include <unistd.h>     // Needed for `close`-ing sockets.
#include <sys/socket.h> // Needed for send/recv

#include "bit-array.h"
#include "protocol.h"

void fido_socket_handler(socket_and_bits_t sab) {
  // local socket ref, local bit ref
  int *client_socket_ptr = sab.socket;
  bit_array_t *bits_ptr = sab.bits;

  // Size of each message (for sending, and receiving)
  int message_size = sizeof(socket_and_bits_t);
  int byte_count;

  // message structs
  fido_message_t message;
  fido_message_t response;

  // Protocol constants
  char const READ  = 'r';
  char const SET   = 's';
  char const UNSET = 'u';
  char const ERROR = 'e';

  // recv so long as the connection is open, or there's no error
  while ((byte_count = recv(*client_socket_ptr, &message, 5, 0)) > 0) {

    // Populate defaults for response
    response.offset = message.offset;
    response.state = ERROR;

    // Check the message.
    switch (message.state) {
      case 'r':
      case 'R':
        if (bit_array_test(bits_ptr, message.offset)) {
          response.state = SET;
        } else {
          response.state = UNSET;
        }
        break;

      case 's':
      case 'S':
        bit_array_set_true(bits_ptr, message.offset);
        response.state = SET;
        break;

      case 'u':
      case 'U':
        bit_array_set_false(bits_ptr, message.offset);
        response.state = UNSET;
        break;
    }

    // Send the data to the socket buffer.
    send(*client_socket_ptr, &response, message_size, 0);
  }

  // The connection has been closed, let's destroy the socket.
  destroy_socket(client_socket_ptr);

}

void destroy_socket(int *socket) {

  shutdown(*socket, SHUT_WR);
  close(*socket);
  free(socket);

}
