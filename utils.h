#ifndef MYSH_UTILS_H
#define MYSH_UTILS_H

#define MAXLINE 4096 /* max line length */

int ret_val;

int line_num;

char* append_string(char* existing, const char* new, int len);

char* strdup(const char* const existing);

char* get_prompt(void);

#endif /* MYSH_UTILS_H */