#ifndef CONSTANTS_H_   /* Include guard */
#define CONSTANTS_H_

#define SERVER_PORT 1500
#define INITIAL_SIZE 64

#define FAILURE -1
#define SUCCESS 1

#define HASH_LEN 4

#define EXOSTRING 0
#define EXOCMD 1

// ERRORS

#define COMMAND_NOT_FOUND -1
#define WRONG_NUMBER_OF_ARGUMENTS -2
#define WRONG_TYPE_OF_COMMAND_ON_TARGET_OBJECT -3


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