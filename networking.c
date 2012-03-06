#include <fcntl.h>
#include <signal.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>   // Needed for threading.
#include <arpa/inet.h> // Needed for socket stuff.
#include <string.h>    // Needed for memset
#include <stdarg.h>

#include <time.h>

#include "bit-array.h"
#include "fido.h"
#include "networking.h"


// Client Event Loop
// -----------------
//
// Basic process:
//
// 1. Loop through over and over again checking to see if there's anything to
//    do. Sleep at the end of the loop. (nano sleep)
// 2. If the socket is readable, read in data. Read in 2412 bytes at a time.
void* fidoClientEventLoop(fidoClient* c) {

  /* Just some bools */
  int readyToRead, readyToDispatch, readyToWrite, clientIsConnected;

  int readResult;

  /* Time related stuff. */
  struct timespec shortNap;
  struct timespec shortNapRem;

  shortNap.tv_sec = 0;
  shortNap.tv_nsec = 1000000; /* 1(ms) */

  clientIsConnected = 1;
  while (clientIsConnected) {
    readyToRead = isFDReady(c->fd, 'r');
    if (readyToRead > 0) {
      /* We're ready to read */
      readResult = readDataFromClient(c);
      if (readResult == -1) {
        clientIsConnected = 0;
        break;
      }
    } else if (readyToRead == -1) {
      /* Client disco, or fatal socket error. */
      clientIsConnected = 0;
      break;
    } /* else, we're not ready for reading yet */

    readyToDispatch = clientMessagesDispatchable(c);
    if (readyToDispatch) {
      /* Chomp through all of the messages. */
      dispatchClientMessages(c);
    }

    readyToWrite = isFDReady(c->fd, 'w');
    if (readyToWrite > 0) {
      writeClientResponseBuffer(c);
    } else if (readyToWrite == -1) {
      /* Client disco, or fatal socket error. */
      clientIsConnected = 0;
      break;
    } /* else, we're not ready for reading yet */


    /* Take a short nap. */
    /* We might want to move this into the `not ready to read yet` area. */
    nanosleep(&shortNap, &shortNapRem);

  }
  /* We're done here. */
  shutdown(*(c->fd), SHUT_WR);
  close(*(c->fd));
  free(c);
}

// Reads data from the client. And adds it to the buffer.
// returns 1 for success.
// returns 0 if the buffer is full.
// returns -1 if the conn was closed.
int _readDataFromClient(fidoClient* c) {

  int i, j;
  int bytesReadIn, bufferIsFull, bufferSize, overDraftIsEmpty;

  bufferSize = 2412;
  char readBuffer[bufferSize];
  char tmpBuffer;
  /* We only deal in 64-bit unsigned ints. */
  char intBuffer[8];
  char action;

  fido_message message;

  /* We're going to assume the client is ready for now. System calls
   * are expensive.
   */
  bufferIsFull = isMessageBufferFull(c->recvBuffer);
  while (!bufferIsFull) {
    overDraftIsEmpty = isMessageBufferFull(c->odRecvBuffer);
    if (!overDraftIsEmpty) {
      /* Uh oh, we have some leftover buffer items we need to process first. */
      message = getMessageFromClientBuffer(c)

    }
    /* Try to get everything */
    bytesReadIn = recv(c->fd, &readBuffer, bufferSize, 0);
    if (bytesReadIn <= bufferSize) {
      for (i = 0; i < bytesReadIn; i+=9) {
        for (j = 0; j < 9; j++) {
          if (i + j + 1 > bytesReadIn) {
            /* We don't have enough bytes on the buffer to fulfill this
             * message. Let's read in one byte at a time. This will slow down
             * the client, but they're obviously already slow. Maybe someone
             * else can rewrite this later to be faster. Basically, we need to
             * make sure no bits are lost.
             */
            bytesReadIn = recv(c->fd, &tmpBuffer, 1, 0);
            if (bytesReadIn != 1) {
              /* client closed, or catastrophic failure */
              return -1;
            }
            readBuffer[i+j] = tmpBuffer;
          }

          /* We have this bit buffered. */
          if (j < 8) {
            intBuffer[j] = readBuffer[i+j];
          } else {
            action = readBuffer[i+j];
          }
        }
        /* We have a single message. We need to add it to the buffer. */
        message.offset = (bit_array_num_t)intBuffer;
        message.action = action;

        if (bufferIsFull) {
          /* Uh oh, time to utilize our client's overdraft protection.
           * This is just a secondary buffer that's exactly the same as the
           * first one, but we will never fill it up, and it has to be drained
           * before we'll recv any more messages.
           */
          _addMessageToClientBuffer(c, message, '2');
        }
        _addMessageToClientBuffer(c, message, '1');
        bufferIsFull = isMessageBufferFull(c->recvBuffer);
      }
    } else if (bytesReadIn <= 0) {
      /* It would appear the client closed the connection. */
      return -1;
    }
  }
  return 1;
}

// Just creat an FD set with a single item in it. This is here as a helper
// function. select requires an fd_set to monitor.
fd_set createSingleFDSet(int* fd) {

  fd_set fds;
  FD_ZERO(&fds);

  FD_SET(fd, &fds);

  return &fds;

}

// This is a simple function that just does some of the grunt work of checking
// whether or not an FD is ready to be read form.
int isFDReady(int* fd, char rw) {

  int state;

  state = -1;

  fd_set* fds;
  fds = createSingleFDSet(fd);

  struct timeval no_time;
  no_time.tv_sec = 0;
  no_time.tv_usec = 0;

  switch (rw) {
    case 'r':
    case 'R':
      // Check for read-ability.
      state = select(*fd+1, fds, NULL, NULL, &no_time);
      break;
    case 'w':
    case 'W':
      // Check for write-ability.
      state = select(*fd+1, NULL, fds, NULL, &no_time);
    default:
      break;
  }

  return state;

}


