#ifndef EXOSTRING_H_   /* Include guard */
#define EXOSTRING_H_
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "constants.h"

typedef struct exoString{
    unsigned long len;
    char buf[];
}exoString;

typedef struct exoVal{
    size_t ds_type;
    void *val_obj;
}exoVal;

exoString* newString(void *, unsigned long);
exoVal* newExoVal(size_t, void *);
void printString(exoString*);
size_t stringHash(char *);
void freeExoString(exoString*);
void freeExoVal(exoVal*);
exoString* upCase(exoString*);
void strUpCase(char *);
exoString* returnError(int);
exoString* returnNull();
exoString* returnOK();

#endif