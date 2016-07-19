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
#include "../protocol/resp.h"
//#include "../tcpsocket/soket.h"

#define MAX_LINE 16384

exoVal* commandDispatcher(linkedList*);
exoVal* executeCommand(exoCmd*, linkedList*);
bool validArgs(size_t, unsigned long, bool);
void sig_handler(int);
int shutdownServer();
void actionBeforeExit();
void readcb(struct bufferevent *, void *);
void errorcb(struct bufferevent *, short, void *);
void do_accept(evutil_socket_t , short, void *);
void writeToBuffer(exoVal *, struct evbuffer *);
void run();
int spinServer();

int ConnectFD;
int SocketFD;