#ifndef MYSH_COMMAND_H
#define MYSH_COMMAND_H

typedef struct command {
    char* name;
    char** args;
} command_t;

#include "queue.h"

command_t* command_initialize(const char* const name,
                              struct arg_queue_head* const arg_queue);

void command_execute(command_t* command);

#endif /* MYSH_COMMAND_H */