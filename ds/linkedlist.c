#include "linkedlist.h"

/*
* returns a new empty linked list
*/
linkedList* newLinkedList(){
    linkedList* ll = (linkedList*)malloc(sizeof(linkedList));
    if(ll){
        ll->head = NULL;
        ll->tail = NULL;
        ll->size = 0;
    }
    return ll;
}

/*
* return a new list node with given key value pair
*/
listNode* newNode(exoString *key, exoVal *value){
    listNode* tmp = (listNode*)malloc(sizeof( listNode));
    if(tmp){
        tmp->key = key;
        tmp->value = value;
        tmp->expiry = DEFAULT_EXPIRY_MS;
        tmp->next = NULL;
        tmp->prev = NULL;
    }
    return tmp;
}

/*
* adds list node to the given list
*/
listNode* addNodeToList(linkedList *list, listNode *node){
    if(list == NULL || node == NULL){
        return NULL;
    } else {
        if(list->head){
            list->tail->next = node;
            node->prev = list->tail;
            list->tail = list->tail->next;
        } else {
            list->head = node;
            list->tail = list->head;
        }
        list->size++;
        return node;
    } 
}

/*
* Remove and returns the node from the list. It does not frees the memory of the node.
* Freeing memory to be handled by the calling function
*/
listNode* removeNodeFromList(linkedList *list, listNode *node){
    if(list == NULL || node == NULL){
        return NULL;
    } else {
        if(list->size == 1){
            list->head = NULL;
            list->tail = NULL;
        } else {
            if(node->prev == NULL){ // head node
                node->next->prev = NULL;
                list->head = node->next;
            } else if(node->next == NULL) { // tail node
                node->prev->next = NULL;
                list->tail = node->prev;
            } else {
                node->prev->next = node->next;
                node->next->prev = node->prev;
            }
        }
        list->size--;
        return node;
    } 
}

/*
* prints the list with its size to STDOUT
* Values are printed only if they are exoString, otherwise only key is printed
*/
void printList(linkedList *list){
    listNode *tmp = list->head;
    exoString *tmp_val;
    printf("%s %lu\n","Size: ", list->size);
    while(tmp){
        if(tmp->value->ds_type == BULKSTRING){
            exoString *tmp_val = (exoString *)tmp->value->val_obj;
            printf("%s %s\n", tmp->key->buf, tmp_val->buf);
        } else {
            printf("%s\n", tmp->key->buf);
        }
        tmp = tmp->next;
    }
        free(tmp);
}

/*
* finds a node with specific key in given list 
*/
listNode* findNode(linkedList *list, exoString *key){
    listNode* tmp = list->head;
    while(tmp){
        if(tmp->key->len ==  key->len && !memcmp(tmp->key->buf, key->buf, key->len)){
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}

/*
* replaces the value in a given list node with new value.
* value is a object of exoVal data type
*/
exoVal* replaceNodeValue(listNode *node, exoString *newkey, exoVal *newval){
    if(node == NULL || newval == NULL){
        return NULL;
    } else {
        exoVal *tmp;
        exoString *tmp_k;
        tmp = node->value;
        tmp_k = node->key;
        node->key = newkey;
        node->value = newval;
        
        freeExoString(tmp_k);
        freeExoVal(tmp);
        return _OK();
    }
}

/*
* frees all the memory holded my given linkedlist
*/
void freeLinkedList(linkedList* list){
    if(list == NULL) return;
    listNode* node = list->tail;
    listNode* tmp;
    while(node){
        tmp = node->prev;
        freeListNode(node);
        node = tmp;
    }
    free(list);
}

/*
* frees the memory holded by given listnode
*/
void freeListNode(listNode *node){
    if(node == NULL) return;
    freeExoString(node->key);
    freeExoVal(node->value);
    free(node);
}

/*
* Returns true if node has expired, false otherwise
*/
bool isExpired(listNode *node){
    exoVal *v = (exoVal*)node->value;
    if(v->ds_type != BULKSTRING){
        return false;
    }
    return node->expiry < timeStamp();
}