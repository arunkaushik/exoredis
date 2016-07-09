#include "soket.h"

void makeSocketKeepAlive(int socket){
  int optval;
  socklen_t optlen = sizeof(optval);
  if(getsockopt(socket, SOL_SOCKET, SO_KEEPALIVE, &optval, &optlen) < 0) {
      perror("getsockopt()");
      close(socket);
      exit(EXIT_FAILURE);
   }
   printf("SO_KEEPALIVE is %s\n", (optval ? "ON" : "OFF"));

   /* Set the option active */
   optval = 1;
   optlen = sizeof(optval);
   if(setsockopt(socket, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) < 0) {
      perror("setsockopt()");
      close(socket);
      exit(EXIT_FAILURE);
   }
   printf("SO_KEEPALIVE set on socket\n");

   /* Check the status again */
   if(getsockopt(socket, SOL_SOCKET, SO_KEEPALIVE, &optval, &optlen) < 0) {
      perror("getsockopt()");
      close(socket);
      exit(EXIT_FAILURE);
   }
   printf("SO_KEEPALIVE is %s\n", (optval ? "ON" : "OFF"));
}

int createSocket(int port) {
  struct sockaddr_in sa;

  int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (-1 == SocketFD) {
    perror("cannot create socket");
    return -1;
  }

  memset(&sa, 0, sizeof sa);

  makeSocketKeepAlive(SocketFD);

  sa.sin_family = AF_INET;
  sa.sin_port = htons(port);
  sa.sin_addr.s_addr = htonl(INADDR_ANY);

  if (-1 == bind(SocketFD,(struct sockaddr *)&sa, sizeof sa)) {
    perror("bind failed");
    close(SocketFD);
    return -1;
  }

  if (-1 == listen(SocketFD, 10)) {
    perror("listen failed");
    close(SocketFD);
    return -1;
  }

  int ConnectFD = accept(SocketFD, NULL, NULL);

  if (0 > ConnectFD) {
    perror("accept failed");
    close(SocketFD);
    return -1;
  }

  return ConnectFD;
}

int shutdownSocketConnection(int ConnectFD, int SocketFD){
  if (-1 == shutdown(ConnectFD, SHUT_RDWR)) {
    perror("shutdown failed");
    close(ConnectFD);
    close(SocketFD);
    return -1;
  }
  close(ConnectFD);
  close(SocketFD);
  return 0;
}