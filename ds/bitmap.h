#ifndef BITMAP_H_   /* Include guard */
#define BITMAP_H_
#include "hashtable.h"

/*
* setting below macro as -> sizeof(unsigned long)*8
* results in painfully ugly behaviour. Have to dig.
* Hardcoding it to 64 since unsigned long is 8 bytes long
*/
#define BITS_PER_WORD 64

bitmapNode* newBitmapNode();
bitmapNode* initBitmapNode(long long, bool);
bool getBit(bitmapNode*, long long);
bool setBit(bitmapNode*, long long, bool);
bool setbitUtility(void* , long long);
bool clearbitUtility(void* , long long);
bool getbitUtility(void* , long long);
inline unsigned long word_offset(long long);
inline size_t bit_offset(long long);

#endif