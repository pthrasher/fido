/*
    Copyright 2012 Philip Thrasher
    Distributed under the MIT License (see accompanying file LICENSE
    or a copy at http://www.opensource.org/licenses/MIT
*/
#include "fido.h"

void server_init(int *hostSocket, struct sockaddr_in *hostAddr, uint32_t address, uint32_t port) {

  // Create the socket, with basic options.
  *hostSocket = socket(AF_INET, SOCK_STREAM, 0);

  // Set options on the socket.
  int so_reuseaddr_optval = 1;
  int so_keepalive_optval = 1;

  setsockopt(*hostSocket, SOL_SOCKET, SO_REUSEADDR, &so_reuseaddr_optval, sizeof(int));
  setsockopt(*hostSocket, SOL_SOCKET, SO_KEEPALIVE, &so_keepalive_optval, sizeof(int));

  // Basic socket settings
  hostAddr->sin_family = AF_INET;
  hostAddr->sin_port = port;

  // Clean that memory out. Not sure what's in there.
  memset(&(hostAddr->sin_zero), 0, 8);

  // We're going to bind to 0.0.0.0
  hostAddr->sin_addr.s_addr = INADDR_ANY;

  // Bind and listen for connections.
  bind(*hostSocket, (struct sockaddr*)&*hostAddr, sizeof(*hostAddr));
  listen(*hostSocket, 10);

}

int sendall(int s, void *buf, int *len) {
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while(total < *len) {
        n = send(s, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
}

void *socket_handler(socket_args *args) {
  int *client_socket = args->client_socket;
  bitmap_t *bits = args->bits;

  int packetLen = 5;
  int byte_count;

  fido_message msg;
  fido_response rsp;

  int num_messages = 0;
  int num_responses = 0;

  // These two numbers should never really be different, though, I guess it's
  // okay.
  int max_messages = 240;
  int max_responses = 1200;

  message_ring_buffer message_queue;
  response_ring_buffer response_queue;

  rbm_init(&message_queue, max_messages, sizeof(fido_message));
  rbr_init(&response_queue, max_responses, sizeof(fido_response));


  char read_char = 'r';
  char set_char = 's';
  char unset_char = 'u';
  char error_char = 'e';

  fd_set master;    // master file descriptor list
  fd_set read_fds;  // temp file descriptor list for select()
  fd_set write_fds;  // temp file descriptor list for select()

  FD_ZERO(&master);
  FD_ZERO(&read_fds);
  FD_ZERO(&write_fds);

  FD_SET(*client_socket, &master); // I realize this seems silly...
  read_fds = master;
  write_fds = master;
  int nfds = *client_socket + 1;

  // Description of process below.:
  // Create new process built on select.
  // * malloc and clear msg stack
  // * malloc and clear response stack
  // * select the socket.
  // * recv when socket is ready.
  //   * recv 5 bytes at a time until all bytes are sent
  //   - or -
  //   * until you've hit max messages per read limit
  // * When done reading, dispatch all messages.
  //   * Add responses to response stack
  // * send until all responses are sent.
  // * start process over.
  // * Also, make sure we do all endian conversions here. Just to be safe.


  // Notes:
  // select returns -1 on an error, 0 on none ready if the timeout expires, or
  // the total number of fds that are ready.
  int should_read = 0;
  int should_dispatch = 0;
  int should_write = 0;

  int read_ready = 0;
  int write_ready = 0;

  // create an immediate timeout for inner selects.
  struct timeval no_t:ime;
  no_time.tv_sec = 0;
  no_time.tv_usec = 0;

  struct timeval short_time;
  no_time.tv_sec = 1;
  no_time.tv_usec = 0;

  int msg_size = (int)sizeof(fido_message);
  int rsp_size = (int)sizeof(fido_response);

  int my_test;

  // I set byte_count to -1 if I ever get 0 back from a recv.
  while (byte_count >= 0) {

    // Wait here until there's data to be read.
    read_ready = select(nfds, &read_fds, NULL, NULL, NULL);
    if (read_ready == -1) {
      // major error with client. We'll likely need to close and free the
      // socket.
      byte_count = -1;
      break;
    } else if (read_ready == 0) {
      // not ready to be read yet.
      should_read = 0;
    } else {
      // We're ready to read some data.
      should_read = 0;
      if (!rbm_is_full(&message_queue)){
        // Just make sure that the queue isn't full already.
        should_read = 1;
      }

      while (should_read) {
        // We're looping here so long as we should continue reading.
        byte_count = recv(*client_socket, &msg, 5, 0);
        if (byte_count < 5) {
          // Hmm... t)is shouldn't happen.
          // we're here because the client didn't send enough data. We should
          // probably just ignore this data, and discard it. I'll ask my
          // friends that are smarter than me what they think. AFAIK the socket
          // wouldn't be ready for reading if there were partial packets on it.
          // Though, I was wrong once... Hopefully someone out there can answer
          // this. I'll think of things, and document possible options here.
          //
          // Option 1:
          //   Queue a msg with the state of 'e' to denote an error, and return
          //   an error back to the client for that one.
          should_read = 0;
          byte_count = 0;
          continue;
        } else {
          // **Rubber meets the road for reading**
          //
          // first, we want to be endian safe, then queue the message. we only
          // have to convert the offset as the state is a char (1 byte) and not
          // affected by endieness.
          // Network Byte Order to Host Long cast as proper type.
          msg.offset = (bitmapnum_t)ntohl(msg.offset);
          if (msg.offset > bits->num_bits) {
            // Either the client has sent us the data in the wrong format, or
            // they're asking for something out of bounds. Either way, we're
            // not writing this msg.
          } else {
            // Append the message to the queue.
            rbm_write(&message_queue, &msg);

            if (rbm_is_full(&message_queue)) {
              // We're now at our limit.
              // Time to dispatch all messages, and queue up responses.
              should_read = 0;
              byte_count = 0;
              continue;
            }
          }
          read_fds = master;
          read_ready = select(nfds, &read_fds, NULL, NULL, &no_time);
          if (read_ready == -1) {
            // some kind of socket error.
            should_read = 0;
            byte_count = -1;
            continue;
          } else if (read_ready == 0) {
            // No more data to be read at this time. Let's move onto selection.
            should_read = 0;
            byte_count = 0;
            continue;
          }
          // If we're here, there's data to be read.
        }
      }

      if (byte_count == -1) {
        // We've continued from the read loop, and the client connecton is dead
        // or closed.
        continue;
      }
    }

    // At this point, we've got some queued up messages. Let's loop through
    // them, dispatch each one, and add the result to the response queue.
    should_dispatch = 0;
    if (!rbr_is_full(&response_queue)) {
      // The queue is full, we need to go down and send some messages. If we
      // can't do that, we'll loop back around again until we can.
      should_dispatch = 1;
    }

    while (should_dispatch) {
      // Grab the 
      if (rbm_is_empty(&message_queue)) {
        should_dispatch = 0;
        continue;
      }
      fido_message tmp_msg;
      rbm_read(&message_queue, &tmp_msg);

      rsp.state = error_char;

      switch (tmp_msg.state) {
        case 'r':
        case 'R':
          if(bitmap_test(bits, tmp_msg.offset)) {
            rsp.state = set_char;
          } else {
            rsp.state = unset_char;
          }
          break;
        case 's':
        case 'S':
          bit_array_set_true(bits, tmp_msg.offset);
          if(bitmap_test(bits, tmp_msg.offset)) {
            rsp.state = set_char;
          } else {
            rsp.state = error_char;
          }
          break;
        case 'u':
        case 'U':
          bit_array_set_false(bits, tmp_msg.offset);
          if(bitmap_test(bits, tmp_msg.offset)) {
            rsp.state = error_char;
          } else {
            rsp.state = unset_char;
          }
          break;
        default:
          break;
      }


      rbr_write(&response_queue, &rsp);

      if (rbr_is_full(&response_queue)) {
        // We've managed to fill up the response queue. This is very odd, but
        // possible. We need to break out of the for loop. and stop reading in
        // messages.
        should_dispatch = 0;
        continue;
      }
    }

    // Now it's time to try to write all of the responses back to the client
    // that we've queued up.
    should_write = 1;
    my_test = rbr_is_empty(&response_queue);
    if (rbr_is_empty(&response_queue)) {
      // We don't have anything to send right now.
      should_write = 0;
    }
    write_fds = master;
    write_ready = select(nfds, NULL, &write_fds, NULL, &no_time);
    if (write_ready == -1) {
      // some kind of socket error.
      should_write = 0;
      byte_count = -1;
      continue;
    } else if (write_ready == 0) {
      // Not ready to write right now. We'll just have to come back around. And
      // try again in a minute.
    } else {
      while (should_write) {
        fido_response tmp_rsp;

        if (rbr_is_empty(&response_queue)) {
          // We don't have anything to send right now.
          should_write = 0;
          continue;
        }
        rbr_read(&response_queue, &tmp_rsp);

        // Send the byte
        byte_count = send(*client_socket, &tmp_rsp, 1, 0);

        if (byte_count < 1) {
          // There was an error here... We're going to have to figure out if
          // it's worth handling or not. For now, we're just going to forget
          // about that byte, and hope it actually made it there.
        }

        // Quickly check to see if the socket is ready for more writes.
        write_fds = master;
        write_ready = select(nfds, NULL, &write_fds, NULL, &no_time);
        if (write_ready == -1) {
          // some kind of socket error.
          should_write = 0;
          byte_count = -1;
          continue;
        } else if (write_ready == 0) {
          should_write = 0;
          byte_count = 0;
          continue;
        }
      }
    }

    byte_count = 0;
    should_read = 0;
    should_dispatch = 0;
    should_write = 0;
  }

  shutdown(*client_socket, SHUT_WR);
  close(*client_socket);
  free(client_socket);
  rbm_free(&message_queue);
  rbr_free(&response_queue);
  printf("Lost a connection.\n");
  return NULL;
}

int main(int argc, char *argv[]) {
  fido_args args;

  args.port = htons(8030);
  args.origport = "8030";
  args.address = INADDR_ANY;
  args.origaddress = "127.0.0.1";
  args.size = 512;

  get_args(&args, argc, argv);
  welcome_msg();
  version_msg();

  print_msg("Port: %s\nBinding Address: %s\nBitmap Size: %u\n",
      args.origport, args.origaddress, args.size);

  // Set up the bitmap.
  bitmap_t bits;
  bit_array_init(&bits);
  bit_array_set_num_bits(&bits, args.size);
  bit_array_set_all_false(&bits);

  int hostSocket;
  int *client_socket;

  socklen_t addrSize;

  struct sockaddr_in myAddr;
  struct sockaddr_in sourceAddr;

  pthread_t thread_id=0;

  addrSize = sizeof(struct sockaddr_in);

  server_init(&hostSocket, &myAddr, args.address, args.port);

  printf("Awaiting connections.\n");

  sigset(SIGPIPE, SIG_IGN);

  while (1) {
    client_socket = (int*)malloc(sizeof(int));

    if ((*client_socket = accept(hostSocket, (struct sockaddr*)&sourceAddr, &addrSize)) != -1 )
    {
      printf("Client connection established.\n");
      socket_args sargs;
      sargs.client_socket = client_socket;
      sargs.bits = &bits;

      pthread_create(&thread_id, 0, (void *)&socket_handler, &sargs);
      pthread_detach(thread_id);


    } else {
      fprintf(stderr, "Error accepting connection.\n");
    }
  }


  return 1;

}
