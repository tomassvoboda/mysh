#ifndef MYSH_QUEUE_H
#define MYSH_QUEUE_H

#include <sys/queue.h>

struct arg_queue_head;

#include "command.h"

/* Struct definitions related to queue of arguments */

typedef STAILQ_HEAD(arg_queue_head, arg_queue_entry) arg_queue_head_t;

typedef struct arg_queue_entry {
    char* argument;
    STAILQ_ENTRY(arg_queue_entry) entries;
} arg_queue_entry_t;

/* Function definitions */

arg_queue_head_t* arg_queue_initialize(void);

void arg_queue_push_back(arg_queue_head_t* const head, const char* argument);

void arg_queue_clear(arg_queue_head_t* const head);

/* Struct definitions related to queue of commands */

typedef STAILQ_HEAD(cmd_queue_head, cmd_queue_entry) cmd_queue_head_t;

typedef struct cmd_queue_entry {
    command_t* command;
    STAILQ_ENTRY(cmd_queue_entry) entries;
} cmd_queue_entry_t;

/* Function definitions */

cmd_queue_head_t* cmd_queue_initialize(void);

void cmd_queue_push_back(cmd_queue_head_t* const head,
                         command_t* const command);

void cmd_queue_clear(cmd_queue_head_t* const head);

/* Struct definitions related to queue of jobs */

typedef STAILQ_HEAD(job_queue_head, job_queue_entry) job_queue_head_t;

typedef struct job_queue_entry {
    cmd_queue_head_t* cmd_queue;
    STAILQ_ENTRY(job_queue_entry) entries;
} job_queue_entry_t;

/* Function definitions */

job_queue_head_t* job_queue_initialize(void);

void job_queue_push_back(job_queue_head_t* const head,
                         cmd_queue_head_t* const cmd_queue);

void job_queue_clear(job_queue_head_t* const head);

void cmd_queue_execute(cmd_queue_head_t* const head);

void job_queue_execute(job_queue_head_t* const head);

#endif /* MYSH_QUEUE_H */