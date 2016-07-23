#ifndef CONSTANTS_H_   /* Include guard */
#define CONSTANTS_H_

#define SERVER_PORT 15000
#define INITIAL_SIZE 1024
#define OFFSET_MAX 4294967295
#define FAILURE -1
#define SUCCESS 1

#define HASH_LEN 4

#define SIMPLE_STRING 0
#define RESP_ERROR 1
#define RESP_INTEGER 2
#define BULKSTRING 3
#define RESP_ARRAY 4

#define EXOCMD 101
#define BITMAP 102

// ERRORS

#define COMMAND_NOT_FOUND -1
#define WRONG_NUMBER_OF_ARGUMENTS -2
#define WRONG_TYPE_OF_COMMAND_ON_TARGET_OBJECT -3
#define PROTOCOL_ERROR -4
#define FAILED_TO_FLUSH_DB -5
#define OFFSET_NOT_INT_OR_OUT_OF_RANGE -6
#define BIT_NOT_INT_OR_OUT_OF_RANGE -7


// LOGGING
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"


#endif