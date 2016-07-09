#include "linkedlist.h"

/*
returns a new empty linked list
*/
linkedList* newLinkedList(){
    linkedList* ll = (linkedList*)malloc(sizeof(linkedList));
    if(ll){
        ll->head = NULL;
        ll->tail = NULL;
        ll->size = 0;
    }
    printf(GRN "new linkedList created\n" RESET);
    return ll;
}

/*
return a new list node with given key value pair
*/
listNode* newNode(exoString *key, exoVal *value){
    listNode* tmp = (listNode*)malloc(sizeof( listNode));
    if(tmp){
        tmp->key = key;
        tmp->value = value;
        tmp->next = NULL;
        tmp->prev = NULL;
    }
    printf("%s %s\n", GRN "new listNode created with key: " RESET, key->buf);
    return tmp;
}

/*
adds list node to the given list
*/
listNode* addNodeToList(linkedList *list, listNode *node){
    if(list == NULL || node == NULL){
        return NULL;
        printf(GRN "node NOT added to List\n" RESET);
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
        printf("%s %s\n", GRN "node with key added to List : " RESET, node->key->buf);
        return node;
    } 
}

/*
prints the list with its size to STDOUT
Values are printed only if they are exoString, otherwise only key is printed
*/
void printList(linkedList *list){
    listNode *tmp = list->head;
    exoString *tmp_val;
    printf("%s %lu\n","Size: ", list->size);
    while(tmp){
        if(tmp->value->ds_type == EXOSTRING){
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
finds a node with specific key in given list 
*/
listNode* findNode(linkedList *list, exoString *key){
    listNode* tmp = list->head;
    while(tmp){
        if(tmp->key->len ==  key->len && !memcmp(tmp->key->buf, key->buf, key->len)){
            return tmp;
        }
        tmp = tmp->next;
    }
    printf("%s %s\n", GRN "node find called with key: " RESET, key->buf);
    return NULL;
}

/*
replaces the value in a given list node with new value.
value is a object of exoVal data type
*/
exoVal* replaceNodeValue(listNode *node, exoVal *newval){
    if(node == NULL || newval == NULL){
        return NULL;
        printf(GRN "node value Not replaced\n" RESET);
    } else {
        exoVal *tmp;
        tmp = node->value;
        node->value = newval;
        free(tmp);
        printf("%s %s\n", GRN "Node value replaced for node with key: " RESET, node->key->buf);
        return newval;
    }
}

/*
frees all the memory holded my given linkedlist
*/
void freeLinkedList(linkedList* list){
    listNode* node = list->tail;
    listNode* tmp;
    while(node){
        tmp = node->prev;
        freeListNode(node);
        node = tmp;
    }
}

/*
frees the memory holded by given listnode
*/
void freeListNode(listNode *node){
    freeExoString(node->key);
    freeExoVal(node->value);
    free(node);
}
