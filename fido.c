#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <netinet/in.h>
#include <resolv.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#include "bit-array.h"

void hostSocketInitAndBind(int *hostSocket, struct sockaddr_in *hostAddr, int port) {

  // Create the socket, with basic options.
  *hostSocket = socket(AF_INET, SOCK_STREAM, 0);

  // Set options on the socket.
  int so_reuseaddr_optval = 1;
  int so_keepalive_optval = 1;

  setsockopt(*hostSocket, SOL_SOCKET, SO_REUSEADDR, &so_reuseaddr_optval, sizeof(int));
  setsockopt(*hostSocket, SOL_SOCKET, SO_KEEPALIVE, &so_keepalive_optval, sizeof(int));

  // Basic socket settings
  hostAddr->sin_family = AF_INET;
  hostAddr->sin_port = htons(port);

  // Clean that memory out. Not sure what's in there.
  memset(&(hostAddr->sin_zero), 0, 8);

  // We're going to bind to 0.0.0.0
  hostAddr->sin_addr.s_addr = INADDR_ANY;

  // Bind and listen for connections.
  bind(*hostSocket, (struct sockaddr*)&*hostAddr, sizeof(*hostAddr));
  listen(*hostSocket, 10);

}

struct message {
  bit_array_num_t offset;
  char val;
};

struct SHArgs
{
    int *clientSocket;
    bit_array_t *bits;
};

void* socketHandler(struct SHArgs *shargs) {
  int *clientSocket = shargs->clientSocket;
  bit_array_t *bits = shargs->bits;

  int packetLen = 5;
  int byteCount;

  struct message msg;
  struct message rsp;

  char read_char = 'r';
  char set_char = 's';
  char unset_char = 'u';
  char error_char = 'e';


  while ((byteCount = recv(*clientSocket, &msg, 5, 0)) > 0) {

    /* printf("got data\n"); */

    rsp.offset = msg.offset;
    rsp.val = error_char;

    if (msg.val == read_char) {
      /* printf("read\n"); */
      if(bit_array_test(bits, msg.offset)) {
        rsp.val = set_char;
      } else {
        rsp.val = unset_char;
      }
    } else if (msg.val == set_char){
      /* printf("set\n"); */
      bit_array_set_true(bits, msg.offset);
      rsp.val = set_char;
    } else if (msg.val == unset_char) {
      /* printf("unset\n"); */
      bit_array_set_false(bits, msg.offset);
      rsp.val = unset_char;
    } else {
      /* printf("recieved malformed action.\n"); */
    }

    send(*clientSocket, &rsp, packetLen, 0);
  }
  shutdown(*clientSocket, SHUT_WR);
  close(*clientSocket);

  free(clientSocket);
  printf("Lost a connection.\n");
  return NULL;
}

int main(int argc, char *argv[]) {
  // enums
  int port = 8030;

  // 2^32 - 1
  bit_array_num_t NUM_BITS = 4294967295;

  bit_array_t bits;
  bit_array_init(&bits);
  bit_array_set_num_bits(&bits, NUM_BITS);
  bit_array_set_all_false(&bits);

  int hostSocket;
  int *clientSocket;

  socklen_t addrSize;

  struct sockaddr_in myAddr;
  struct sockaddr_in sourceAddr;

  pthread_t thread_id=0;

  addrSize = sizeof(struct sockaddr_in);

  hostSocketInitAndBind(&hostSocket, &myAddr, port);

  printf("Awaiting connections.\n");

  sigset(SIGPIPE, SIG_IGN);

  while (1)
  {
    clientSocket = (int*)malloc(sizeof(int));

    if ((*clientSocket = accept(hostSocket, (struct sockaddr*)&sourceAddr, &addrSize)) != -1 )
    {
      printf("Client connection established.\n");
      struct SHArgs shargs;
      shargs.clientSocket = clientSocket;
      shargs.bits = &bits;

      pthread_create(&thread_id, 0, (void *)&socketHandler, &shargs);
      pthread_detach(thread_id);


    } else {
      fprintf(stderr, "Error accepting %d\n", errno);
    }
  }

  return 0;

}
