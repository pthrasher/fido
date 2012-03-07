#ifndef _util_h
#define _util_h

// ### print_msg
//
// This is a generic printing function we'll use for printing anything out to
// the console. This way we only have one place to change the fd we're printing
// to.
/* void printMsg(const char *msg, ...) { */
/*   va_list a_list; */
/*   va_start(a_list, msg); */
/*   vfprintf(stderr, msg, a_list); */
/*   va_end(a_list); */
/* } */


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

#endif
