#ifndef COMMANDS_H_   /* Include guard */
#define COMMANDS_H_
#include "bitmap.h"

typedef struct exoCmd{
    char* cmd_str;
    size_t ds_type;
    size_t args_count;
    bool variable_arg_count;
    struct exoVal *(*f_ptr)();
}exoCmd;

hashTable* initializeCmdTable();
exoCmd* addCommand(hashTable*, exoCmd*);
exoVal* getCommand(linkedList*);
exoVal* setCommand(linkedList*);
exoVal* pingCommand(linkedList*);
exoVal* flushCommand(linkedList*);
exoVal* getbitCommand(linkedList*);
exoVal* setbitCommand(linkedList*);

#endif