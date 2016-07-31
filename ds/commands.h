#ifndef COMMANDS_H_   /* Include guard */
#define COMMANDS_H_
#include "bitmap.h"
#include "skiplist.h"

typedef struct exoCmd{
    char* cmd_str;
    size_t ds_type;
    size_t args_count;
    bool variable_arg_count;
    bool skip_arg_test;
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
exoVal* delCommand(linkedList*);
exoVal* zaddCommand(linkedList*);
void parseSwitches(linkedList*, bool *, bool *, bool *, bool *);
exoVal* zcardCommand(linkedList*);
exoVal* zcountCommand(linkedList*);
bool parseMinMax(listNode*, long double *, long double *);

#endif