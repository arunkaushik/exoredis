#include "resp.h"

/*
It is a string parser which implements RESP protocol to create meaning
out of the string passed. It is used for Commands read from RESP clients

returns a linked list of tokens. Tokens are simply listnode with key as
the token and value as void.

It is a fast and efficient parser due to RESP.

It returns empty linkedlist which is treated an an ERR message if the passed
string is illegal according to RESP
*/

linkedList* respTokenizer(char *str, unsigned long len){
    linkedList *tokens = newLinkedList();
    // YET TO IMPLEMENT
    return tokens;
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
linkedList* simpleTokenizer(char *str, unsigned long len){
    if(len < 2){
        return NULL;
    }

    printf(MAG "-------------------------- SIMPLE TOKENIZER BEGIN ------------------------\n" RESET);

    linkedList* tokens = newLinkedList();
    listNode* node;
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
            node = newNode(tkn, NULL);
            addNodeToList(tokens, node);
        }
        str = runner;
    }
    
    printf(MAG "-------------------------- SIMPLE TOKENIZER ENDS ------------------------\n" RESET);
    return tokens;
}
