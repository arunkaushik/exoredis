#ifndef EXOSTRING_H_   /* Include guard */
#define EXOSTRING_H_
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <float.h>
#include <sys/time.h>
#include "constants.h"

#define MAX( a, b ) ( ( a > b) ? a : b )
#define MIN( a, b ) ( ( a < b) ? a : b )

typedef struct exoString{
    unsigned long len;
    char buf[];
}exoString;

typedef struct exoVal{
    size_t ds_type;
    void *val_obj;
}exoVal;

typedef struct argListNode{
    exoString *key;
    bool dead;
    struct argListNode *next;
}argListNode;

typedef struct argList{
    unsigned long size;
    argListNode *head;
    argListNode *tail;
}argList;

typedef struct garbageNode{
    exoVal *garbage;
    struct garbageNode *next;
}garbageNode;

typedef struct garbageList{
    garbageNode *head;
    garbageNode *tail;
}garbageList;

exoString* newString(void *, unsigned long);
exoVal* newExoVal(size_t, void *);
void printString(exoString*);
size_t stringHash(char *, unsigned long);
void freeExoString(exoString*);
void freeExoVal(exoVal*);
exoString* upCase(exoString*);
void strUpCase(char *);
exoString* numberToString(unsigned long);
exoString* llToString(uint64_t);
long long stringToLongLong(char *);
int stringToBit(exoString *);
exoVal* _Error(int);
exoVal* _Null();
exoVal* _OK();
exoVal* _Pong();
long double stringTolongDouble(char *);
exoString* doubleToString(long double);
uint64_t timeStamp();

argList* newArgList();
argListNode* newArg(exoString *);
argListNode* addArgToList(argList *, argListNode *);
void freeDeadArgs(argList *);
void freeAllArgs(argList *);
void freeArgNode(argListNode *);
void setAllDead(argList *);

garbageList* newGarbageList();
garbageNode* newGarbageNode(exoVal *);
garbageNode* addGarbageToList(garbageList *, garbageNode *);

garbageList *GARBAGE_LIST;
#endif