#ifndef SOKET_H_   /* Include guard */
#define SOKET_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int createSocket(int port); // Returns socket connection on given port
void makeSocketKeepAlive(int socket);
int shutdownSocketConnection(int ConnectFD, int SocketFD);

#endif // SOKET_H_