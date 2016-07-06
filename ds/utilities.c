#include "utilities.h"

exoString* newString(void *source, unsigned long len){
    exoString *str = (exoString*)malloc(sizeof (exoString)+ len + 1);
    if(!str){
        return str;
    } else {
        str->len = len;
        if (len) {
            memcpy(str->buf, source, len);
        }
        str->buf[len] = '\0';
        return str; 
    } 
}

exoVal* newExoVal(size_t ds_type, void *obj){
    exoVal *val = (exoVal*)malloc(sizeof (exoVal));
    if(!val){
        return val;
    } else {
        val->ds_type = ds_type;
        val->val_obj = obj;
        return val; 
    }
}


void printString(exoString* str){
    if(!str) return;
    else{
        printf("%s\n", str->buf );
        printf("%s %lu\n","Size: ", str->len);
    }
}

size_t stringHash(char *str){
  unsigned long hash = 5381;
  int c, l = PRIMARY_HASH_LEN;

  while (*str != '\0' && l-- > 0){
    c = *str++;
    hash = ((hash << 5) + hash) + c;
  }
  return hash % INITIAL_SIZE;
}
 
// int main(){
//     return 0;
// }