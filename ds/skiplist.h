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
exoVal* addToSortedSet(skipList*, bool, bool, bool, bool, linkedList*);
exoVal* addOrUpdateMember(skipList*, bool, bool, bool, listNode*, size_t);
exoVal* scoreIncrement(skipList*, listNode*, bool);
skipListNode* addNodeToSkiplist(skipList *, exoString*, long double);
void removeNodeFromSkipList(skipList *, exoString*, long double, skipListNode*);
size_t randomLevelRaiser();
void printSkipList(skipList*);
bool parseArgs(listNode* , long double [], exoString* []);
void parselongDouble(char *str);
unsigned long rankByScore(skipList*, long double, bool);
skipListNode* rankByOrder(skipList*, long long);