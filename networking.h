#ifndef __fidonetworking_h
#define __fidonetworking_h

typedef struct fidoClient {
  int* fd;
} fidoClient;

fidoClient newFidoClient(int*, bit_array_t*)

void* fidoClientEventLoop(fidoClient*);

int readDataFromClient(fidoClient*);

int clientMessagesDispatchable(fidoClient*);

int dispatchClientMessages(fidoClient*);

int writeClientResponseBuffer(fidoClient*);

fd_set createSingleFDSet(int);

int isFDReady(int*, char);

#endif
