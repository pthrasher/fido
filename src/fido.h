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

// Protocol related stuff.

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
