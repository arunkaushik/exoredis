#include "commands.h"

hashTable *COMMANDS;

struct exoCmd commandTable[] = {
  {"GET", 1, false, get},
  {"SET", 2, true, set},
};

int initializeCmdTable(){
  COMMANDS = newHashTable(10);
  int i,n = sizeof(commandTable)/sizeof(struct exoCmd);
  for (i = 0; i < n; i++) {
    exoCmd *c = commandTable+i;
    if(addCommand(COMMANDS, c) == NULL){
        return FAILURE;
    }
  }
  return SUCCESS;
}

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

int main(){
    initializeCmdTable();
    int n;
    exoVal* cmd_node;
    exoCmd *cmd;
    unsigned long l;
    char str[1000];
    scanf("%d", &n);
    while(n--){
        scanf("%s", str);
        l = strlen(str);
        exoString *key = newString(str, l);
        cmd_node = get(COMMANDS, key);

        if(cmd_node){
            if(cmd_node->ds_type == EXOCMD){
                cmd = (exoCmd*)cmd_node->val_obj;
                printf("%s %zu %d\n", cmd->cmd_str, cmd->args_count, cmd->variable_arg_count);
            } else {
                printf("%s\n", "NOT A COMMAND" );
            }
        } else {
            printf("%s\n", "COMMAND NOT FOUND" );
        }
    }
}