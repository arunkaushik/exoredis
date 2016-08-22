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
    bool free_args;
    struct exoVal *(*f_ptr)();
}exoCmd;

hashTable* initializeCmdTable();
exoCmd* addCommand(hashTable*, exoCmd*);
exoVal* getCommand(argList*);
exoVal* setCommand(argList*);
exoVal* pingCommand(argList*);
exoVal* flushCommand(argList*);
exoVal* getbitCommand(argList*);
exoVal* setbitCommand(argList*);
exoVal* delCommand(argList*);
exoVal* zaddCommand(argList*);
void parseSwitches(argList*, bool *, bool *, bool *, bool *);
exoVal* zcardCommand(argList*);
exoVal* zcountCommand(argList*);
exoVal* zrangeCommand(argList*);
exoVal* saveCommand();
exoVal* loadCommand();
bool parseMinMax(argListNode*, long double *, long double *);
int parseRange(argListNode*, long long *, long long *, bool*);

void writeHashMapToFile(listNode *, FILE *, char *);
void writeSkiplistToFile(listNode *, FILE *, char *);
void writeHBitMapToFile(listNode *, FILE *, char *);

int loadFromDB();
int loadHashMapEntry(FILE *, void *);
int loadSkiplistEntry(FILE *, void *);
int loadBitmapEntry();

#endif