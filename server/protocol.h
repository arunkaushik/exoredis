#ifndef PROTOCOL_H_   /* Include guard */
#define PROTOCOL_H_
#include "../ds/commands.h"

argList* bufferTokenizer(char *, unsigned long);
argList* respTokenizer(char *, unsigned long);
argList* simpleTokenizer(char *, unsigned long);
unsigned long getRespInt(char *, char *, unsigned long);

#endif