#ifndef HASHTABLE_H_   /* Include guard */
#define HASHTABLE_H_
#include "linkedlist.h"

typedef struct hashTable{
    unsigned long size;
    linkedList *buckets[];
}hashTable;

hashTable* newHashTable(size_t);
listNode* getListNode(hashTable*, exoString*);
exoVal* get(hashTable*, exoString*);
exoVal* set(hashTable*, exoString*, exoVal*);

#endif