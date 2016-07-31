#include "utilities.h"

exoString PONG = {
    5,
    "+PONG"
};

exoString RET_OK = {
    3,
    "+OK"
};

exoString RET_NULL = {
    3,
    "$-1"
};

exoString RET_COMMAND_NOT_FOUND = {
    20,
    "-ERR unknown command"
};

exoString RET_WRONG_NUMBER_OF_ARGUMENTS = {
    30,
    "-ERR wrong number of arguments"
};

exoString RET_PROTOCOL_ERROR = {
    19,
    "-ERR Protocol error"
};

exoString RET_WRONG_TYPE_OF_COMMAND_ON_TARGET_OBJECT = {
    39,
    "-WRONG_TYPE_OF_COMMAND_ON_TARGET_OBJECT"
};

exoString RET_FLUSHALL_ERROR = {
    19,
    "-Failed to flush db"
};

exoString RET_NOT_INT_OR_OUT_OF_RANGE = {
    49,
    "-ERR bit offset is not an integer or out of range"
};

exoString RET_BIT_NOT_INT_OR_OUT_OF_RANGE = {
    42,
    "-ERR bit is not an integer or out of range"
};

exoString RET_INCR_OPTION_SUPPORTS_A_SINGLE_INCREMENT_ELEMENT_PAIR = {
    57,
    "-ERR INCR option supports a single increment-element pair"
};

exoString RET_SYNTAX_ERROR = {
    17,
    "-ERR syntax error"
};

exoString RET_VALUE_IS_NOT_A_VALID_FLOAT = {
    31,
    "-ERR value is not a valid float"
};

exoString RET_XX_AND_NX_OPTIONS_AT_THE_SAME_TIME_ARE_NOT_COMPATIBLE = {
    58,
    "-ERR XX and NX options at the same time are not compatible"
};

exoString RET_MIN_OR_MAX_IS_NOT_A_FLOAT = {
    30,
    "-ERR min or max is not a float"
};
/*
exoString is a data_type to store string and its length
it makes length operation O(1)
*/
exoString* newString(void *source, unsigned long len){
    exoString *str = (exoString*)malloc(sizeof (exoString)+ len + 1);
    if(!str){
        return str;
    } else {
        str->len = len;
        if (len) {
            memcpy(str->buf, source, len);
        }
        // every exoString terminates with \0 character
        str->buf[len] = '\0';
        printf("%s %s %lu\n", YEL "new exoString created: " RESET, str->buf, str->len);
        return str; 
    } 
}

/*
exoVal is one data_type to store objects of all data_types. ds_type attribute 
holds the information of data_type of the actual object store in the struct.
Based on ds_type, we can type case actual object
*/
exoVal* newExoVal(size_t ds_type, void *obj){
    exoVal *val = (exoVal*)malloc(sizeof (exoVal));
    if(!val){
        return val;
    } else {
        val->ds_type = ds_type;
        val->val_obj = obj;
        printf(YEL "new exoVal created\n" RESET);
        return val; 
    }
}

/*
Prints exoString with length
*/
void printString(exoString* str){
    if(!str) return;
    else{
        printf("%s\n", str->buf );
        printf("%s %lu\n","Size: ", str->len);
    }
}

/*
Hash function used to calculate hash for strings
*/
size_t stringHash(char *str){
    unsigned long hash = 5381;
    int c, l = HASH_LEN;

    while (*str != '\0' && l-- > 0){
        c = *str++;
        hash = ((hash << 5) + hash) + c;
    }
    printf("%s %s %zu\n", YEL "string hash calculated for str: " RESET, str, hash % INITIAL_SIZE);
    return hash % INITIAL_SIZE;
}

/*
Frees memory holded by exoVal object
*/
void freeExoVal(exoVal* val){
    if(val && val->ds_type == BITMAP){
        freebitmapNode(val->val_obj);
        free(val);
    } else if(val){
        free(val);
    }
}

/*
Frees memory holded by exoString object
*/
void freeExoString(exoString* str){
    if(str){
        free(str);
    }
}

/*
Frees memory holded by bitmapnode object
*/
void freebitmapNode(bitmapNode* node){
    if(node == NULL) return;
    free(node->mem);
    free(node);
}

/*
Function takes in a parameter code and return corresponding err exoString.
It is one place to get result string for errored out cases.
*/
exoVal* returnError(int code){
    printf("%s %d\n", YEL "returnError Called with code: " RESET, code );
    switch(code){
    case COMMAND_NOT_FOUND:
        return newExoVal(RESP_ERROR, &RET_COMMAND_NOT_FOUND);

    case WRONG_NUMBER_OF_ARGUMENTS:
        return newExoVal(RESP_ERROR, &RET_WRONG_NUMBER_OF_ARGUMENTS);

    case WRONG_TYPE_OF_COMMAND_ON_TARGET_OBJECT:
        return newExoVal(RESP_ERROR, &RET_WRONG_TYPE_OF_COMMAND_ON_TARGET_OBJECT);
    case PROTOCOL_ERROR:
        return newExoVal(RESP_ERROR, &RET_PROTOCOL_ERROR);
    case FAILED_TO_FLUSH_DB:
        return newExoVal(RESP_ERROR, &RET_FLUSHALL_ERROR);
    case OFFSET_NOT_INT_OR_OUT_OF_RANGE:
        return newExoVal(RESP_ERROR, &RET_NOT_INT_OR_OUT_OF_RANGE);
    case BIT_NOT_INT_OR_OUT_OF_RANGE:
        return newExoVal(RESP_ERROR, &RET_BIT_NOT_INT_OR_OUT_OF_RANGE);
    case INCR_OPTION_SUPPORTS_A_SINGLE_INCREMENT_ELEMENT_PAIR:
        return newExoVal(RESP_ERROR, &RET_INCR_OPTION_SUPPORTS_A_SINGLE_INCREMENT_ELEMENT_PAIR);
    case SYNTAX_ERROR:
        return newExoVal(RESP_ERROR, &RET_SYNTAX_ERROR);
    case VALUE_IS_NOT_A_VALID_FLOAT:
        return newExoVal(RESP_ERROR, &RET_VALUE_IS_NOT_A_VALID_FLOAT);
    case XX_AND_NX_OPTIONS_AT_THE_SAME_TIME_ARE_NOT_COMPATIBLE:
        return newExoVal(RESP_ERROR, &RET_XX_AND_NX_OPTIONS_AT_THE_SAME_TIME_ARE_NOT_COMPATIBLE);
    case MIN_OR_MAX_IS_NOT_A_FLOAT:
        return newExoVal(RESP_ERROR, &RET_MIN_OR_MAX_IS_NOT_A_FLOAT);
    }
    return returnNull();
}

/*
Cases where exoRedis has to return null
*/
exoVal* returnNull(){
    return newExoVal(SIMPLE_STRING, &RET_NULL);
}

/*
Cases where exoredis has to return +OK
*/
exoVal* returnOK(){
    return newExoVal(SIMPLE_STRING, &RET_OK);
}

/*
Cases where exoredis has to return +PONG
*/
exoVal* returnPong(){
    return newExoVal(SIMPLE_STRING, &PONG);
}

/*
Function to upcase a character string. It is used to upcase command string
All commands are stored in a hash_table with upcase string to handle lower_Case,
Upper_case and Mixed_case scenerios
*/
exoString* upCase(exoString* str){
    strUpCase(str->buf);
    printf("%s %s\n", YEL "cmd str Upcased called with: " RESET, str->buf);
    return str;
}

/*
Utility funtion used by upCase
*/
void strUpCase(char *str){
    unsigned long c = 0;
 
    while (str[c] != '\0') {
        if (str[c] >= 'a' && str[c] <= 'z') {
         str[c] = str[c] - 32;
        }
        c++;
    }
}

/*
Converts an unsigned long into exostring. Returns NULL if fails
*/
exoString* numberToString(unsigned long num){
    char buffer [50];
    if(sprintf (buffer, "%lu", num) > 0){
        return newString(buffer, strlen(buffer));
    } else {
        return NULL;
    }
}

/*
Converts a long double into exostring. Returns NULL if fails
*/
exoString* doubleToString(long double num){
    char buffer [100];
    if(snprintf (buffer, 100, "%Lf", num) > 0){
        return newString(buffer, strlen(buffer));
    } else {
        return NULL;
    }
}

/*
Converts a char string into unsigned long. Return -1 if greater than _max_
*/
unsigned long stringToLong(char *str){
    unsigned long l = 0;
    while(*str != '\0') {
        // return -1 if illegal number or > then OFFSET_MAX
        if(*str < '0' || *str > '9' || l > OFFSET_MAX){
            return -1;
        }
        l = (l*10)+(*str - '0');
        str++;
    }
    return l > OFFSET_MAX ? -1 : l;
}

/*
Converts an exostring into bit. Return -1 if illegal string
*/
int stringToBit(exoString *str){
    char *c = str->buf;
    if(str->len > 1){
        return -1;
    } else if(*c == '0' || *c == '1'){
        return *c - '0';
    } else {
        return -1;
    }
    
}

/*
Convers a char string into long double. Return -1 if illegal
*/
long double stringTolongDouble(char *str){
    long double res;
    long double frac = 0.0;
    int num = 0, fraction = 0, frac_len = 1;
    bool decimal = false;
    while(*str != '\0'){
        if(*str < '0' || *str > '9'){
            if(*str == '.'){
                if(decimal){
                    return -1;
                } else {
                    decimal = true;
                }
            } else {
                return -1;
            }
        } else {
            if(decimal){
                frac_len *= 10;
                fraction = (fraction*10)+(*str - '0');
            } else {
                num = (num*10)+(*str - '0');
            }
        }
        str++;
    }
    res = num;
    if(fraction){
        frac = (double)fraction/frac_len;
    }
    return res + frac;
}