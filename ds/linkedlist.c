#include "linkedlist.h"

linkedList* newLinkedList(){
    linkedList* ll = (linkedList*)malloc(sizeof(linkedList));
    if(ll){
        ll->head = NULL;
        ll->tail = NULL;
        ll->size = 0;
    }
    return ll;
}

listNode* newNode(exoString *key, exoVal *value){
    listNode* tmp = (listNode*)malloc(sizeof( listNode));
    if(tmp){
        tmp->key = key;
        tmp->value = value;
        tmp->next = NULL;
        tmp->prev = NULL;
    }
    return tmp;
}

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

exoVal* replaceNodeValue(listNode *node, exoVal *newval){
    if(node == NULL || newval == NULL){
        return NULL;
    } else {
        exoVal *tmp;
        tmp = node->value;
        node->value = newval;
        free(tmp);
        return newval;
    }
}

// int main(){
//     char key[1000], val[1000], f[1000];
//     listNode *node;
//     linkedList *list = newLinkedList();
//     exoString *k, *v, *find, *tmp_val;
//     exoVal *val_obj;
//     unsigned long l;
//     int n,t;
//     scanf("%d %d", &n, &t);
//     while(n--){
//         scanf("%s", key);
//         scanf("%s", val);
//         l = strlen(key);
//         k = newString((void *)key, l);
//         l = strlen(val);
//         v = newString((void *)val, l);
//         val_obj = newExoVal(EXOSTRING, (void *)v);
//         node = newNode(k, val_obj);
//         addNodeToList(list, node);
//     }
//     printList(list);
//     printf("%s\n", "PRINTING AGAIN");
//     printList(list);

//     while(t--){
//         scanf("%s", f);
//         l = strlen(f);
//         find = newString((void *)f, l);
//         node = findNode(list, find);
//         if(node){
//             exoString *tmp_val = (exoString *)node->value->val_obj;
//             printf("%s %s\n", node->key->buf, tmp_val->buf);
//         } else {
//             printf("%s\n", "NOT FOUND" );
//         }
//     }

//     printf("%s\n", "PRINTING AGAIN");
//     printList(list);
//     return 0;
// }