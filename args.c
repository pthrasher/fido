/*
    Copyright 2012 Philip Thrasher
    Distributed under the MIT License (see accompanying file LICENSE
    or a copy at http://www.opensource.org/licenses/MIT
*/
#include "args.h"

void get_args(fido_args* arg_out, int argc, char* argv[]) {
  int c;
  struct in_addr host_addr;
  int port = 0;
  long size = 0;

  enum { MAX = 4294967295, MIN = 1 };

  while ((c = getopt (argc, argv, "a:p:s:")) != -1)
  {
    switch (c)
    {
      case 'a':
        /* Address */

        if (inet_aton(optarg, &host_addr) != 0)
        {
          arg_out->address = host_addr.s_addr;
        }
        else
        {
          oh_noes("I couldn't quite parse that IPAddr, Jim. (%s)", optarg);
        }

        /* No error, proceed */
        arg_out->origaddress = strdup(optarg);
        arg_out->address = host_addr.s_addr;
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

        if (size > MAX || size < MIN) {
          oh_noes("You must specify *at least* a size of 1, and no more than %u\n", MAX);
        }

        /* No error, proceed */
        arg_out->size = (bit_array_num_t)size;
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
