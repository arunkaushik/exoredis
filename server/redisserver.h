#include <netinet/in.h>
/* For socket functions */
#include <sys/socket.h>
/* For fcntl */
#include <fcntl.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include "protocol.h"

#define INPUT_BUFFER_SIZE 1024 * 1024
#define MAX_LINE 1024 * 1024

exoVal* commandDispatcher(argList*);
exoVal* executeCommand(exoCmd*, argList*);
bool validArgs(size_t, unsigned long, bool);
void sigHandler(int);
int shutdownServer();
void actionBeforeExit();
void readCallback(struct bufferevent *, void *);
void errorCallback(struct bufferevent *, short, void *);
void doAccept(evutil_socket_t , short, void *);
void writeToBuffer(exoVal *, struct evbuffer *);
void writeRespArrayToBuffer(exoVal *, struct evbuffer *);
void run();
int spinServer();
void freeGarbage();

int ConnectFD;
int SocketFD;
char* DEFAULT_FILE_PATH = "./data.rdb";
char* DB_FILE_PATH;
