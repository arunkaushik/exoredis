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
bitmapNode* initBitmapNode(unsigned long, bool);
bool getBit(bitmapNode*, unsigned long);
bool setBit(bitmapNode*, unsigned long, bool);
bool setbitUtility(void* , unsigned long);
bool clearbitUtility(void* , unsigned long);
bool getbitUtility(void* , unsigned long);
inline unsigned long word_offset(unsigned long);
inline size_t bit_offset(unsigned long);

#endif