
// Standard stuff for header files. If it's already defined, don't do it again.
#ifndef _fido_h
#define _fido_h

#include <stdio.h>
#include <stdarg.h>

#include "version.h"
#include "bit-array.h"

typedef struct fido_args {
  uint32_t port;
  char *origport;
  uint32_t address;
  char *origaddress;
  bit_array_num_t size;
} fido_args;


typedef struct socket_args {
    int *client_socket;
    bit_array_t *bits;
} socket_args;

// For incoming messages from a client, you need to send the following struct.
// The server properly handles network byte order to host byte order
// conversions.
// The protocol is as follows:
// For reads:
//  client must send the offset, immediately followed by the state where state
//  is one of the following:
//  * 'r' read
//  * 's' set
//  * 'u' unset
//  offset must conform to bit_array_num_t (currently uint32_t)
typedef struct fido_message {
  bit_array_num_t offset;
  char state;
} fido_message;

// I know this is a one-item struct, and that's silly. But, I want to have some
// place to specify how we return data to the client.
//
// basically, the response goes like this:
// For reads:
//   's' is returned if the bit is set.
//   'u' is returned if the bit is unset.
//   'e' is returned if there was an error with the request.
// For writes:
//   's' is returned to denote success if you asked to set it.
//   'u' is returned to denote success if you asked to unset it.
//   'e' is returned for all failures, and errors.
typedef struct fido_response {
  char state;
} fido_response;

typedef struct message_ring_buffer {
    int          size;   /* maximum number of elements           */
    int          start;  /* index of oldest element              */
    int          end;    /* index at which to write new element  */
    fido_message *elems; /* vector of elements                   */
} message_ring_buffer;

typedef struct response_ring_buffer {
    int           size;   /* maximum number of elements           */
    int           start;  /* index of oldest element              */
    int           end;    /* index at which to write new element  */
    fido_response *elems; /* vector of elements                   */
} response_ring_buffer;

// Messages
// ========
//
// Here is where I define all of the banners/messages used when printing to the
// console.

// ### Colors
//
// Just a simple list of colors defined for use in the below messages, or
// anywhere else in the app for that matter.
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// ### print_msg
//
// This is a generic printing function we'll use for printing anything out to
// the console. This way we only have one place to change the fd we're printing
// to.
void print_msg(const char *msg, ...) {
  va_list a_list;
  va_start(a_list, msg);
  vfprintf(stderr, msg, a_list);
  va_end(a_list);
}

// ### ohnoes
//
// This is a fun ascii art "OH NOES!" message that prints out any time we have
// catastrophic failure.
char *ohnoes = 
  ANSI_COLOR_RED
  "          ___           ___\n"
  "         /  /\\         /__/\\ \n"
  "        /  /::\\        \\  \\:\\ \n"
  "       /  /:/\\:\\        \\__\\:\\ \n"
  "      /  /:/  \\:\\   ___ /  /::\\ \n"
  "     /__/:/ \\__\\:\\ /__/\\  /:/\\:\\ \n"
  "     \\  \\:\\ /  /:/ \\  \\:\\/:/__\\/ \n"
  "      \\  \\:\\  /:/   \\  \\::/ \n"
  "       \\  \\:\\/:/     \\  \\:\\ \n"
  "        \\  \\::/       \\  \\:\\ \n"
  "         \\__\\/         \\__\\/ \n"
  "          ___           ___           ___           ___ \n"
  "         /__/\\         /  /\\         /  /\\         /  /\\     ___ \n"
  "         \\  \\:\\       /  /::\\       /  /:/_       /  /:/_   /__/\\ \n"
  "          \\  \\:\\     /  /:/\\:\\     /  /:/ /\\     /  /:/ /\\  \\  \\:\\ \n"
  "      _____\\__\\:\\   /  /:/  \\:\\   /  /:/ /:/_   /  /:/ /::\\  \\  \\:\\ \n"
  "     /__/::::::::\\ /__/:/ \\__\\:\\ /__/:/ /:/ /\\ /__/:/ /:/\\:\\  \\  \\:\\ \n"
  "     \\  \\:\\~~\\~~\\/ \\  \\:\\ /  /:/ \\  \\:\\/:/ /:/ \\  \\:\\/:/~/:/   \\  \\:\\ \n"
  "      \\  \\:\\  ~~~   \\  \\:\\  /:/   \\  \\::/ /:/   \\  \\::/ /:/     \\__\\/ \n"
  "       \\  \\:\\        \\  \\:\\/:/     \\  \\:\\/:/     \\__\\/ /:/          __ \n"
  "        \\  \\:\\        \\  \\::/       \\  \\::/        /__/:/          /__/\\ \n"
  "         \\__\\/         \\__\\/         \\__\\/         \\__\\/           \\__\\/ \n"
  ANSI_COLOR_RESET;

char *usage = 
  "\nusage:\n"
  "    fido [-a ipaddress] [-p port] [-s bitmapsize]\n\n";

char *welcome = 
  ANSI_COLOR_BLUE
  "   _  _      _ \n"
  " / _|(_)    | |\n"
  "| |_  _   __| |  ___\n"
  "|  _|| | / _` | / _ \\ \n"
  "| |  | || (_| || (_) |\n"
  "|_|  |_| \\__,_| \\___/ \n"
  ANSI_COLOR_RESET;

char *versionmsg =
  ANSI_COLOR_MAGENTA
  "version "
  FIDO_VERSION
  "\n\n"
  ANSI_COLOR_RESET;

// Macros
// ======
//
// Macro purpose varies greatly, but as all macros, the below generally just
// make my coding life easier.


// Message Printers
// ----------------
//
// These are the macros used to easily print the messages from the above
// section.

// ### usage_msg
//
// Call this 'method' in your code, and the usage statement will be printed
// using print_msg.
#define usage_msg() print_msg(usage);

// ### oh_noes_msg
//
// Call this 'method' in your code, and the ohnoes ascii art will be printed
// using print_msg.
#define oh_noes_msg() print_msg(ohnoes);

// ### welcome_msg
//
// Call this 'method' in your code, and the logo ascii art will be printed
// using print_msg.
#define welcome_msg() print_msg(welcome);

// ### version_msg
//
// Call this 'method' in your code, and the version statement will be printed
// using print_msg.
#define version_msg() print_msg(versionmsg);


// Error Handling
// --------------
//
// These are the error handling macros.

// ### oh_noes
//
// This is a catastrophic failure message. Basically anytime the app hits
// a fatal error, it calls this with a message, and it in turn prints out
// ohnoes from above, prints the passed in message, prints a usage statement,
// and then exits with code 1.
#define usage_exit() { usage_msg(); exit(1); }
#define oh_noes(M, ...) { oh_noes_msg(); print_msg(M, ##__VA_ARGS__); usage_exit(); }


// End of `#ifndef _fido_h`
#endif
