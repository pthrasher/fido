/*
    Copyright 2012 Philip Thrasher
    Distributed under the MIT License (see accompanying file LICENSE
    or a copy at http://www.opensource.org/licenses/MIT
*/

// Standard stuff for header files. If it's already defined, don't do it again.
#ifndef _fido_h
#define _fido_h

// General necessities
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <stdarg.h>

// Socket related
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

// CLI Args
#include <getopt.h>

// Threading / Async
#include <pthread.h>
#include <ev.h>

// App specific
#include "version.h"
#include "asciiart.h"
#include "ringbuffer.h"

// Bitmap stuff
typedef uintmax_t bitmapnum_t;
typedef struct bitmap_t {
  bitmapnum_t *slots;
  bitmapnum_t numBits;
  bitmapnum_t numSlots;
} bitmap_t;
#define BITS_PER_SLOT CHAR_BIT * sizeof(bitmapnum_t)
#define ALL_TRUE_BIT_MASK UINTMAX_MAX
#define ALL_FALSE_BIT_MASK 0
void bitmapInit(bitmap_t*);
void bitmapDestroy(bitmap_t*);
bool bitmapSetNumBits(bitmap_t*, const bitmapnum_t);
void bitmapSetTrue(bitmap_t*, const bitmapnum_t);
void bitmapSetAllTrue(bitmap_t*);
void bitmapSetFalse(bitmap_t*, const bitmapnum_t);
void bitmapSetAllFalse(bitmap_t*);
void bitmapToggle(bitmap_t*, const bitmapnum_t);
void bitmapToggleAll(bitmap_t*);
bool bitmapTest(const bitmap_t*, const bitmapnum_t);
bool bitmapTestAllTrue(const bitmap_t*);
bool bitmapTestAllFalse(const bitmap_t*);
bool bitmapTestAnyTrue(const bitmap_t*);
bool bitmapTestAnyFalse(const bitmap_t*);
void memset64(void * dest, uint64_t value, uintptr_t size);

// Protocol related stuff.
typedef struct fido_message {
  bitmapnum_t offset;
  char state;
} fido_message;
typedef struct fido_response {
  char state;
} fido_response;

// Networking related stuff
typedef struct fidoServer {
  struct sockaddr_in addr;
  int fd;
  bitmap_t* bits;
  int maxClients;
  int numClients;
  long numRequests;
} fidoServer;
typedef struct fidoClient {
  struct sockaddr_in addr;
  int fd;
} fidoClient;
fidoServer createServer(uint32_t nboAddress, uint32_t nboPort, bitmap_t* bits);
int runServer(fidoServer* _server);
void initAddr(struct sockaddr_in* addr, uint32_t nboAddress, uint32_t nboPort);

// Settings related stuff.
typedef struct fido_args {
  uint32_t port;
  char* origport;
  uint32_t address;
  char* origaddress;
  bitmapnum_t size;
} fido_args;
void setDefaultArgs(fido_args* args);
void get_args(fido_args* arg_out, int argc, char* argv[]);

// Some logging macros

#define printMsg(M, ...) fprintf(stderr, M, ##__VA_ARGS__)
#define usageMsg() printMsg(usage, 0);
#define ohNoesMsg() printMsg(ohnoes, 0);
#define welcomeMsg() printMsg(welcome, 0);
#define versionMsg() printMsg(versionmsg, 0);
#define usageExit() { usageMsg(); exit(1); }
#define ohNoes(M, ...) { ohNoesMsg(); fprintf(stderr, M, ##__VA_ARGS__); usageExit(); }


// End of `#ifndef _fido_h`

#endif
