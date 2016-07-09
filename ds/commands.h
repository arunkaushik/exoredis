#ifndef COMMANDS_H_   /* Include guard */
#define COMMANDS_H_
#include "hashtable.h"

typedef struct exoCmd{
    char* cmd_str;
    size_t ds_type;
    size_t args_count;
    bool variable_arg_count;
    struct exoString *(*f_ptr)();
}exoCmd;

hashTable* initializeCmdTable();
exoCmd* addCommand(hashTable*, exoCmd*);
exoString* getCommand(linkedList*);
exoString* setCommand(linkedList*);

#endif