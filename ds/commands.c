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
// cmd_str, ds_type, args_count, variable_arg_count, skip_arg_test, free_args, f_ptr
struct exoCmd commandTable[] = {
  {"GET", BULKSTRING, 1, false, false, true, getCommand},
  {"SET", BULKSTRING, 2, true, true, false, setCommand},
  {"DEL", BULKSTRING, 1, false, true, true, delCommand},
  {"PING", SIMPLE_STRING, 0, false, false, true, pingCommand},
  {"FLUSHALL", SIMPLE_STRING, 0, false, false, true, flushCommand},
  {"GETBIT", BITMAP, 2, false, false, true, getbitCommand},
  {"SETBIT", BITMAP, 3, false, false, false, setbitCommand},
  {"ZADD", SORTED_SET, 3, true, true, false, zaddCommand},
  {"ZCARD", SORTED_SET, 1, false, false, true, zcardCommand},
  {"ZCOUNT", SORTED_SET, 3, false, false, true, zcountCommand},
  {"ZRANGE", SORTED_SET, 3, false, true, true, zrangeCommand},
  {"SAVE", BULKSTRING, 0, false, false, true, saveCommand},
  {"LOADDB", SIMPLE_STRING, 0, false, false, true, loadCommand}
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
    int i, n = sizeof(commandTable)/sizeof(struct exoCmd);
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
    size_t key_hash = stringHash(cmd->cmd_str, l);

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
exoVal* getCommand(argList* args){
    printf(CYN "getCommand Called\n" RESET);
    argListNode* node = args->head;

    // head holds command node, actual args starts from head->next
    exoVal* val = get(HASH_TABLE, node->next->key);

    if(val && val->ds_type == BULKSTRING){
        return val;
    } else if(val && val->ds_type != BULKSTRING) {
        return _Error(WRONG_TYPE_OF_COMMAND_ON_TARGET_OBJECT);
    } else {
        return _Null();
    }
}

/*
Entry point of SET api.
SET command overwirtes the target value if it exists, 
So no need to check the ds_type of target object. Simply over-write
*/
exoVal* setCommand(argList* args){
    printf(CYN "setCommand Called\n" RESET);
    if(args->size > 9){
        return _Error(SYNTAX_ERROR);
    } else if( args->size < 3){
        return _Error(WRONG_NUMBER_OF_ARGUMENTS);
    }
    unsigned long long exp_ms, ex_v = 0, px_v = 0;
    bool ex = false, px = false, nx = false, xx = false;

    int sw = parseSetSwitches(args, &ex, &ex_v, &px, &px_v, &nx, &xx);
    if(sw < 0){
        return _Error(sw);
    }

    exp_ms = ex_v * 1000 + px_v;

    argListNode* node = args->head->next;
    exoString* key;
    exoVal* val;

    if((ex || px) && exp_ms == 0){
        return _Error(INVALID_EXPIRE_TIME_IN_SET);
    } else if(nx && xx) {
        return _Null();
    }

    key = node->key;
    val = newExoVal(BULKSTRING, node->next->key);

    val = setWithExpiry(HASH_TABLE, key, val, exp_ms, nx, xx);
    if(!val){
        return _Null();
    }
    return val;
}

/*
Entry point of DEL api.
Deletes the entry from the hash table.
*/
exoVal* delCommand(argList* args){
    printf(CYN "delCommand Called\n" RESET);
    unsigned long count = 0;

    // head holds command node, actual args starts from head->next
    argListNode* node = args->head->next;
    if(args->size < 2){
        return _Error(WRONG_NUMBER_OF_ARGUMENTS);
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
exoVal* pingCommand(argList* args){
    return _Pong();
}

/*
Entry point of FLUSHALL api.
*/
exoVal* flushCommand(argList* args){
    hashTable *new_table = newHashTable(INITIAL_SIZE);
    if(new_table){
        freeHashTable(HASH_TABLE);
        HASH_TABLE = new_table;
        return _OK();
    } else {
        return _Error(FAILED_TO_FLUSH_DB);
    } 
}

/*
Entry point of GETBIT api.
*/
exoVal* getbitCommand(argList* args){
    bitmapNode* node;
    bool result;
    long long pos;
    printf(CYN "getbitCommand Called\n" RESET);
    argListNode* arg = args->head->next;
    pos = stringToLongLong(arg->next->key->buf);
    if(pos == -1 || pos > OFFSET_MAX){
        return _Error(OFFSET_NOT_INT_OR_OUT_OF_RANGE);
    }

    exoVal* val = get(HASH_TABLE, arg->key);
    if(val){
        if(val->ds_type == BITMAP){
            node = (bitmapNode*)val->val_obj;
            result = getBit(node, pos);                
        } else {
            return _Error(WRONG_TYPE_OF_COMMAND_ON_TARGET_OBJECT);
        }
    } else {     
        result = false;
    }
    return result ? newExoVal(RESP_INTEGER, &RET_BIT_ON) : newExoVal(RESP_INTEGER, &RET_BIT_OFF);
}

/*
Entry point of SETBIT api.
*/
exoVal* setbitCommand(argList* args){
    bitmapNode* node;
    bool result;
    long long pos;
    int bit;
    argListNode* arg = args->head->next;
    printf(CYN "setbitCommand Called\n" RESET);

    pos = stringToLongLong(arg->next->key->buf);
    if(pos == -1 || pos > OFFSET_MAX){
        setAllDead(args);
        return _Error(OFFSET_NOT_INT_OR_OUT_OF_RANGE);
    }
    bit = stringToBit(arg->next->next->key);
    if(bit == -1){
        setAllDead(args);
        return _Error(BIT_NOT_INT_OR_OUT_OF_RANGE);
    }

    exoVal* val = get(HASH_TABLE, arg->key);

    if(val){
        setAllDead(args);
        if(val->ds_type == BITMAP){
            node = (bitmapNode*)val->val_obj;
            result = setBit(node, pos, bit);
        } else {
            return _Error(WRONG_TYPE_OF_COMMAND_ON_TARGET_OBJECT);
        }
    } else {
            args->head->dead = args->head->next->next->dead = args->head->next->next->next->dead = true;
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
exoVal* zaddCommand(argList* args){
    printf(CYN "zaddCommand Called\n" RESET);
    if(args->size < 4){
        return _Error(WRONG_NUMBER_OF_ARGUMENTS);
    }
    bool xx = false, nx = false, ch = false, incr = false;
    argListNode* arg = args->head->next;
    exoVal* ret = NULL;
    exoVal* val = get(HASH_TABLE, arg->key);
    skipList *sk_list = NULL;
    if(val){
        if(val->ds_type == SORTED_SET){
            sk_list = (skipList*)val->val_obj;
        } else {
            setAllDead(args);
            return _Error(WRONG_TYPE_OF_COMMAND_ON_TARGET_OBJECT);
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
exoVal* zcardCommand(argList* args){
    printf(CYN "zcardCommand Called\n" RESET);
    argListNode* arg = args->head->next;
    unsigned long res;
    exoVal* val = get(HASH_TABLE, arg->key);
    skipList *sk_list = NULL;
    if(val){
        if(val->ds_type == SORTED_SET){
            sk_list = (skipList*)val->val_obj;
            res = sk_list->size;
        } else {
            return _Error(WRONG_TYPE_OF_COMMAND_ON_TARGET_OBJECT);
        }
    } else {
        res = 0;
    }
    return newExoVal(RESP_INTEGER, numberToString(res));
}

/*
Entry point of ZCOUNT api.
*/
exoVal* zcountCommand(argList* args){
    printf(CYN "zcountCommand Called\n" RESET);
    argListNode* arg = args->head->next;
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
                return _Error(MIN_OR_MAX_IS_NOT_A_FLOAT);
            }
        } else {
            return _Error(WRONG_TYPE_OF_COMMAND_ON_TARGET_OBJECT);
        }
    } else {
        res = 0;
    }
    return newExoVal(RESP_INTEGER, numberToString(res));
}

/*
Entry point of ZRANGE api.
*/
exoVal* zrangeCommand(argList* args){
    if(args->size -1 != 3 && args->size -1 != 4){
        return _Error(WRONG_NUMBER_OF_ARGUMENTS);
    }
    printf(CYN "zrangeCommand Called\n" RESET);
    argList* result = newArgList();
    argListNode *tmp;
    argListNode* arg = args->head->next;
    bool withscore = false;
    int valid_args;
    long long left, right, len = 0, list_size;

    // return error if illegal arguments, else execute command
    valid_args = parseRange(arg->next, &left, &right, &withscore);
    if(valid_args == -1) {
        return _Error(VALUE_IS_NOT_AN_INTEGER_OR_OUT_OF_RANGE);
    } else if(valid_args == -2) {
        return _Error(SYNTAX_ERROR);
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
            return _Error(WRONG_TYPE_OF_COMMAND_ON_TARGET_OBJECT);
        }
    } else {
        sk_node = NULL;
    }
    while(sk_node && len--){
        tmp = addArgToList(result, newArg(sk_node->key));
        if(withscore){
            tmp = newArg(doubleToString(sk_node->score));
            tmp->dead = true;
            addArgToList(result, tmp);
        }
        sk_node = sk_node->next;
    }
    return newExoVal(RESP_ARRAY, result);
}

/*
Entry point of SAVE api.
*/
exoVal* saveCommand(char *file_path){
    printf("%s\n", "Saving final state of DB on disk.");
    linkedList* list = NULL;
    listNode *node = NULL;
    char buffer[800000];
    int i;
    FILE *pFile;
    pFile = fopen(file_path, "w+");
    if(pFile == NULL){
        perror("File error");
        perror("Failed to save DB on disk");
        return _Null();
    }

    for(i = 0; i < HASH_TABLE->size; i++){
        list = HASH_TABLE->buckets[i];
        node = list->head;

        while(node){
            switch(node->value->ds_type){
            case BULKSTRING:
                writeHashMapToFile(node, pFile, buffer);
                break;
            case BITMAP:
                writeHBitMapToFile(node, pFile, buffer);
                break;
            case SORTED_SET:
                writeSkiplistToFile(node, pFile, buffer);
                break;
            }
            node = node->next;
        }
    }
    fclose(pFile);
    printf("%s\n", "DB saved on disk");
    return _OK();
}

exoVal* loadCommand(char *file_path){
    if(loadFromDB(file_path) != 0){
        return _Null();
    } else {
        return _OK();
    }
}

/* ------------------ utilitiy functions begin here ------------------ */

/*
Utility function used by ZADD, ZCOUNT api
*/
void parseSwitches(argList* args, bool *xx, bool *nx, bool *ch, bool *incr){
    printf(CYN "parseSwitches Called\n" RESET);
    char *str;
    exoString *tmp;
    argListNode *node = args->head->next->next; // move node to third node after cmd and key
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
int parseRange(argListNode* args, long long *left, long long *right, bool *withscore){
    printf(CYN "parseRange Called\n" RESET);
    argListNode *node = args;
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
        num = stringToLongLong(str);
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
        strUpCase(node->key->buf);
        if(strcmp(node->key->buf, "WITHSCORES") == 0){
            *withscore = true;
        } else {
            return -2; // -ERR syntax error
        }
    }
    return 0;
}

int parseSetSwitches(argList* args, bool *ex, uint64_t *ex_v, bool *px,\
                     uint64_t *px_v, bool *nx, bool *xx){
    argListNode *node = args->head->next->next->next;
    long long tmp;

    while(node){
        strUpCase(node->key->buf);

        if(!strcmp(node->key->buf, "EX")){
            *ex = true;
            if(node->next){
                node = node->next;
                tmp = stringToLongLong(node->key->buf);
                if(tmp == -1){
                    return VALUE_IS_NOT_AN_INTEGER_OR_OUT_OF_RANGE;
                } else {
                    *ex_v = (uint64_t)MIN(tmp, DEFAULT_EXPIRY_MS);
                }
            } else {
                return SYNTAX_ERROR;
            }
        } else if(!strcmp(node->key->buf, "PX")){
            *px = true;
            if(node->next){
                node = node->next;
                tmp = stringToLongLong(node->key->buf);
                if(tmp == -1){
                    return VALUE_IS_NOT_AN_INTEGER_OR_OUT_OF_RANGE;
                } else {
                    *px_v = (uint64_t)MIN(tmp, DEFAULT_EXPIRY_MS);
                }
            } else {
                return SYNTAX_ERROR;
            }
        } else if(!strcmp(node->key->buf, "NX")){
            *nx = true;
        } else if(!strcmp(node->key->buf, "XX")){
            *xx = true;
        } else {
            return SYNTAX_ERROR;
        }
        node = node->next;
    }
    return 0;
}


/*
Utility function used by ZRANGE api
*/
bool parseMinMax(argListNode* args, long double *left, long double *right){
    printf(CYN "parseMinMax Called\n" RESET);
    argListNode *node = args;
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
        if(*str == '+'){
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


/* ------------------ functions for DB persistence begin here ------------------ */

void writeHashMapToFile(listNode *node, FILE *pFile, char *buffer){
    exoString *tmp;

    // data type
    unsigned long code = BULKSTRING;
    fwrite(&code, sizeof(unsigned long), 1, pFile);

    // key
    tmp = node->key;
    fwrite(&(tmp->len), sizeof(unsigned long), 1, pFile);
    fwrite(tmp->buf, sizeof(char), tmp->len, pFile);

    // value
    tmp = (exoString*)node->value->val_obj;
    fwrite(&(tmp->len), sizeof(unsigned long), 1, pFile);
    fwrite(tmp->buf, sizeof(char), tmp->len, pFile);

    // expiry
    tmp = llToString(node->expiry);
    fwrite(&(tmp->len), sizeof(unsigned long), 1, pFile);
    fwrite(tmp->buf, sizeof(char), tmp->len, pFile);

}

void writeHBitMapToFile(listNode *node, FILE *pFile, char *buffer){
    exoString *tmp;
    bitmapNode *bm_node;

    // data type
    unsigned long code = BITMAP;
    fwrite(&code, sizeof(unsigned long), 1, pFile);

    // key
    tmp = node->key;
    fwrite(&(tmp->len), sizeof(unsigned long), 1, pFile);
    fwrite(tmp->buf, sizeof(char), tmp->len, pFile);

    // memory
    bm_node = (bitmapNode*)node->value->val_obj;
    fwrite(&(bm_node->len), sizeof(unsigned long), 1, pFile);
    fwrite(bm_node->mem, sizeof(word_t), bm_node->len, pFile);
}

void writeSkiplistToFile(listNode *node, FILE *pFile, char *buffer){
    exoString *tmp;
    skipList *sk_list;
    skipListNode *sk_node;
    sk_list = (skipList*)node->value->val_obj;
    sk_node = sk_list->head;
    while(sk_node->down) 
        sk_node = sk_node->down;

    sk_node = sk_node->next;

    while(sk_node){
        // data type
        unsigned long code = SORTED_SET;
        fwrite(&code, sizeof(unsigned long), 1, pFile);

        // key
        tmp = node->key;
        fwrite(&(tmp->len), sizeof(unsigned long), 1, pFile);
        fwrite(tmp->buf, sizeof(char), tmp->len, pFile);

        // score
        tmp = doubleToString(sk_node->score);
        fwrite(&(tmp->len), sizeof(unsigned long), 1, pFile);
        fwrite(tmp->buf, sizeof(char), tmp->len, pFile);


        // member
        tmp = sk_node->key;
        fwrite(&(tmp->len), sizeof(unsigned long), 1, pFile);
        fwrite(tmp->buf, sizeof(char), tmp->len, pFile);

        sk_node = sk_node->next;
    }
}

int loadFromDB(char *file_path){
    FILE *fp;
    exoString *key, *tmp;
    exoVal *val;
    // allocalte 520mb for buffer
    void *buffer = malloc(sizeof(char) * 1024 * 1024 * 520);

    unsigned long len = 0;
    unsigned long code;
    long double score;
    argList *tokens = NULL;

    fp = fopen(file_path, "r");
    if (fp){
        while(fread(&code, sizeof(unsigned long), 1, fp)){
            switch(code){
                case BULKSTRING:
                    if(loadHashMapEntry(fp, buffer) == -1)
                        return -1;
                    break;

                case SORTED_SET:
                    if(loadSkiplistEntry(fp, buffer) == -1)
                        return -1;
                    break;

                case BITMAP:
                    if(loadBitmapEntry(fp, buffer) == -1)
                        return -1;
                    break;
            }
        }
    }
    free(buffer);
    return 0;
}

int loadHashMapEntry(FILE *fp, void *buffer){
    exoVal *res, *val;
    int ret;
    unsigned long len;
    uint64_t exp_ms;
    exoString *str,*key, *tmp;

    //key
    fread(&len, sizeof(unsigned long), 1, fp);
    fread(buffer, sizeof(char), len, fp);
    key = newString(buffer, len);

    //value
    fread(&len, sizeof(unsigned long), 1, fp);
    fread(buffer, sizeof(char), len, fp);
    tmp = newString(buffer, len);
    val = newExoVal(BULKSTRING, tmp);

    //expiry
    fread(&len, sizeof(unsigned long), 1, fp);
    fread(buffer, sizeof(char), len, fp);
    str = newString(buffer, len);
    exp_ms = stringToLongLong(str->buf);
 
    res = setFromLoad(HASH_TABLE, key, val, exp_ms);
    if(res){
      str = (exoString*)res->val_obj;
      ret = strcmp("+OK", str->buf) == 0 ?  0 :  -1;  
    } else {
        ret = -1;
    }
    return ret;
}

int loadSkiplistEntry(FILE *fp, void *buffer){
    argList *tokens = newArgList();
    unsigned long len;
    int ret;
    exoString *str;
    exoVal *res = NULL;
    argListNode *arg;

    str = newString("ZADD", 4);
    arg = newArg(str);
    addArgToList(tokens, arg);

    // key
    fread(&len, sizeof(unsigned long), 1, fp);
    fread(buffer, sizeof(char), len, fp);
    str = newString(buffer, len);
    arg = newArg(str);
    addArgToList(tokens, arg);

    // score
    fread(&len, sizeof(unsigned long), 1, fp);
    fread(buffer, sizeof(char), len, fp);
    str = newString(buffer, len);
    arg = newArg(str);
    addArgToList(tokens, arg);

    //member
    fread(&len, sizeof(unsigned long), 1, fp);
    fread(buffer, sizeof(char), len, fp);
    str = newString(buffer, len);
    arg = newArg(str);
    addArgToList(tokens, arg);

    res = zaddCommand(tokens);
    freeDeadArgs(tokens);
    if(res){
      str = (exoString*)res->val_obj;
      ret = strcmp("1", str->buf) == 0 ? 0 : -1;  
    } else {
        ret = -1;
    }
    return ret;
}

int loadBitmapEntry(FILE *fp, void *buffer){
    unsigned long len;
    exoString *key;

    //key
    fread(&len, sizeof(unsigned long), 1, fp);
    fread(buffer, sizeof(char), len, fp);
    key = newString(buffer, len);

    //memory
    fread(&len, sizeof(unsigned long), 1, fp);
    fread(buffer, sizeof(word_t), len, fp);

    return loadBitmap(key, buffer, len);
}