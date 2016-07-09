#ifndef RESP_H_   /* Include guard */
#define RESP_H_
#include "../ds/commands.h"

linkedList* respTokenizer(char *, unsigned long);
linkedList* simpleTokenizer(char *, unsigned long);

#endif