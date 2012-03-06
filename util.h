
#ifndef _util_h
#define _util_h
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
#define usageMsg() printMsg(usage);

// ### oh_noes_msg
//
// Call this 'method' in your code, and the ohnoes ascii art will be printed
// using print_msg.
#define ohNoesMsg() printMsg(ohnoes);

// ### welcome_msg
//
// Call this 'method' in your code, and the logo ascii art will be printed
// using print_msg.
#define welcomeMsg() printMsg(welcome);

// ### version_msg
//
// Call this 'method' in your code, and the version statement will be printed
// using print_msg.
#define versionMsg() printMsg(versionmsg);


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
#define usageExit() { usageMsg(); exit(1); }
#define ohNoes(M, ...) { ohNoesMsg(); printMsg(M, ##__VA_ARGS__); usageExit(); }

#endif
