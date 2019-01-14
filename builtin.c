#define _XOPEN_SOURCE 700

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtin.h"
#include "error.h"

static void set_pwds(char* const pwd, char* const old_pwd)
{
    if (setenv("PWD", pwd, 1) == -1)
        err_sys("setenv(PWD) error");
    if (setenv("OLDPWD", old_pwd, 1) == -1)
        err_sys("setenv(OLDPWD) error");
}

static int builtin_cd_home(void)
{
    char* cwd = getcwd(NULL, 0);
    if (!cwd) {
        err_sys("getcwd error");
    }
    char* home = getenv("HOME");
    if (!home) {
        err_msg("mysh: cd: HOME not set");
        return 1;
    }
    if (chdir(home) == -1) {
        err_sys("chdir error");
    }
    set_pwds(home, cwd);
    free(cwd);
    return 0;
}

int builtin_cd(char** const args)
{
    if (*(args + 1) == NULL) {
        return builtin_cd_home();
    } else if (*(args + 2) == NULL) {
        char* dest;
        if (!strcmp(*(args + 1), "-")) {
            if (!(dest = getenv("OLDPWD"))) {
                err_msg("mysh: cd: OLDPWD not set");
                return 1;
            }
        } else {
            dest = realpath(*(args + 1), NULL);
            if (!dest) {
                err_ret("mysh: cd: %s", *(args + 1));
                return 1;
            }
        }

        char* cwd = getcwd(NULL, 0);
        if (!cwd) {
            err_sys("getcwd error");
        }

        if (chdir(dest)) {
            err_ret("mysh: cd: %s", *(args + 1));
            free(cwd);
            return 1;
        } else {
            set_pwds(dest, cwd);
            free(cwd);
            return 0;
        }
    } else {
        err_msg("mysh: cd: too many arguments");
        return 1;
    }
}