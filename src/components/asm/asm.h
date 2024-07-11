#ifndef asm_h
#define asm_h
#include <ti/getcsc.h>

sk_key_t getKey(void);

enum _user_input_flags
{
    INPUT_OBFUSCATE = (1 << 0), // input is a password
    INPUT_ASYNC = (1 << 1),     // should function async
};
uint8_t user_input(char *buffer, size_t length, uint8_t flags);

#endif