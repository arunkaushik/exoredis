#include "bitmap.h"

/*
Returns an empty bitmap node
*/
bitmapNode* newBitmapNode(){
    bitmapNode* node = NULL;
    node = malloc(sizeof(bitmapNode));
    if(node){
        node->len = 0;
    }
    return node;
}

/*
Initialize a bitmap node. Expands the memory to hold bit at given position
*/
bitmapNode* initBitmapNode(unsigned long pos, bool bit){
    unsigned long len = word_offset(pos);
    bitmapNode* node = NULL;
    node = newBitmapNode();
    if(node){
        node->mem = malloc(sizeof(unsigned long) * (len + 1));
        if(node->mem){
            memset(node->mem, 0, sizeof(node->mem));
            node->len = len + 1;
            bit ? setbitUtility(node->mem, pos) : clearbitUtility(node->mem, pos);
            return node;
        } else {
            free(node);
        }
    }
    return node;
}

/*
returns the bit stored in node at given position
*/
bool getBit(bitmapNode* node, unsigned long pos){
    printf(RED "GETBIT CALLED ::::::::::::::\n" RESET);
    if(pos < node->len * BITS_PER_WORD){
        return getbitUtility(node->mem, pos);
    } else {
        return false;
    }
}

/*
sets the bit stored in node at given position to value passed in bit(0 or 1)
*/
bool setBit(bitmapNode* node, unsigned long pos, bool bit){
    printf(RED "SETBIT CALLED\n" RESET);
    unsigned long len = word_offset(pos);
    void *new_start;
    bool ret;
    if(pos < node->len * BITS_PER_WORD){
        ret = getbitUtility(node->mem, pos);
        bit ? setbitUtility(node->mem, pos) : clearbitUtility(node->mem, pos);
    } else {
        node->mem = realloc(node->mem, sizeof(unsigned long) * (len + 1));
        if(node->mem){
            new_start = node->mem + node->len;
            memset(new_start, 0, len + 1 - node->len);
            node->len = len + 1;
            ret = false;
            bit ? setbitUtility(node->mem, pos) : clearbitUtility(node->mem, pos);
        } else {
            // have to return err
        }
    }
    return ret;
}

/*
utility funtion used to set bit
*/
bool setbitUtility(void* mem, unsigned long pos){
    unsigned long *ptr = mem;
    ptr += word_offset(pos);
    *ptr |= (1 << bit_offset(pos));
    return true;
}

/*
utility funtion used to clear bit (make bit = 0)
*/
bool clearbitUtility(void* mem, unsigned long pos){
    unsigned long *ptr = mem;
    ptr += word_offset(pos);
    *ptr &= ~(1 << bit_offset(pos));
    return true;
}

/*
utility funtion used to get bit at pos
*/
bool getbitUtility(void* mem, unsigned long pos){
    unsigned long *ptr = mem;
    ptr += word_offset(pos);
    return 0 != (*ptr & (1 << bit_offset(pos)));
}

/*
utility functions for bit position calculations
*/
unsigned long word_offset(unsigned long pos){
    return pos/BITS_PER_WORD;
}
size_t bit_offset(unsigned long pos){
    return pos%BITS_PER_WORD;
}