#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "fido/socketz.h"

int socketSetNonBlock ( int fd )
{
  int flags = fcntl( fd, F_GETFL );
  if ( flags == -1 ) {
    return SOCKETZ_EE;
  }
  if ( fcntl( fd, F_SETFL, flags | O_NONBLOCK ) == -1 ) {
    return SOCKETZ_EE;
  }
  return SOCKETZ_OK;
}

int socketSetKeepAlive ( int fd )
{
  int yes = 1;
  if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(yes)) == -1) {
    return SOCKETZ_EE;
  }
  return SOCKETZ_OK;
}

int socketSetNoDelay ( int fd )
{
  int yes = 1;
  if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes)) == -1) {
    return SOCKETZ_EE;
  }
  return SOCKETZ_OK;
}

int socketSetReuseAddr ( int fd )
{
  int yes = 1;
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
    return SOCKETZ_EE;
  }
  return SOCKETZ_OK;
}

short convPort ( int port )
{
  return htons(port);
}

long convAddr ( char * ipAddr )
{
  struct sockaddr_in sa;
  memset( &sa, 0, sizeof( sa ) );

  if ( inet_aton( ipAddr, &sa.sin_addr ) < 1 ) {
    return SOCKETZ_EE;
  }
  return sa.sin_addr.s_addr;
}

struct sockaddr_in sockaddrAlloc ( char * ipAddr, int port )
{
  struct sockaddr_in sa;
  long nboAddr;

  memset( &sa, 0, sizeof( sa ) );

  sa.sin_family = AF_INET;
  sa.sin_port = convPort( port );
  sa.sin_addr.s_addr = INADDR_ANY;

  if ( ( nboAddr = convAddr( ipAddr ) ) != SOCKETZ_EE ) {
    sa.sin_addr.s_addr = nboAddr;
  }

  return sa;
}

int socketAlloc ()
{
  int fd;

  fd = socket( AF_INET, SOCK_STREAM, 0 );
  if ( fd == -1 ) {
    return SOCKETZ_EE;
  }

  return fd;
}

void socketFree( int fd )
{
  shutdown( fd, SHUT_WR );
  close( fd );
}

int socketWrite ( int fd, char * buf, int bytesToWrite )
{
  int bytesWritten, totalBytesWritten = 0;

  while ( totalBytesWritten != bytesToWrite ) {
    bytesWritten = write( fd, buf, bytesToWrite - totalBytesWritten );
    if ( bytesWritten == 0 ) return totalBytesWritten;
    if ( bytesWritten == -1 ) return SOCKETZ_EE;
    totalBytesWritten += bytesWritten;
    buf += bytesWritten;
  }
  return totalBytesWritten;
}

int socketRead ( int fd, char * buf, int bytesToRead )
{
  int bytesRead, totalBytesRead = 0;

  while ( totalBytesRead != bytesToRead ) {
    bytesRead = read( fd, buf, bytesToRead - totalBytesRead );
    if ( bytesRead == 0 ) return totalBytesRead;
    if ( bytesRead == -1 ) return SOCKETZ_EE;
    totalBytesRead += bytesRead;
    buf += bytesRead;
  }
  return totalBytesRead;
}

int serverAlloc ( int port, char * ipAddr )
{
  int fd;
  struct sockaddr_in sa;

  fd = socketAlloc();
  if ( fd == SOCKETZ_EE ) {
    return SOCKETZ_EE;
  }

  // Standard stuff.
  socketSetReuseAddr( fd );
  socketSetKeepAlive( fd );

  // ASYNC!!
  socketSetNonBlock( fd );
  socketSetNoDelay( fd );

  // Bind and listen
  sa = sockaddrAlloc( ipAddr, port );
  serverListen( fd, ( struct sockaddr * )&sa, (socklen_t)sizeof( sa ) );

  return fd;
}

int serverListen ( int fd, struct sockaddr * sa, socklen_t len )
{
  if ( bind( fd, sa, len ) == -1 ) {
    socketFree( fd );
    return SOCKETZ_EE;
  }
  if ( listen( fd, 511 ) == -1 ) { /* the magic 511 constant is from nginx */
    socketFree( fd );
    return SOCKETZ_EE;
  }
  return SOCKETZ_OK;
}

int serverAccept ( int fd, char * ipAddr, int * port )
{
  int cfd;
  struct sockaddr_in sa;
  socklen_t len = sizeof(sa);

  while ( 1 ) {
    cfd = accept( fd, ( struct sockaddr* )&sa, &len );
    if ( cfd == -1 ) {
      if ( errno == EINTR ) {
        continue;
      } else {
        return SOCKETZ_EE;
      }
    }
    break;
  }

  if ( ipAddr ) strcpy( ipAddr, inet_ntoa( sa.sin_addr ) );
  if ( port ) *port = ntohs( sa.sin_port );
  return cfd;
}
