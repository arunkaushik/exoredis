#include "hashtable.h"

/*
* returns a new empty hashtable of given size.
* size is the number of buckets in hash_table. 
* Buckets are implemented via linked lists.
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
* gets a value from given hash_table with specific key
*/
exoVal* get(hashTable* ht, exoString* key){
    size_t str_hash = stringHash(key->buf, key->len);
    listNode *node = findNode(ht->buckets[str_hash], key);
    if(node){
        if(isExpired(node)){
            del(ht, key);
            return NULL;
        }
        return node->value;
    }
    return NULL;
}

/*
* sets a value in given hash_table with specified key, val pair
*/
exoVal* set(hashTable* ht, exoString* key, exoVal* val){
    size_t str_hash = stringHash(key->buf, key->len);
    listNode* node = findNode(ht->buckets[str_hash], key);
    if(node){
        return replaceNodeValue(node, key, val);
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
* sets a value in given hash_table with specified key, val pair when DB is restored from disk
*/
exoVal* setFromLoad(hashTable* ht, exoString* key, exoVal* val, uint64_t exp_ms){
    if(exp_ms < timeStamp()){
        return _OK();
    }
    size_t str_hash = stringHash(key->buf, key->len);
    listNode* node = newNode(key, val);
    if(addNodeToList(ht->buckets[str_hash], node)){
        node->expiry = exp_ms;
        return _OK();
    } else {
        return NULL;
    }
}

exoVal* setWithExpiry(hashTable* ht, exoString* key, exoVal* val, \
                        uint64_t exp_ms, bool nx, bool xx){
    uint64_t xp = timeStamp();
    size_t str_hash = stringHash(key->buf, key->len);
    listNode* node = findNode(ht->buckets[str_hash], key);

    if(node && isExpired(node)){
        freeListNode(node);
        node = NULL;
    }
    if(node){
        if(nx){
            return _Null();
        } else {
            node->expiry = exp_ms != 0 ? exp_ms + xp : DEFAULT_EXPIRY_MS;
            return replaceNodeValue(node, key, val);
        }
    } else {
        if(xx){
            return _Null();
        } else {
            node = newNode(key, val);
            node->expiry = exp_ms != 0 ? exp_ms + xp : DEFAULT_EXPIRY_MS;
            if(addNodeToList(ht->buckets[str_hash], node)){
                return _OK();
            } else {    
                return NULL;
            }
        }
    }
}

/*
deletes the entry from the given hash_table with specified key
It does not check for ds_type, simple remove the entry from table
*/
size_t del(hashTable* ht, exoString* key){
    bool expired;
    size_t str_hash = stringHash(key->buf, key->len);
    listNode* node = findNode(ht->buckets[str_hash], key);
    if(node){
        expired = isExpired(node);
        removeNodeFromList(ht->buckets[str_hash], node);
        freeListNode(node);
        return expired ? 0 : 1;
    }
    return 0;
}

/*
frees all the memory holded my hash_table
*/
void freeHashTable(hashTable* ht){
    if(ht == NULL) return;
    unsigned long i;
    for(i = 0; i < ht->size; i++){
        freeLinkedList(ht->buckets[i]);
    }
}
