#include "redisserver.h"

int main(){
  unsigned long bytesread;
  exoString *result;
  unsigned long buffSize = 1000;
  char buf[buffSize];
  linkedList* tokens = NULL;

  // binding save method on exit signal
  signal(SIGINT, sig_handler);

  ConnectFD = createSocket(PORT);
  if (ConnectFD == -1) {
    perror("Socket Creation Failed");
    return (EXIT_FAILURE);
  }

  /*
    Initialize command table hash in COMMANDS global var
    It contains all the API exposed to clients 
  */
    COMMANDS = initializeCmdTable();

    HASH_TABLE = newHashTable(INITIAL_SIZE);

    while(1){
        bytesread = read(ConnectFD, buf, buffSize);

        // have to implement RESP parser. Intelligence to choose
        // right parser based on the string read from client
        tokens = simpleTokenizer(buf, bytesread-1);

        result = commandDispatcher(tokens);

        if(result){
            result->buf[result->len] = '\n';
            if(write(ConnectFD,result->buf,result->len+1) < 0 ){
                perror("ERROR writing to socket");
            }
        }
    }

    return EXIT_SUCCESS; 
}

/*
Finds the command fired in the command hash_table.
If found, task is dispatched to the found command else
returns COMMAND NOT FOUND err
*/
exoString* commandDispatcher(linkedList* tokens){
    if(tokens && tokens->size){
        exoString* cmd_str = tokens->head->key;
        exoVal* node = get(COMMANDS, upCase(cmd_str));
        exoCmd* cmd;
        if(node){
            cmd = (exoCmd*)node->val_obj;
            return executeCommand(cmd, tokens);
        } else {
            return returnError(COMMAND_NOT_FOUND);
        }
    }
    return NULL;
}

/*
Checks if the passed args are compliant to the end API.
If compliant, it sends of the args to the function pointer
store in the exoCmd struct. Else it retrun WRONG_NUMBER_OF_ARGUMENTS err
*/
exoString* executeCommand(exoCmd* cmd, linkedList* tokens){
    if(validArgs(cmd->args_count, tokens->size -1)){
        return cmd->f_ptr(tokens);
    } else {
        returnError(WRONG_NUMBER_OF_ARGUMENTS);
    }
    return returnNull();
}

/*
Utility function used by executeCommand
*/
bool validArgs(size_t arg_count, unsigned long args_passed){
    printf("%s %zu %lu\n", WHT "validArgs called with: " RESET, arg_count, args_passed);
    if(arg_count == args_passed){
        return true;
    } else if(args_passed){
        return args_passed % arg_count ? false : true;
    }
    return false;
}

/*
Function binded to inturrupt signal. It is excuted when user hits ctrl+C
*/
void sig_handler(int signo) {
    if(signo == SIGINT){
        printf("received SIGINT\n");
        if(-1 == shutdownServer()){
            perror("Cannot shutdown server properly.");
        } else {
            actionBeforeExit();
            printf("%s\n", "Server shutdown successfully.");
        }
    }
    exit(0);
}

/*
It shuts down the server after writing all the stored data in rdb file for persistence.
*/
int shutdownServer(){
    printf(" %s\n","Shutting Down server...");
    return shutdownSocketConnection(ConnectFD, SocketFD);
}

/*
Function called by shutdownServer() for persistence.
It frees all the memory used by server after writing the rdb file. 
*/
void actionBeforeExit(){
    printf("%s\n","I will do some work before exit.");
    freeHashTable(COMMANDS);
    freeHashTable(HASH_TABLE);
    return;
}