#ifndef BITMAP_H_   /* Include guard */
#define BITMAP_H_
#include "hashtable.h"

typedef struct bitmapNode{
    void *mem;
    unsigned long len;
}bitmapNode;

typedef uint32_t word_t;
// assuming a char is made up of 8 bits
enum { BITS_PER_WORD = sizeof(word_t) * 8 };

bitmapNode* newBitmapNode();
bitmapNode* initBitmapNode(long long, bool);
bool getBit(bitmapNode*, long long);
bool setBit(bitmapNode*, long long, bool);
bool setbitUtility(void* , unsigned long);
bool clearbitUtility(void* , unsigned long);
bool getbitUtility(void* , unsigned long);
void freebitmapNode(bitmapNode*);
inline unsigned long word_offset(unsigned long);
inline size_t bit_offset(unsigned long);

#endif