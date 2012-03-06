/*
    Copyright 2012 Philip Thrasher
    Distributed under the MIT License (see accompanying file LICENSE
    or a copy at http://www.opensource.org/licenses/MIT
*/
#ifndef _args_h
#define _args_h

#include <arpa/inet.h> // Needed for socket stuff.
#include <getopt.h> // Needed for socket stuff.
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "bitmap.h"
#include "util.h"

typedef struct fido_args {
  uint32_t port;
  char* origport;
  uint32_t address;
  char* origaddress;
  bitmapnum_t size;
} fido_args;

void get_args(fido_args* arg_out, int argc, char* argv[]);

#endif
