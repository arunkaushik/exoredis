#include "skiplist.h"
/*
returns a new empty skiplist
*/
skipList* newSkipList(){
    printf(RED "newSkipList Called\n" RESET);
    skipList* list = malloc(sizeof(skipList));
    exoString *sentinel = NULL;
    if(list){
        sentinel = newString("sentinel", strlen("sentinel"));
        list->head = newSkipListNode(sentinel, DOUBLE_MIN);
        list->size = 0;
        list->levels = 0;
        // have to make dynamic
        list->table = newHashTable(INITIAL_SIZE);
    }
    return list;
}

/*
returns a new skiplist node with member and score
*/
skipListNode* newSkipListNode(exoString* key, long double score){
    printf(RED "newSkipListNode Called\n" RESET);
    skipListNode* node = malloc(sizeof(skipListNode));
    if(node){
        node->key = key;
        node->score = score;
        node->children = 1;
        node->next = NULL;
        node->down = NULL;
    }
    return node;
}

/*
Adds new node to sorted set based on switches(xx,nx,incr)
*/
exoVal* addToSortedSet(skipList* list, bool xx, bool nx, bool ch, bool incr, argList* args){
    printf(RED "addToSortedSet Called\n" RESET);
    if(nx && xx){
        setAllDead(args);
        return returnError(XX_AND_NX_OPTIONS_AT_THE_SAME_TIME_ARE_NOT_COMPATIBLE);
    }
    size_t switches = 0, headers, move;
    argListNode* arg = args->head;
    if(nx || xx) switches++;
    if(ch) switches++;
    if(incr) switches++;

    headers = args->size - switches;
    if(headers % 2 == 0 ){
        move = switches + 2;
        while(move--)
            arg = arg->next;
        if(incr){
            if(args->size - switches == 4 ){
                return scoreIncrement(list, arg, nx);
            } else {
                setAllDead(args);
                return returnError(INCR_OPTION_SUPPORTS_A_SINGLE_INCREMENT_ELEMENT_PAIR);
            }
        } else {
            return addOrUpdateMember(list, xx, nx, ch, arg, args->size - switches - 2);
        }
    } else {
        setAllDead(args);
        return returnError(SYNTAX_ERROR);
    }
}

/*
Add elements in skiplist stored at list. Returns number of elements added or changed
based on switches(xx, nx, ch). Handels 2*n argumentss
*/
exoVal* addOrUpdateMember(skipList* list, bool xx, bool nx, bool ch, argListNode* args, size_t arg_size){
    printf(RED "addOrUpdateMember Called\n" RESET);
    skipListNode* target = NULL;
    size_t changed = 0, new_insert = 0, res, i;
    long double score;
    exoString* member;
    long double scores[arg_size];
    exoString* members[arg_size];
    bool valid_args = parseArgs(args, scores, members);
    if(valid_args){
        for(i = 0; i < arg_size/2; i++){
            score = scores[i];
            member = members[i];
            //printf("%s %zu %lf %s\n", RED "addOrUpdateMember FOR LOOP: \n" RESET, i, score, member->buf);
            exoVal* tmp = get(list->table, member);
            if(xx){
                if(tmp){
                    target = (skipListNode*)tmp->val_obj;;
                    removeNodeFromSkipList(list, member, target->score, target);
                    addNodeToSkiplist(list, member, score);
                    changed++;
                }
            } else if(nx) {
                if(tmp == NULL){
                    addNodeToSkiplist(list, member, score);
                    new_insert++;
                }
            } else {
                if(tmp){
                    target = (skipListNode*)tmp->val_obj;;
                    removeNodeFromSkipList(list, member, target->score, target);
                    addNodeToSkiplist(list, member, score);
                    changed++;
                } else {
                    addNodeToSkiplist(list, member, score);
                    new_insert++;
                }
            }
        }
    } else {
        return returnError(VALUE_IS_NOT_A_VALID_FLOAT);
    }
    
    res = ch ? changed : new_insert;
    return newExoVal(RESP_INTEGER, numberToString((unsigned long)res));
}

/*
Increment score of the member passed and re-inserts it in correct place
*/
exoVal* scoreIncrement(skipList* list, argListNode* args, bool nx){
    printf(RED "scoreIncrement Called\n" RESET);
    skipListNode* target = NULL;
    long double res = 0;
    // INCR Switch takes only 1 score, member pair
    long double scores[1];
    exoString* members[1];
    bool valid_args = parseArgs(args, scores, members);
    if(valid_args){
        exoVal* tmp = get(list->table, members[0]);
        if(tmp){
            if(nx){
                // setting score and member as dead to free memory
                args->dead = args->next->dead = true;
                return returnNull();
            } else {
                target = (skipListNode*)tmp->val_obj;
                res += target->score;
                removeNodeFromSkipList(list, members[0], target->score, target); 
            }          
        }
        res += scores[0];
        addNodeToSkiplist(list, members[0], res);
        return newExoVal(BULKSTRING, doubleToString(res));
    } else {
        return returnError(VALUE_IS_NOT_A_VALID_FLOAT);
    }
}


/*
Utility function to materialize new node insert. Returns the reference of the node
stored in the highest level of the list
*/
skipListNode* addNodeToSkiplist(skipList *list, exoString* key, long double score){
    printf(RED "addNodeToSkiplist Called\n" RESET);
    skipListNode *tmp = NULL, *prev = NULL;
    size_t children[SKIPLIST_MAX_LEVEL + 1];
    skipListNode *dirty[SKIPLIST_MAX_LEVEL];
    size_t raise_to = randomLevelRaiser();
    int i;

    children[list->levels + 1] = 1;
    memset(dirty, 0, sizeof(dirty));
    tmp = list->head;
    i = list->levels;
    while(i >= 0){
        children[i] = children[i+1];
        while(tmp->next && ( tmp->next->score < score || 
                (tmp->next->score == score && strcmp(tmp->key->buf, key->buf) < 0 ))){
            children[i] += tmp->children;
            tmp = tmp->next;
        }
        // tmp can never be NULL because first check in above loop is tmp->next;
        dirty[i] = tmp;
        tmp = tmp->down;
        i--;
    }

    for(i = list->levels + 1; i <= raise_to; i++){
        tmp = newSkipListNode(newString("sentinel", strlen("sentinel")), DOUBLE_MIN);
        tmp->down = list->head;
        tmp->children = list->size + 1;
        list->head = tmp;
        dirty[i] = tmp;
        children[i] = 1;

        list->levels = raise_to;
    }

    for(i = 0; i <= raise_to; i++){
        tmp = newSkipListNode(key, score);
        tmp->next = dirty[i]->next;
        dirty[i]->next = tmp;
        tmp->children = dirty[i]->children - (children[0] - children[i]);
        dirty[i]->children = children[0] - children[i] + 1;
        if(prev){
            tmp->down = prev;
        }
        prev = tmp;
    }

    for(i = raise_to + 1; i <= list->levels; i++){
        dirty[i]->children++;
    }
    set(list->table, key, newExoVal(SORTED_SET, tmp));
    list->size++;
    return tmp;
}

/*
Function to choose the level in a randomized way.
*/
size_t randomLevelRaiser(){
    size_t count = 0;
    while(rand() % 2)
        count++;
    printf("%s %zu", RED "randomLevelRaiser Called: \n" RESET, count);
    return count % SKIPLIST_MAX_LEVEL;
}

/*
Node is only removed if it is found in the hash_table of list. Its reference is passed 
to the function. Caller must enforce that function is called only if member exists.
*/
void removeNodeFromSkipList(skipList *list, exoString *key, long double score, skipListNode *node){
    printf(RED "removeNodeFromSkipList Called\n" RESET);

    if(node == NULL) return; // dirty code, its here just to make sure work is really required.

    skipListNode *tmp = NULL, *to_free = node;
    skipListNode *dirty[SKIPLIST_MAX_LEVEL];
    int i;
    memset(dirty, 0, sizeof(dirty));
    tmp = list->head;
    i = list->levels;
    while(i >= 0){
        while(tmp->next && ( tmp->next->score < score || 
                (tmp->next->score == score && strcmp(tmp->next->key->buf, key->buf) < 0 ))){
            tmp = tmp->next;
        }
        // tmp can never be NULL because first check in above loop is tmp->next;
        dirty[i] = tmp;
        tmp = tmp->down;
        i--;
    }
    tmp = dirty[0]->next;
    if (tmp && score == tmp->score && strcmp(tmp->key->buf, key->buf) == 0) {
        for (i = list->levels; i >= 0; i--) {
            if( dirty[i]->next && strcmp(tmp->key->buf, dirty[i]->next->key->buf) == 0 ) {
                dirty[i]->children += node->children - 1;
                dirty[i]->next = node->next;
                node = node->down;
            } else {
                dirty[i]->children--;
            }
        }
        freeSkipListNode(to_free);
        del(list->table, key);
        // Not altering the levels of the list. Let it sit as it is. Could be a improvement
        list->size--;
    }
}

void freeSkipListNode(skipListNode *node){
    printf("%s %s %LF\n",RED "freeSkipListNode Called with key and score: " RESET, node->key->buf, node->score);
    /*
    * A skiplistnode in all levels share the same exoString as the key. key will be freed by
    * del command which takes care of removing the entry from skiplist's hashtable
    */
    skipListNode *tmp = NULL;
    while(node){
        tmp = node->down;
        free(node);
        node = tmp;
    }
}

/*
Prints the skiplist to the screen
*/
void freeSkipList(skipList* list){
    printf(RED "freeSkipList Called: \n" RESET);
    int i = list->levels;
    skipListNode *node = list->head, *head = list->head, *tmp, *tmp_head;
    while(i>=0){
        tmp_head = head->down;
        while(node) {
            tmp = node->next;
            free(node);
            node = tmp;
        }
        node = tmp_head;
        head = tmp_head;
        i--;
    }
    freeHashTable(list->table);
    free(list);
}

/*
Prints the skiplist to the screen
*/
void printSkipList(skipList* list){
    printf("%s\n", WHT "----------------- PRINTING LIST -------------- " RESET);
    int i = list->levels;
    skipListNode *node = list->head, *head = list->head;
    while(i>=0){
        printf("%s %d\n", "LEVEL: ", i);
        while(node) {
            printf(" %s %Lf %zu %c ", node->key->buf, node->score, node->children, '|' );
            node = node->next;
        }
        printf("\n");
        node = head->down;
        head = head->down;
        i--;
    }
    printf("%s\n", WHT "----------------- PRINTING LIST -------------- " RESET);
}

/*
Utility function used by zadd command to parse the arguments.
Checks the validity of all arguments before starting the add operation
It converts the arguments in required data type.
Return true is parsing is successful and args are valids. Return false otherwise
*/
bool parseArgs(argListNode* args, long double scores[], exoString* members[]){
    printf(RED "parseArgs Called\n" RESET);
    argListNode *node = args;
    bool minus;
    char *str;
    long double score;
    size_t i = 0;
    while(node){
        // setting all score nodes as dead
        node->dead = true;
        minus = false;
        str = node->key->buf;
        if(*str == '-'){
            minus = true;
            str++;
        }
        score = stringTolongDouble(str);
        if(score == -1){
            return false;
        } else {
            score = minus ? -1 * score : score;
            scores[i] = score;
            members[i] = node->next->key;
        }
        i++;
        node = node->next->next;
    }
    return true;
}

/*
Return the rank of a member in the list. inclusive flas determines to check
for equality of scores or not
*/
unsigned long rankByScore(skipList* list, long double score, bool inclusive){
    printf("%s %Lf\n", RED "-------- rankByScore Called----------: " RESET, score);
    skipListNode *tmp = list->head;
    bool check = false;
    unsigned long rank = 0;
    int i = list->levels;
    while(i >= 0){
        if(inclusive){
            while(tmp->next && tmp->next->score <= score){
                rank += tmp->children;
                tmp = tmp->next;
            }
            tmp = tmp->down;
            i--;    
        } else {
             while(tmp->next && tmp->next->score < score){
                rank += tmp->children;
                tmp = tmp->next;
            }
            tmp = tmp->down;
            i--;
        }
    }
    return rank;
}

skipListNode* rankByOrder(skipList* list, long long pos){
    printf("%s\n", RED "-------- rankByOrder Called----------: " RESET);
    int i = list->levels;
    unsigned long count = 0;
    skipListNode *tmp = list->head;
    skipListNode *prev= NULL;
    if(pos >= list->size || pos < 0){
        return NULL;
    } else {
        while(i >= 0){
            while(tmp->next && count + tmp->children <= pos){
                count += tmp->children;
                tmp = tmp->next;
            }
            prev = tmp;
            tmp = tmp->down;
            i--; 
        }
        return prev->next;
    }
}