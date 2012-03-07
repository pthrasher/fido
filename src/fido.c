/*
    Copyright 2012 Philip Thrasher
    Distributed under the MIT License (see accompanying file LICENSE
    or a copy at http://www.opensource.org/licenses/MIT
*/
#include "fido.h"

/* Globals */

fidoServer server;

/* End Globals */


void setDefaultArgs(fido_args* args) {
  args->port = htons(8030);
  args->origport = "8030";
  args->address = INADDR_ANY;
  args->origaddress = "127.0.0.1";
  args->size = 512;
}

void setupBitmap(bitmap_t* bits, bitmapnum_t size) {
  bitmapInit(bits);
  bitmapSetNumBits(bits, size);
  bitmapSetAllFalse(bits);
}

int main(int argc, char *argv[]) {
  /* Settings */
  fido_args args;
  setDefaultArgs(&args);
  get_args(&args, argc, argv);

  /* Hello! */
  welcomeMsg();
  versionMsg();
  printMsg("Port: %s\nBinding Address: %s\nBitmap Size: %u\n",
      args.origport, args.origaddress, args.size);

  /* Create the bitmap */
  bitmap_t bits;
  setupBitmap(&bits, args.size);

  return 0;

  /* Establish the global server object */
  server = createServer(args.address, args.port, &bits);
  runServer(&server);

  int *client_socket;
  struct sockaddr_in sourceAddr;

  pthread_t thread_id = 0;

  sigset(SIGPIPE, SIG_IGN);

  return 0;

}
