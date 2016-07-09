#include "hashtable.h"

/*
returns a new empty hashtable of given size.
size is the number of buckets in hash_table. 
Buckets are implemented via linked lists.
*/
hashTable* newHashTable(size_t size){
    size_t i;
    hashTable* ht = (hashTable*)malloc(sizeof(linkedList) + size * sizeof(void *));
    if(ht){
        ht->size = size;
        for(i = 0; i < size; i++){
            ht->buckets[i] = newLinkedList();
        }
    }
    return ht;
}

/*
gets a value from given hash_table with specific key
*/
exoVal* get(hashTable* ht, exoString* key){
    printf(RED "get Called\n" RESET);
    size_t str_hash = stringHash(key->buf);
    listNode *node = findNode(ht->buckets[str_hash], key);
    return node ? node->value : NULL;
}

/*
sets a value in given hash_table with specified key, val pair
*/
exoVal* set(hashTable* ht, exoString* key, exoVal* val){
    printf(RED "set Called\n" RESET);
    size_t str_hash = stringHash(key->buf);
    listNode* node = findNode(ht->buckets[str_hash], key);
    if(node){
        return replaceNodeValue(node, val);
    } else {
        node = newNode(key, val);
        if(addNodeToList(ht->buckets[str_hash], node)){
            return val;
        } else {
            return NULL;
        }
    }
}

/*
frees all the memory holded my hash_table
*/
void freeHashTable(hashTable* ht){
    unsigned long i;
    for(i = 0; i < ht->size; i++){
        freeLinkedList(ht->buckets[i]);
    }
}
