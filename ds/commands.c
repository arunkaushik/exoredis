#include "commands.h"

/*
array of exoCmds. exoCmd is the data_type that holds all the information 
of the APIs exposed to clients, like GET, SET, etc. 
It also folds the function pointers to which command execution shall be dispatched
*/
struct exoCmd commandTable[] = {
  {"GET", BULKSTRING, 1, false, getCommand},
  {"SET", BULKSTRING, 2, true, setCommand},
  {"PING", SIMPLE_STRING, 0, false, pingCommand},
  {"FLUSHALL", SIMPLE_STRING, 0, false, flushCommand}
};

/*
Initialized a hash_table which stores all the APIs.
key is the upcased string of command name, like SET for set and GET for get.
All APIs commands are stored in a hash_table. When a requests comes, the command 
is searched in this hash_table. If the command is found, it holds the function pointer
to which the task shall be dispatched. If the command is not found, then that command
is not supported by exoRedis
*/
hashTable* initializeCmdTable(){
    printf(CYN "initialized COMMANDS\n" RESET);
    hashTable* commands = newHashTable(INITIAL_SIZE);
    int i,n = sizeof(commandTable)/sizeof(struct exoCmd);
    for (i = 0; i < n; i++) {
        exoCmd *c = commandTable+i;
        if(addCommand(commands, c) == NULL){
        return NULL;
        }
    }
    return commands;
}

/*
    adds a command in the command hash_table. Utility function used by initializeCmdTable
*/
exoCmd* addCommand(hashTable *ht, exoCmd* cmd){
    unsigned long l = strlen(cmd->cmd_str);
    size_t key_hash = stringHash(cmd->cmd_str);

    exoString* cmd_str = newString(cmd->cmd_str, l);
    exoVal* exo_cmd = newExoVal(EXOCMD, cmd);

    listNode* cmd_node = newNode(cmd_str, exo_cmd);

    if(addNodeToList(ht->buckets[key_hash], cmd_node)){
        return cmd;
        printf(CYN "Command added to table\n" RESET);
    } else {
        printf(CYN "Command NOT added to table\n" RESET);
        return NULL;
    }
}

/*
Entry point for GET api
*/
exoVal* getCommand(linkedList* args){
    printf(CYN "getCommand Called\n" RESET);
    listNode* node = args->head;

    // head holds command node, actual args starts from head->next
    exoVal* val = get(HASH_TABLE, node->next->key);
    if(val && val->ds_type == BULKSTRING){
        return val;
    } else if(val && val->ds_type != BULKSTRING) {
        return returnError(WRONG_TYPE_OF_COMMAND_ON_TARGET_OBJECT);
    } else {
        return returnNull();
    }
}

/*
Entry point of SET api.
SET command overwirtes the target value if it exists, 
So no need to check the ds_type of target object. Simply over-write
*/
exoVal* setCommand(linkedList* args){
    printf(CYN "setCommand Called\n" RESET);
    listNode* node = args->head->next;
    exoString* key;
    exoVal* val;
    while(node){
        key = node->key;
        val = newExoVal(BULKSTRING, node->next->key);
        val = set(HASH_TABLE, key, val);
        if(!val){
            return returnNull();
        }
        node = node->next->next;
    }
    return returnOK();
}

exoVal* pingCommand(linkedList* args){
    return returnPong();
}

exoVal* flushCommand(linkedList* args){
    hashTable *new_table = newHashTable(INITIAL_SIZE);
    if(new_table){
        freeHashTable(HASH_TABLE);
        HASH_TABLE = new_table;
        return returnOK();
    } else {
        return returnError(FAILED_TO_FLUSH_DB);
    } 
}