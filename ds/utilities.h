#ifndef EXOSTRING_H_   /* Include guard */
#define EXOSTRING_H_
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <float.h>
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

typedef struct bitmapNode{
    void *mem;
    unsigned long len;
}bitmapNode;

exoString* newString(void *, unsigned long);
exoVal* newExoVal(size_t, void *);
void printString(exoString*);
size_t stringHash(char *);
void freeExoString(exoString*);
void freeExoVal(exoVal*);
exoString* upCase(exoString*);
void strUpCase(char *);
exoString* numberToString(unsigned long);
unsigned long stringToLong(char *);
int stringToBit(exoString *);
exoVal* returnError(int);
exoVal* returnNull();
exoVal* returnOK();
exoVal* returnPong();
void freebitmapNode(bitmapNode*);
long double stringTolongDouble(char *);
exoString* doubleToString(long double);

#endif