#include "protocol.h"

/*
It is a string parser which implements RESP protocol to create meaning
out of the string passed. It is used for Commands read from RESP clients

returns a linked list of tokens. Tokens are simply listnode with key as
the token and value as void.

It is a fast and efficient parser due to RESP.

It returns NULL which is treated an an ERR message if the passed
string is illegal according to RESP
*/
argList* bufferTokenizer(char *str, unsigned long len){
    // empty string case, return NULL
    if(len < 1) return NULL;

    argList* tokens = NULL;
    if(*str == '*'){
        // If it starts with a * it must be a RESP string (from redis client)
        printf(MAG "-------------------------- RESP TOKENIZER BEGINS ----------------------\n" RESET);
        tokens = respTokenizer(str, len);
        printf(MAG "-------------------------- RESP TOKENIZER ENDS ------------------------\n" RESET);
    } else {
        // if doesn't starts with a * it must be a simple string (from telnet)
        printf(MAG "-------------------------- SIMPLE TOKENIZER BEGINS --------------------\n" RESET);
        tokens = simpleTokenizer(str, len);
        printf(MAG "-------------------------- SIMPLE TOKENIZER ENDS ----------------------\n" RESET);
    }
    return tokens;
}

argList* respTokenizer(char *str, unsigned long len){
    char* start = str;
    argList *tokens = newArgList();
    argListNode* node;
    unsigned long num, l;
    exoString* tkn;
    // Retrun illegal RESP protocol is first byte != '*'
    if(*str != '*'){
        return NULL;
    }
    // get the number of elements in the RESP array
    str++;
    num = getRespInt(str, start, len) + 1;

    // parse num bulk strings in the byte sequence, return illegal at any time
    // when data type is not bulk string or any un-desired byte

    // loop starts with str @ first byte after *
    while(str - start < len && num--){
        char c;
        c = *str;
        //printf("%s %c %d %ld\n", "-------> ", c , c, str - start);
        // move to next \r\n bytes
        while(str - start < len && *str != '\r') str++;
        // check next 3 bytes to be legal and then parse the next bulk string
        if(*str == '\r' && str + 1 - start < len && *(str+1) == '\n'){
            // if not last \r\n, jump 3 bytes
            if(str + 2 - start < len && *(str+2) == '$'){
                str += 3;
            }
            // if not last \r\n and next byte is not $, its an illegal sequence
            // return NULL 
            else if(str + 2 - start < len && *(str+2) != '$'){
                return NULL;
            }
            // it is the last \r\n, break here
            else {
                // move str to end of the string
                str++;
                //printf("%s  %c %d %ld\n", "BREAKING HERE", c , c, str - start);
                break;
            }
            
        } else {
            return NULL;
        }
        l = getRespInt(str, start, len);
        if(l != -1){
            while(str - start < len && *str != '\r') str++;
            str += 2;
            if(str + l + 1 - start < len){
                tkn = newString(str, l);
                node = newArg(tkn);
                addArgToList(tokens, node);
                str += l;
            } else {
                return NULL;
            }
        } else {
            return NULL;
        }
        if(*str != '\r') return NULL; 
    }
    if(!num && str - start == len - 1){
        return tokens;
    } else {
        return NULL;
    }
}

unsigned long getRespInt(char *str, char *start, unsigned long len){
    unsigned long l = 0;
    while(*str != '\r') {
        // return -1 if illegal byte in sequence
        if(str - start >= len || *str < '0' || *str > '9'){
            return -1;
        }
        l = (l*10)+(*str - '0');
        str++;
    }
    if(str + 1 - start < len && *(str + 1) != '\n'){
        return -1;
    } else {
        return l;
    }
}

/*
It is a string parser which implements a certain protocol to create meaning
out of the string passed. It is used for Commands read from telnet or any other 
client which does not talks in RESP.

It assumes the white space character as the delimiter.

returns a linked list of tokens. Tokens are simply listnode with key as
the token and value as void.

It is a slow parser, since we have to parse the whole string character by character.
*/
argList* simpleTokenizer(char *str, unsigned long len){
    argList* tokens = newArgList();
    argListNode* node;
    exoString* tkn;
    unsigned long index = 0, token_size = 0;
    char *runner = str;

    while(index < len){
        while(index<len && (*runner == ' ' || *runner == '\r' || *runner == '\n' || *runner == '\0')){
            runner++;
            str++;
            index++;
        }
        token_size = 0;
        while(index<len && *runner != ' ' && *runner != '\0' && *runner != '\n' && *runner !='\r'){
            runner++;
            token_size++;
            index++;
        }
        //if(index==len)token_size -= 1;
        if(token_size){
            tkn = newString(str, token_size);
            node = newArg(tkn);
            addArgToList(tokens, node);
        }
        str = runner;
    }
    return tokens;
}