#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "queue.h"
#include "utils.h"

arg_queue_head_t* arg_queue_initialize(void)
{
    arg_queue_head_t* new_head = malloc(sizeof *new_head);
    if (!new_head) {
        err_sys("malloc error when allocating new arguments queue memory");
    }
    STAILQ_INIT(new_head);
    return new_head;
}

void arg_queue_push_back(arg_queue_head_t* const head, const char* argument)
{
    arg_queue_entry_t* new_entry = malloc(sizeof *new_entry);
    if (!new_entry) {
        err_sys("malloc error when allocating new argument memory");
    }
    new_entry->argument = strdup(argument);
    STAILQ_INSERT_TAIL(head, new_entry, entries);
}

void arg_queue_clear(arg_queue_head_t* const head)
{
    arg_queue_entry_t* entry1 = STAILQ_FIRST(head);
    while (entry1) {
        arg_queue_entry_t* entry2 = STAILQ_NEXT(entry1, entries);
        free(entry1->argument);
        free(entry1);
        entry1 = entry2;
    }
    free(head);
}

cmd_queue_head_t* cmd_queue_initialize(void)
{
    cmd_queue_head_t* new_head = malloc(sizeof *new_head);
    if (!new_head) {
        err_sys("malloc error when allocating new commands queue memory");
    }
    STAILQ_INIT(new_head);
    return new_head;
}

void cmd_queue_push_back(cmd_queue_head_t* const head, command_t* const command)
{
    cmd_queue_entry_t* new_entry = malloc(sizeof *new_entry);
    if (!new_entry) {
        err_sys("malloc error when allocating new command memory");
    }
    new_entry->command = command;
    STAILQ_INSERT_TAIL(head, new_entry, entries);
}

void cmd_queue_clear(cmd_queue_head_t* const head)
{
    cmd_queue_entry_t* entry1 = STAILQ_FIRST(head);
    while (entry1) {
        cmd_queue_entry_t* entry2 = STAILQ_NEXT(entry1, entries);
        free(entry1->command->name);
        size_t index = 0;
        char** args_ptr = entry1->command->args;
        while (*(args_ptr + index)) {
            free(*(args_ptr + index++));
        }
        free(entry1->command->args);
        free(entry1->command);
        free(entry1);
        entry1 = entry2;
    }
    free(head);
}

void cmd_queue_execute(cmd_queue_head_t* const head)
{
    cmd_queue_entry_t* entry;
    STAILQ_FOREACH(entry, head, entries) { command_execute(entry->command); }
}

job_queue_head_t* job_queue_initialize(void)
{
    job_queue_head_t* new_head = malloc(sizeof *new_head);
    if (!new_head) {
        err_sys("malloc error when allocating new jobs queue memory");
    }
    STAILQ_INIT(new_head);
    return new_head;
}

void job_queue_push_back(job_queue_head_t* const head,
                         cmd_queue_head_t* const cmd_queue)
{
    job_queue_entry_t* new_entry = malloc(sizeof *new_entry);
    if (!new_entry) {
        err_sys("malloc error when allocating new job memory");
    }
    new_entry->cmd_queue = cmd_queue;
    STAILQ_INSERT_TAIL(head, new_entry, entries);
}

void job_queue_clear(job_queue_head_t* const head)
{
    job_queue_entry_t* entry1 = STAILQ_FIRST(head);
    while (entry1) {
        job_queue_entry_t* entry2 = STAILQ_NEXT(entry1, entries);
        cmd_queue_clear(entry1->cmd_queue);
        free(entry1);
        entry1 = entry2;
    }
    free(head);
}

void job_queue_execute(job_queue_head_t* const head)
{
    job_queue_entry_t* entry;
    STAILQ_FOREACH(entry, head, entries)
    {
        cmd_queue_execute(entry->cmd_queue);
    }
}
