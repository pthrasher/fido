/*
    Copyright 2012 Philip Thrasher
    Distributed under the MIT License (see accompanying file LICENSE
    or a copy at http://www.opensource.org/licenses/MIT
*/
#include "fido.h"

void initAddr(struct sockaddr_in* addr, uint32_t nboAddress, uint32_t nboPort) {
  // Basic socket settings
  addr->sin_family = AF_INET;
  addr->sin_port = nboPort;
  addr->sin_addr.s_addr = nboAddress;

  // Clean that memory out. Not sure what's in there.
  memset(&(addr->sin_zero), 0, 8);
}


int socketSetNonBlock(int fd) {
  /* Set the socket nonblocking.
   * Note that fcntl(2) for F_GETFL and F_SETFL can't be
   * interrupted by a signal.
   */

  int flags = fcntl(fd, F_GETFL);
  if (flags == -1) {
    return -1;
  }
  if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
    return -1;
  }
  return 1;
}

int socketSetTcpKeepAlive(int fd) {
  int yes = 1;
  if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(yes)) == -1) {
    return -1;
  }
  return 1;
}

int socketSetTcpNoDelay(int fd) {
  int yes = 1;
  if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes)) == -1) {
    return -1;
  }
  return 1;
}

int newAsyncSocket() {
  int fd;
  int yes = 1;

  fd = socket(AF_INET, SOCK_STREAM, 0);
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
  socketSetTcpKeepAlive(fd);
  socketSetTcpNoDelay(fd);
  socketSetNonBlock(fd);

  return fd;
}

fidoServer createServer(uint32_t nboAddress, uint32_t nboPort, bitmap_t* bits) {
  int yes = 1;
  fidoServer _server;
  _server.fd = newAsyncSocket();

  // Initialize the server's sockaddr_in
  initAddr(&(_server.addr), nboAddress, nboPort);

  // Global Server settings
  _server.bits = bits;

  // Shouldn't be greater than max fd's.
  _server.maxClients = 1024;

  // Internal counters and stats.
  _server.numClients = 0;
  _server.numRequests = 0;

  return _server;
}

int runServer(fidoServer* _server) {
  int result;

  result = bind(_server->fd, (struct sockaddr*)&(_server->addr), sizeof(_server->addr));
  if (result == -1) return 0;

  result = listen(_server->fd, 10);
  if (result == -1) return 0;

  return 1;
}

int sendall(int s, void *buf, int *len) {
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while(total < *len) {
        n = send(s, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
}
