#ifndef RESP_H_   /* Include guard */
#define RESP_H_
#include "../ds/commands.h"

linkedList* bufferTokenizer(char *, unsigned long);
linkedList* respTokenizer(char *, unsigned long);
linkedList* simpleTokenizer(char *, unsigned long);
unsigned long getRespInt(char *, char *, unsigned long);

#endif