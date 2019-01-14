#define _XOPEN_SOURCE 700

#include <fcntl.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <readline/history.h>
#include <readline/readline.h>

#include "error.h"
#include "lex.yy.h"
#include "parser.tab.h"
#include "queue.h"
#include "utils.h"

char* get_prompt(void)
{
    char* cwd = getcwd(NULL, 0);
    if (!cwd) {
        err_sys("getcwd error");
    }
    size_t res_len = strlen(cwd) + 8;
    char* res = malloc(res_len * sizeof(char));
    snprintf(res, res_len, "mysh:%s$ ", cwd);
    free(cwd);
    return res;
}

char* interactive_line(void)
{
    sigset_t sigs, osigs;
    if (sigemptyset(&sigs) == -1)
        err_sys("sigemptyset");
    if (sigaddset(&sigs, SIGINT) == -1)
        err_sys("sigaddset");
    if (sigprocmask(SIG_SETMASK, &sigs, &osigs) == -1)
        err_sys("sigprocmask");

    char* prompt = get_prompt();
    if (sigprocmask(SIG_UNBLOCK, &sigs, NULL) == -1)
        err_sys("sigprocmask");
    char* line = readline(prompt);
    if (sigprocmask(SIG_SETMASK, &sigs, NULL) == -1)
        err_sys("sigprocmask");
    free(prompt);

    if (line && *line) {
        add_history(line);
    }
    if (sigprocmask(SIG_UNBLOCK, &osigs, NULL) == -1)
        err_sys("sigprocmask");
    return line;
}

void parse_line(char* line)
{
    YY_BUFFER_STATE s = yy_scan_string(line);
    yyparse();
    yy_delete_buffer(s);
}

void sig_handler(int sig) {}

int readline_line_reset(void)
{
    rl_crlf();
    rl_initialize();
    rl_redisplay();
    return 0;
}

void script_line(char* path)
{
    char* full_path = realpath(path, NULL);
    if (!full_path) {
        err_sys("realpath error");
    }

    char* cwd = getcwd(NULL, 0);
    if (!cwd) {
        err_sys("getcwd error");
    }

    int fd;
    if ((fd = open(full_path, O_RDONLY)) == -1) {
        err_sys("open error");
    }
    free(full_path);

    char buf[MAXLINE];
    char* line = NULL;
    int r;

    line_num = 0;
    while ((r = read(fd, buf, MAXLINE)) > 0) {
        int pos = 0;
        int old_pos = -1;

        while (pos < r) {
            if (buf[pos] == '\n') {
                int len = pos - old_pos;
                line = append_string(line, buf + old_pos + 1, len);

                line_num++;
                parse_line(line);
                free(line);
                line = NULL;
                old_pos = pos;

                if (ret_val == 254) {
                    exit(ret_val);
                }
            }

            pos++;
        }
        int len = pos - old_pos - 1;
        line = append_string(line, buf + old_pos + 1, len);
    }

    if (r == 0 && line != NULL) {
        line_num++;
        parse_line(line);
        free(line);
        line = NULL;

        if (ret_val == 254) {
            exit(ret_val);
        }
    } else if (r == -1) {
        err_sys("read error");
    }

    if (close(fd) == -1) {
        err_sys("close error");
    }

    if (chdir(cwd) == -1) {
        err_sys("chdir error");
    }
}

int main(int argc, char** argv)
{
    struct sigaction act = {0};
    act.sa_handler = sig_handler;
    if (sigaction(SIGINT, &act, NULL) == -1)
        err_sys("sigaction");

    rl_signal_event_hook = readline_line_reset;

    int opt;
    while ((opt = getopt(argc, argv, "c:")) != -1) {
        switch (opt) {
        case 'c': {
            char* line = NULL;
            if (!(line = strdup(optarg)))
                err_sys("malloc error when allocating memory for -c optarg");
            parse_line(line);
            free(line);
            return ret_val;
            break;
        }
        }
    }
    if (argv[optind] == NULL) {
        char* line = NULL;
        do {
            free(line);
            line = interactive_line();
            if (line && *line) {
                parse_line(line);
            }
        } while (line);
        exit(0);

    } else {
        argv = &argv[optind];

        while (*argv) {
            script_line(*argv);
            argv++;
        }
    }
    exit(0);
}