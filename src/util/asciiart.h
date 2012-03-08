#ifndef _asciiart_h
#define _asciiart_h

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


#endif
