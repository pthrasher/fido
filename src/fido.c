/*
    Copyright 2012 Philip Thrasher
    Distributed under the MIT License (see accompanying file LICENSE
    or a copy at http://www.opensource.org/licenses/MIT
*/
#include "fido.h"

/* Globals */

fidoServer server;

/* End Globals */

/* Other Source Files */

#include "networking.c"
#include "bitmap.c"
#include "args.c"

/* End Other Source Files */

int main(int argc, char *argv[]) {
  /* Settings */
  fido_args args;
  setDefaultArgs(&args);
  get_args(&args, argc, argv);

  /* Hello! */
  welcomeMsg();
  versionMsg();
  printMsg("Port: %s\nBinding Address: %s\nBitmap Size: %u\n",
      args.origport, args.origaddress, (unsigned)args.size);

  /* Create the bitmap */
  bitmap_t* bits = newBitmap(args.size);

  return 0;

  /* Establish the global server object */
  server = createServer(args.address, args.port, bits);
  runServer(&server);

  int *client_socket;
  struct sockaddr_in sourceAddr;

  pthread_t thread_id = 0;

  sigset(SIGPIPE, SIG_IGN);

  return 0;

}
