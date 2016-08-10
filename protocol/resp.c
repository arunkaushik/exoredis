#include "resp.h"

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
    if(len < 2){
        return NULL;
    }

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



argList* fileLineTokenizer(char *str, unsigned long len){
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
    num = fileRespInt(str, start, len) + 1;

    // parse num bulk strings in the byte sequence, return illegal at any time
    // when data type is not bulk string or any un-desired byte

    // loop starts with str @ first byte after *
    while(str - start < len && num--){
        char c;
        c = *str;
        //printf("%s %c %d %ld\n", "-------> ", c , c, str - start);
        // move to next \r\n bytes
        while(str - start < len && *str != ' ') str++;
        // check next 3 bytes to be legal and then parse the next bulk string
        if(*str == ' ' && str - start < len){
            // if not last \r\n, jump 3 bytes
            if(str + 1 - start < len && *(str+1) == '$'){
                str += 2;
            }
            // if not last \r\n and next byte is not $, its an illegal sequence
            // return NULL 
            else if(str + 1 - start < len && *(str+1) != '$'){
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
        l = fileRespInt(str, start, len);
        if(l != -1){
            while(str - start < len && *str != ' ') str++;
            str += 1;
            if(str + l - start < len){
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
        if(*str != ' '){
            return NULL;
        } 
    }
    if(!num && str - start == len){
        return tokens;
    } else {
        return NULL;
    }
}

unsigned long fileRespInt(char *str, char *start, unsigned long len){
    unsigned long l = 0;
    while(*str != ' ') {
        // return -1 if illegal byte in sequence
        if(str - start >= len || *str < '0' || *str > '9'){
            return -1;
        }
        l = (l*10)+(*str - '0');
        str++;
    }
    return l;
}

// int main(){
//     linkedList *tokens;
//     listNode *node;
//     int n;
//     char *str;
//     char *str_arr[43];
//     str_arr[0]= "*";
//     str_arr[1]= "*2";
//     str_arr[2]= "*2\r";
//     str_arr[3]= "*2\r\n";
//     str_arr[4]= "*2\r\n$";
//     str_arr[5]= "*2\r\n$3";
//     str_arr[6]= "*2\r\n$3\r";
//     str_arr[7]= "*2\r\n$3\r\n";
//     str_arr[8]= "*2\r\n$3\r\nf";
//     str_arr[9]= "*2\r\n$3\r\nfo";
//     str_arr[10]= "*2\r\n$3\r\nfoo";
//     str_arr[11]= "*2\r\n$3\r\nfoo\r";
//     str_arr[12]= "*2\r\n$3\r\nfoo\r\n";
//     str_arr[13]= "*2\r\n$3\r\nfoo\r\n$";
//     str_arr[14]= "*2\r\n$3\r\nfoo\r\n$4";
//     str_arr[15]= "*2\r\n$3\r\nfoo\r\n$4\r";
//     str_arr[16]= "*2\r\n$3\r\nfoo\r\n$4\r\n";
//     str_arr[17]= "*2\r\n$3\r\nfoo\r\n$4\r\na";
//     str_arr[18]= "*2\r\n$3\r\nfoo\r\n$4\r\nar";
//     str_arr[19]= "*2\r\n$3\r\nfoo\r\n$4\r\naru";
//     str_arr[20]= "*2\r\n$3\r\nfoo\r\n$4\r\narun";
//     str_arr[21]= "*2\r\n$3\r\nfoo\r\n$4\r\narun\r";
//     str_arr[22]= "*2\r\n$3\r\nfoo\r\n$11\r\narunkaushi\r\n";
//     str_arr[23]= "*2\r\n$3\r\nfoo\r\n$11\r\narunkaushikk\r\n";
//     str_arr[24]= "*2\r\n$3\r\nfoo\r\n$11\r\narunkaushikk\r\n";
//     str_arr[25]= "*3\r\n$3\r\nfoo\r\n$11\r\narunkaushik\r\n";
//     str_arr[26]= "*2\r\n$1\r\nfoo\r\n$11\r\narunkaushik\r\n";
//     str_arr[27]= "*2\r\n$4\r\nfoo\r\n$11\r\narunkaushik\r\n";
//     str_arr[28]= "*2\r\n$3\r\nfoo\r\n$10\r\narunkaushik\r\n";
//     str_arr[29]= "*2\r\n$3\r\nfoo\r\n$12\r\narunkaushik\r\n";
//     str_arr[30]= "*4\r\n$3\r\nfoo\r\n$11\r\narunkaushik\r\n";
//     str_arr[31]= "*2\r\n$300\r\nfoo\r\n$11\r\narunkaushik\r\n";
//     str_arr[32]= "*2\r\n$-3\r\nfoo\r\n$11\r\narunkaushik\r\n";
//     str_arr[33]= "*2\r\n$v\r\nfoo\r\n$11\r\narunkaushik\r\n";
//     str_arr[34]= "*5gf55\r\n$3\r\nfoo\r\n$11\r\narunkaushik\r\n";
//     str_arr[35]= "*\r\n$3\r\nfoo\r\n$11\r\narunkaushik\r\n";
//     str_arr[36]= "*2\r\n$3\r\nfoo\r\n\n$11\r\narunkaushik\r\n";
//     str_arr[37]= "*2\r\r\n$3\r\nfoo\r\n$11\r\narunkaushik\r\n";
//     str_arr[38]= "*2\r\n$3\r\nfoo\r\n$11\r\r\r\r\r\r\narunkaushik\r\n";
//     str_arr[39]= "*2\r\n$3\r\nfoo\r\r\r\n$11\r\r\r\r\r\r\narunkaushik\r\r\r\n";
//     str_arr[40]= "*2\r\n$3\r\nfoo\r\n$11\r\narunkaushik\r\n";
//     str_arr[41]= "*2\r\n$3\r\nfoo\r\n$12\r\narun\nkaushik\r\n";
//     str_arr[42]= "*8\r\n$3\r\nset\r\n$3\r\nfoo\r\n$3\r\nbar\r\n$3\r\nabc\r\n$3\r\nabc\r\n$3\r\nabc\r\n$3\r\nabc\r\n$3\r\nfoo\r\n";

//     //str_arr[22]= "*2\r\n$3\r\nfoo\r\n$4\r\narun\r\n";
//     unsigned long len;
//     n = 0;
//     //printf("%s %d\n","for r ->" , '\r' );
//     //printf("%s %d\n","for n ->" , '\n' );
//     while(n < 43){
//         str = str_arr[n];
//         len = strlen(str);
//         //printf("%s %lu\n", "Strlen is ", len );
//         tokens = respTokenizer(str, len);
//         if(tokens && tokens->size){
//             node = tokens->head;
//             while(node){
//                 printf("%d %c %s ", n , ' ', node->key->buf);
//                 node = node->next;
//             }
//             printf("\n");
//         } else {
//             printf("%d %s\n", n, " ILLEGAL REPS STRING" );
//         }
//         freeLinkedList(tokens);
//         n++;
//     }
//         return 0;
// }