#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "error.h"
#include "utils.h"

static char* concat(char* first, ...)
{
    va_list ap;
    va_start(ap, first);
    size_t res_len = strlen(first);
    char* res = malloc(res_len * sizeof(char));
    strncpy(res, first, res_len);
    for (;;) {
        char* val = va_arg(ap, char*);
        if ((val == NULL) || (val[0] == '\0')) {
            break;
        }
        size_t val_len = strlen(val);
        char* new_res = (char*)realloc(res, res_len + val_len);
        if (new_res == NULL) {
            free(res);
            return NULL;
        }
        res = new_res;
        strncpy(res + res_len, val, val_len);
        res_len += val_len;
    }
    va_end(ap);

    return res;
}

char* append_string(char* existing, const char* new, int len)
{
    char* added = malloc((len + 1) * sizeof(char));
    if (!added) {
        err_sys("malloc");
    }

    for (int i = 0; i < len; i++) {
        *(added + i) = *(new + i);
    }
    *(added + len) = '\0';

    if (!existing) {
        existing = added;
    } else {
        char* new_line = concat(existing, added);
        free(existing);
        free(added);
        existing = new_line;
    }
    return existing;
}

char* strdup(const char* const existing)
{
    if (!existing) {
        return (char*)NULL;
    }

    int len = strlen(existing) + 1;
    char* res = malloc(len * sizeof(char));
    if (res == NULL) {
        err_sys("malloc error when allocating string duplicate");
    }
    strcpy(res, existing);

    return res;
}
