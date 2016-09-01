#ifndef HASHTABLE_H_   /* Include guard */
#define HASHTABLE_H_
#include "linkedlist.h"

typedef struct hashTable{
    unsigned long size;
    linkedList *buckets[];
}hashTable;

hashTable* newHashTable(size_t);
exoVal* get(hashTable*, exoString*);
exoVal* set(hashTable*, exoString*, exoVal*);
exoVal* setFromLoad(hashTable*, exoString*, exoVal*, uint64_t);
exoVal* setWithExpiry(hashTable*, exoString*, exoVal*, uint64_t, bool, bool);
size_t del(hashTable*, exoString*);
void freeHashTable(hashTable*);

hashTable *COMMANDS;
hashTable *HASH_TABLE;

#endif