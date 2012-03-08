#ifndef SOCKETZ_H
#define SOCKETZ_H

#define SOCKETZ_EE -1
#define SOCKETZ_OK 0

int socketAlloc();
void socketFree(int fd);
int socketWrite(int fd, char * buf, int bytesToWrite);
int socketRead(int fd, char * buf, int bytesToRead);
int serverAlloc(int port, char * ipAddr);
int serverListen(int fd, struct sockaddr * sa, socklen_t len);
int serverAccept(int fd, char * ipAddr, int * port);

#endif /* end of include guard: SOCKETZ_H */
