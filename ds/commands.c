#include "commands.h"

exoString RET_BIT_ON = {
    1,
    "1"
};

exoString RET_BIT_OFF = {
    1,
    "0"
};
/*
array of exoCmds. exoCmd is the data_type that holds all the information 
of the APIs exposed to clients, like GET, SET, etc. 
It also folds the function pointers to which command execution shall be dispatched
*/
struct exoCmd commandTable[] = {
  {"GET", BULKSTRING, 1, false, false, getCommand},
  {"SET", BULKSTRING, 2, true, false, setCommand},
  {"DEL", BULKSTRING, 1, false, true, delCommand},
  {"PING", SIMPLE_STRING, 0, false, false, pingCommand},
  {"FLUSHALL", SIMPLE_STRING, 0, false, false, flushCommand},
  {"GETBIT", BITMAP, 2, false, false, getbitCommand},
  {"SETBIT", BITMAP, 3, false, false, setbitCommand},
  {"ZADD", SORTED_SET, 3, true, true, zaddCommand},
  {"ZCARD", SORTED_SET, 1, false, false, zcardCommand},
  {"ZCOUNT", SORTED_SET, 3, false, false, zcountCommand},
  {"ZRANGE", SORTED_SET, 3, false, true, zrangeCommand}
};

/*
Initialized a hash_table which stores all the APIs.
key is the upcased string of command name, like SET for set and GET for get.
All APIs commands are stored in a hash_table. When a requests comes, the command 
is searched in this hash_table. If the command is found, it holds the function pointer
to which the task shall be dispatched. If the command is not found, then that command
is not supported by exoRedis
*/
hashTable* initializeCmdTable(){
    printf(CYN "initialized COMMANDS\n" RESET);
    hashTable* commands = newHashTable(INITIAL_SIZE);
    int i,n = sizeof(commandTable)/sizeof(struct exoCmd);
    for (i = 0; i < n; i++) {
        exoCmd *c = commandTable+i;
        if(addCommand(commands, c) == NULL){
        return NULL;
        }
    }
    return commands;
}

/*
adds a command in the command hash_table. Utility function used by initializeCmdTable
*/
exoCmd* addCommand(hashTable *ht, exoCmd* cmd){
    unsigned long l = strlen(cmd->cmd_str);
    size_t key_hash = stringHash(cmd->cmd_str);

    exoString* cmd_str = newString(cmd->cmd_str, l);
    exoVal* exo_cmd = newExoVal(EXOCMD, cmd);

    listNode* cmd_node = newNode(cmd_str, exo_cmd);

    if(addNodeToList(ht->buckets[key_hash], cmd_node)){
        return cmd;
    } else {
        return NULL;
    }
}

/*
Entry point for GET api
*/
exoVal* getCommand(linkedList* args){
    printf(CYN "getCommand Called\n" RESET);
    listNode* node = args->head;

    // head holds command node, actual args starts from head->next
    exoVal* val = get(HASH_TABLE, node->next->key);
    if(val && val->ds_type == BULKSTRING){
        return val;
    } else if(val && val->ds_type != BULKSTRING) {
        return returnError(WRONG_TYPE_OF_COMMAND_ON_TARGET_OBJECT);
    } else {
        return returnNull();
    }
}

/*
Entry point of SET api.
SET command overwirtes the target value if it exists, 
So no need to check the ds_type of target object. Simply over-write
*/
exoVal* setCommand(linkedList* args){
    printf(CYN "setCommand Called\n" RESET);
    listNode* node = args->head->next;
    exoString* key;
    exoVal* val;
    while(node){
        key = node->key;
        val = newExoVal(BULKSTRING, node->next->key);
        val = set(HASH_TABLE, key, val);
        if(!val){
            return returnNull();
        }
        node = node->next->next;
    }
    return returnOK();
}

/*
Entry point of DEL api.
Deletes the entry from the hash table.
*/
exoVal* delCommand(linkedList* args){
    printf(CYN "delCommand Called\n" RESET);
    unsigned long count = 0;
    listNode* node = args->head;

    // head holds command node, actual args starts from head->next
    if(args->size < 2){
        return returnError(WRONG_NUMBER_OF_ARGUMENTS);
    } else {
        while(node){
            count += del(HASH_TABLE, node->key);
            node = node->next;
        }
    }
    return newExoVal(RESP_INTEGER, numberToString(count));
}

/*
Entry point of PING api.
*/
exoVal* pingCommand(linkedList* args){
    return returnPong();
}

/*
Entry point of FLUSHALL api.
*/
exoVal* flushCommand(linkedList* args){
    hashTable *new_table = newHashTable(INITIAL_SIZE);
    if(new_table){
        freeHashTable(HASH_TABLE);
        HASH_TABLE = new_table;
        return returnOK();
    } else {
        return returnError(FAILED_TO_FLUSH_DB);
    } 
}

/*
Entry point of GETBIT api.
*/
exoVal* getbitCommand(linkedList* args){
    bitmapNode* node;
    bool result;
    unsigned long pos;
    printf(CYN "getbitCommand Called\n" RESET);
    listNode* arg = args->head->next;
    pos = stringToLong(arg->next->key->buf);
    if(pos == -1){
        return returnError(OFFSET_NOT_INT_OR_OUT_OF_RANGE);
    }

    exoVal* val = get(HASH_TABLE, arg->key);
    if(val){
        if(val->ds_type == BITMAP){
            node = (bitmapNode*)val->val_obj;
            result = getBit(node, pos);                
        } else {
            return returnError(WRONG_TYPE_OF_COMMAND_ON_TARGET_OBJECT);
        }
    } else {     
        result = false;
    }
    return result ? newExoVal(RESP_INTEGER, &RET_BIT_ON) : newExoVal(RESP_INTEGER, &RET_BIT_OFF);
}

/*
Entry point of SETBIT api.
*/
exoVal* setbitCommand(linkedList* args){
    bitmapNode* node;
    bool result;
    unsigned long pos;
    int bit;
    listNode* arg = args->head->next;
    printf(CYN "setbitCommand Called\n" RESET);

    pos = stringToLong(arg->next->key->buf);
    if(pos == -1){
        return returnError(OFFSET_NOT_INT_OR_OUT_OF_RANGE);
    }
    bit = stringToBit(arg->next->next->key);
    if(bit == -1){
        return returnError(BIT_NOT_INT_OR_OUT_OF_RANGE);
    }

    exoVal* val = get(HASH_TABLE, arg->key);

    if(val){
        if(val->ds_type == BITMAP){
            node = (bitmapNode*)val->val_obj;
            result = setBit(node, pos, bit);
        } else {
            return returnError(WRONG_TYPE_OF_COMMAND_ON_TARGET_OBJECT);
        }
    } else {     
            node = initBitmapNode(pos, bit);
            val = newExoVal(BITMAP, node);
            set(HASH_TABLE, arg->key, val);
            result = false;
    }
    return result ? newExoVal(RESP_INTEGER, &RET_BIT_ON) : newExoVal(RESP_INTEGER, &RET_BIT_OFF);
}

/*
Entry point of ZADD api.
Checks of arguments by itself
*/
exoVal* zaddCommand(linkedList* args){
    printf(CYN "zaddCommand Called\n" RESET);
    bool xx = false, nx = false, ch = false, incr = false;
    listNode* arg = args->head->next;
    exoVal* ret;
    exoVal* val = get(HASH_TABLE, arg->key);
    skipList *sk_list = NULL;
    if(val){
        if(val->ds_type == SORTED_SET){
            sk_list = (skipList*)val->val_obj;
        } else {
            return returnError(WRONG_TYPE_OF_COMMAND_ON_TARGET_OBJECT);
        }
    } else {
        sk_list = newSkipList();
        set(HASH_TABLE, arg->key, newExoVal(SORTED_SET, sk_list));
    }
    parseSwitches(args, &xx, &nx, &ch, &incr);
    ret = addToSortedSet(sk_list, xx, nx, ch, incr, args);
    if(sk_list->size == 0){
        // have to free memory too
        del(HASH_TABLE, arg->key);
    }
    printSkipList(sk_list);
    return ret;
}

/*
Entry point of ZCARD api.
*/
exoVal* zcardCommand(linkedList* args){
    printf(CYN "zcardCommand Called\n" RESET);
    listNode* arg = args->head->next;
    unsigned long res;
    exoVal* val = get(HASH_TABLE, arg->key);
    skipList *sk_list = NULL;
    if(val){
        if(val->ds_type == SORTED_SET){
            sk_list = (skipList*)val->val_obj;
            res = sk_list->size;
        } else {
            return returnError(WRONG_TYPE_OF_COMMAND_ON_TARGET_OBJECT);
        }
    } else {
        res = 0;
    }
    return newExoVal(RESP_INTEGER, numberToString(res));
}

/*
Entry point of ZCOUNT api.
*/
exoVal* zcountCommand(linkedList* args){
    printf(CYN "zcountCommand Called\n" RESET);
    listNode* arg = args->head->next;
    bool valid_args = false;
    unsigned long res, ma, mi;
    long double left, right;
    exoVal* val = get(HASH_TABLE, arg->key);
    skipList *sk_list = NULL;
    if(val){
        if(val->ds_type == SORTED_SET){
            sk_list = (skipList*)val->val_obj;
            valid_args = parseMinMax(arg->next, &left, &right);
            if(valid_args){
                mi = rankByScore(sk_list, left, false);
                ma = rankByScore(sk_list, right, true);
                res = mi > ma ? 0 : ma - mi;
            } else {
                return returnError(MIN_OR_MAX_IS_NOT_A_FLOAT);
            }
        } else {
            return returnError(WRONG_TYPE_OF_COMMAND_ON_TARGET_OBJECT);
        }
    } else {
        res = 0;
    }
    return newExoVal(RESP_INTEGER, numberToString(res));
}

/*
Entry point of ZRANGE api.
*/
exoVal* zrangeCommand(linkedList* args){
    if(args->size -1 != 3 && args->size -1 != 4){
        return returnError(WRONG_NUMBER_OF_ARGUMENTS);
    }
    printf(CYN "zrangeCommand Called\n" RESET);
    linkedList* result = newLinkedList();
    listNode *tmp;
    listNode* arg = args->head->next;
    bool withscore = false;
    int valid_args;
    long long left, right, len = 0, list_size;

    // return error if illegal arguments, else execute command
    valid_args = parseRange(arg->next, &left, &right, &withscore);
    if(valid_args == -1) {
        return returnError(VALUE_IS_NOT_AN_INTEGER_OR_OUT_OF_RANGE);
    } else if(valid_args == -2) {
        return returnError(SYNTAX_ERROR);
    }

    exoVal* val = get(HASH_TABLE, arg->key);
    skipList *sk_list = NULL;
    skipListNode *sk_node = NULL;
    if(val){
        if(val->ds_type == SORTED_SET){
            sk_list = (skipList*)val->val_obj;
            list_size = (long long)sk_list->size;
            left = left < 0 ? MAX(list_size + left, 0) : left;
            right = right < 0 ? MAX(list_size + right, -1) : right;
            right = MIN(right, list_size - 1);
            len = MAX(right - left + 1, 0);
            sk_node = len > 0 ? rankByOrder(sk_list, left) : NULL;
        } else {
            return returnError(WRONG_TYPE_OF_COMMAND_ON_TARGET_OBJECT);
        }
    } else {
        sk_node = NULL;
    }
    while(sk_node && len--){
        tmp = addNodeToList(result, newNode(sk_node->key, NULL));
        if(withscore){
            addNodeToList(result, newNode(doubleToString(sk_node->score), NULL));
        }
        sk_node = sk_node->next;
    }
    return newExoVal(RESP_ARRAY, result);
}

/*
Utility function used by ZADD, ZCOUNT api
*/
void parseSwitches(linkedList* args, bool *xx, bool *nx, bool *ch, bool *incr){
    printf(CYN "parseSwitches Called\n" RESET);
    char *str;
    exoString *tmp;
    listNode *node = args->head->next->next; // move node to third node after cmd and key
    while(node){
        str = node->key->buf;
        if((*str >= '0' && *str <= '9') || *str == '.' || *str == '-') // score and member pair started
            break;

        tmp = upCase(node->key);
        if( strcmp("NX", tmp->buf) == 0 ){
            *nx = true;
        } else if( strcmp("XX", tmp->buf) == 0 ){
            *xx = true;
        } else if( strcmp("CH", tmp->buf) == 0 ){
            *ch = true;
        } else if( strcmp("INCR", tmp->buf) == 0 ){
            *incr = true;
        }
        node = node->next;
    }
}

/*
Utility function used by ZCOUNT api
*/
int parseRange(listNode* args, long long *left, long long *right, bool *withscore){
    printf(CYN "parseRange Called\n" RESET);
    listNode *node = args;
    exoString *tmp;
    bool minus;
    unsigned long num;
    size_t count = 0;
    char *str;
     while(node && count < 2){
        minus = false;
        str = node->key->buf;
        if(*str == '-'){
            minus = true;
            str++;
        }
        num = stringToLong(str);
        if(num == -1){
            return -1; //-ERR value is not an integer or out of range
        } else {
            num = minus ? -1 * num : num;
            if(count == 0)
                *left = num;
            else 
                *right = num;
        }
        count++;
        node = node->next;
    }
    if(node){
        tmp = upCase(node->key);
        if(strcmp(tmp->buf, "WITHSCORES") == 0){
            *withscore = true;
        } else {
            return -2; // -ERR syntax error
        }
    }
    return 0;
}

bool parseMinMax(listNode* args, long double *left, long double *right){
    printf(CYN "parseMinMax Called\n" RESET);
    listNode *node = args;
    bool minus;
    long double score;
    size_t count = 0;
    char *str;
     while(node){
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
            if(count == 0)
                *left = score;
            else 
                *right = score;
        }
        count++;
        node = node->next;
    }
    return true;
}