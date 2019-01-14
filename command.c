#define _XOPEN_SOURCE 700

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtin.h"
#include "command.h"
#include "error.h"
#include "utils.h"

command_t* command_initialize(const char* const name,
                              arg_queue_head_t* const arg_queue)
{
    command_t* new_command = malloc(sizeof *new_command);
    if (!new_command) {
        err_sys("malloc error when allocating new command memory");
    }
    new_command->name = strdup(name);

    char** args;
    size_t len = 2;
    arg_queue_entry_t* entry;
    STAILQ_FOREACH(entry, arg_queue, entries) { len++; }
    args = malloc(len * sizeof(char*));
    if (!args) {
        err_sys("malloc error when allocating command arguments array");
    }

    *args = strdup(name);
    size_t index = 1;
    STAILQ_FOREACH(entry, arg_queue, entries)
    {
        *(args + index) = strdup(entry->argument);
        index++;
    }
    *(args + index) = NULL;

    new_command->args = args;
    return new_command;
}

void command_execute(command_t* const command)
{
    if (!strcmp(command->name, "exit")) {
        exit(ret_val);
    } else if (!strcmp(command->name, "cd")) {
        ret_val = builtin_cd(command->args);
    } else {
        pid_t pid;
        int status;

        sigset_t sigs;
        if (sigemptyset(&sigs) == -1)
            err_sys("sigemptyset error");
        if (sigaddset(&sigs, SIGINT) == -1)
            err_sys("sigaddset error");
        if (sigprocmask(SIG_UNBLOCK, &sigs, NULL) == -1)
            err_sys("sigprocmask error");

        switch (pid = fork()) {
        case -1:
            err_sys("fork error");
            break;
        case 0:
            /* child */
            execvp(command->name, command->args);
            err_ret("mysh: %s", command->name);
            exit(127);
        default:
            /* parent */
            if (sigprocmask(SIG_SETMASK, &sigs, NULL) == -1)
                err_sys("sigprocmask error");
            if ((pid = waitpid(pid, &status, 0)) < 0)
                err_sys("waitpid error");
            if (WIFSIGNALED(status)) {
                err_msg("Killed by signal %d", WTERMSIG(status));
                ret_val = 128 + WTERMSIG(status);
            } else {
                ret_val = WEXITSTATUS(status);
            }
            break;
        }
    }
}