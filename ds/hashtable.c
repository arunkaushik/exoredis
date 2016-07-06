#include "hashtable.h"

hashTable* newHashTable(size_t size){
    size_t i;
    hashTable* ht = (hashTable*)malloc(sizeof(linkedList) + size * sizeof(void *));
    if(ht){
        ht->size = 0;
        for(i = 0; i < size; i++){
            ht->buckets[i] = newLinkedList();
        }
    }
    return ht;
}

listNode* getListNode(hashTable* ht, exoString* key){
    size_t str_hash = stringHash(key->buf);
    return findNode(ht->buckets[str_hash], key);
}

exoVal* get(hashTable* ht, exoString* key){
    listNode *node = getListNode(ht, key);
    return node ? node->value : NULL;
}

int set(hashTable* ht, exoString* key, exoVal* val){
    size_t str_hash = stringHash(key->buf);
    listNode* node = getListNode(ht, key);
    if(node){
        return replaceNodeValue(node, val);
    } else {
        node = newNode(key, val);
        return addNodeToList(ht->buckets[str_hash], node);
    }
}

int main(){
    hashTable *table = newHashTable(INITIAL_SIZE);
    char key[1000], val[1000], f[1000];
    linkedList *list;
    exoString *k, *v, *find, *tmp_val;
    exoVal *value;
    unsigned long l;
    int n,t,i;
    scanf("%d %d", &n, &t);
    while(n--){
        scanf("%s", key);
        scanf("%s", val);
        l = strlen(key);
        k = newString((void *)key, l);
        l = strlen(val);
        v = newString((void *)val, l);
        value = newExoVal(EXOSTRING, (void *)v);
        set(table, k, value);
    }
    for(i = 0; i < INITIAL_SIZE; i++){
        printList(table->buckets[i]);
        printf("%s\n", "----------------");
    }
    printf("%s\n", "PRINTING AGAIN");

    for(i = 0; i < INITIAL_SIZE; i++){
        printList(table->buckets[i]);
        printf("%s\n", "----------------");
    }

    while(t--){
        scanf("%s", f);
        l = strlen(f);
        find = newString((void *)f, l);
        value = get(table, find);
        if(value){
            exoString *tmp_val = (exoString *)value->val_obj;
            printf("%s\n", tmp_val->buf);
        } else {
            printf("%s\n", "NOT FOUND" );
        }
    }

    printf("%s\n", "PRINTING AGAIN");
    
    for(i = 0; i < INITIAL_SIZE; i++){
        printList(table->buckets[i]);
        printf("%s\n", "----------------");
    }

    return 0;
}