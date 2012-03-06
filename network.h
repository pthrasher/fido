#ifndef __fidonetworking_h
#define __fidonetworking_h

typedef struct fidoClient {
  int* fd;
} fidoClient;


void* fidoClientEventLoop(fidoClient*);

int _readDataFromClient(fidoClient*);

fd_set createSingleFDSet(int);

int isFDReady(int*, char);

#endif
