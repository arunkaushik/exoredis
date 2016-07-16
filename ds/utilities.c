#include "utilities.h"

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
    printf("%s %s\n", YEL "new stringHash called for string: " RESET, str);
    unsigned long hash = 5381;
    int c, l = HASH_LEN;

    while (*str != '\0' && l-- > 0){
        c = *str++;
        hash = ((hash << 5) + hash) + c;
    }
    printf("%s %zu\n", YEL "string hash calculated: " RESET, hash % INITIAL_SIZE);
    return hash % INITIAL_SIZE;
}

/*
Frees memory holded by exoVal object
*/
void freeExoVal(exoVal* val){
    if(val){
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
Function takes in a parameter code and return corresponding err exoString.
It is one place to get result string for errored out cases.
*/
exoString* returnError(int code){
    printf("%s %d\n", YEL "returnError Called with code: " RESET, code );
    char *err_str;
    switch(code){
        case COMMAND_NOT_FOUND:
            err_str = "-ERR COMMAND_NOT_FOUND";
            return newString(err_str, strlen(err_str));

        case WRONG_NUMBER_OF_ARGUMENTS:
            err_str = "-ERR WRONG_NUMBER_OF_ARGUMENTS";
            return newString(err_str, strlen(err_str));

        case WRONG_TYPE_OF_COMMAND_ON_TARGET_OBJECT:
            err_str = "-ERR WRONG_TYPE_OF_COMMAND_ON_TARGET_OBJECT";
            return newString(err_str, strlen(err_str));
    }
    return returnNull();
}

/*
Cases where exoRedis has to return null
*/
exoString* returnNull(){
    char *str = "$-1";
    return newString(str, strlen(str));
}

/*
Cases where exoredis has to return +OK
*/
exoString* returnOK(){
    char *str = "+OK";
    return newString(str, strlen(str));
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
