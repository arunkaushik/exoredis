#ifndef RESP_H_   /* Include guard */
#define RESP_H_
#include "../ds/commands.h"

argList* bufferTokenizer(char *, unsigned long);
argList* respTokenizer(char *, unsigned long);
argList* simpleTokenizer(char *, unsigned long);
unsigned long getRespInt(char *, char *, unsigned long);
unsigned long fileRespInt(char *str, char *start, unsigned long len);
argList* fileLineTokenizer(char *str, unsigned long len);

#endif