#ifndef LINKEDLIST_H_   /* Include guard */
#define LINKEDLIST_H_
#include "utilities.h"

typedef struct listNode{
    exoString *key;
    exoVal *value;
    uint64_t expiry;
    struct listNode *next;
    struct listNode *prev;
}listNode;

typedef struct linkedList{
    listNode *head;
    listNode *tail;
    unsigned long size;
}linkedList;

linkedList* newLinkedList();
listNode* newNode(exoString *, exoVal *);
listNode * addNodeToList(linkedList *, listNode *);
listNode* removeNodeFromList(linkedList *, listNode *);
listNode* findNode(linkedList *, exoString *);
exoVal* replaceNodeValue(listNode *, exoString *, exoVal *);
void printList(linkedList *);
void freeListNode(listNode *);
void freeLinkedList(linkedList*);
bool isExpired(listNode *);
#endif