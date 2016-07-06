#ifndef LINKEDLIST_H_   /* Include guard */
#define LINKEDLIST_H_
#include "utilities.h"

typedef struct listNode{
    exoString *key;
    exoVal *value;
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
int addNodeToList(linkedList *, listNode *);
listNode* findNode(linkedList *, exoString *);
int replaceNodeValue(listNode *, exoVal *);
void printList(linkedList *);

#endif