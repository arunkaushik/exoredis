#include "hashtable.h"

typedef struct skipListNode{
    exoString *key;
    long double score;
    size_t children;
    struct skipListNode *next;
    struct skipListNode *down;
}skipListNode;

typedef struct skipList{
    skipListNode *head;
    unsigned long size;
    size_t levels;
    hashTable* table;
}skipList;

skipList* newSkipList();
skipListNode* newSkipListNode(exoString*, long double);
exoVal* addToSortedSet(skipList*, bool, bool, bool, bool, argList*);
exoVal* addOrUpdateMember(skipList*, bool, bool, bool, argListNode*, size_t);
exoVal* scoreIncrement(skipList*, argListNode*, bool);
skipListNode* addNodeToSkiplist(skipList *, exoString*, long double);
void removeNodeFromSkipList(skipList *, exoString*, long double, skipListNode*);
size_t randomLevelRaiser();
void printSkipList(skipList*);
bool parseArgs(argListNode* , long double [], exoString* []);
void parselongDouble(char *str);
unsigned long rankByScore(skipList*, long double, bool);
skipListNode* rankByOrder(skipList*, long long);
void freeSkipList(skipList* );
void freeSkipListNode(skipListNode *);