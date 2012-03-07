/*
    Copyright 2012 Philip Thrasher
    Distributed under the MIT License (see accompanying file LICENSE
    or a copy at http://www.opensource.org/licenses/MIT
*/
#include "fido.h"

void setDefaultArgs(fido_args* args) {
  args->port = htons(8030);
  args->origport = "8030";
  args->address = INADDR_ANY;
  args->origaddress = "127.0.0.1";
  args->size = 512;
}
void get_args(fido_args* arg_out, int argc, char* argv[]) {
  int c;
  struct in_addr hostAddr;
  int port = 0;
  long size = 0;

  enum { MAX = 4294967295, MIN = 1 };

  while ((c = getopt (argc, argv, "a:p:s:")) != -1)
  {
    switch (c)
    {
      case 'a':
        /* Address */

        if (inet_aton(optarg, &hostAddr) != 0)
        {
          arg_out->address = hostAddr.s_addr;
        }
        else
        {
          ohNoes("I couldn't quite parse that IPAddr, Jim. (%s)", optarg);
        }

        /* No error, proceed */
        arg_out->origaddress = strdup(optarg);
        arg_out->address = hostAddr.s_addr;
        break;
      case 'p':
        /* Port */

        port = atoi(optarg);

        /* No error, proceed */
        arg_out->origport = strdup(optarg);
        arg_out->port = htons(port);
        break;
      case 's':
        /* Size */
        size = atol(optarg);

        if (size > UINTMAX_MAX || size < MIN) {
          ohNoes("You must specify *at least* a size of 1, and no more than %u\n", (unsigned)UINTMAX_MAX);
        }

        /* No error, proceed */
        arg_out->size = (bitmapnum_t)size;
        break;
      case '?':
        if (optopt == 'c' || optopt == 'a' || optopt == 's')
          fprintf(stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint(optopt))
          fprintf(stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf(stderr,
              "Unknown option character `\\x%x'.\n",
              optopt);
        break;
      default:
        abort ();
    }
  }
}
