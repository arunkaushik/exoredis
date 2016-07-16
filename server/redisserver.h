#include "../protocol/resp.h"
#include "../tcpsocket/soket.h"

exoString* commandDispatcher(linkedList*);
exoString* executeCommand(exoCmd*, linkedList*);
bool validArgs(size_t, unsigned long, bool);
void sig_handler(int);
int shutdownServer();
void actionBeforeExit();

int ConnectFD;
int SocketFD;