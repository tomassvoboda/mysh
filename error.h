#ifndef MYSH_ERROR_H
#define MYSH_ERROR_H

void err_ret(const char* fmt, ...);

void err_sys(const char* fmt, ...);

void err_msg(const char* fmt, ...);

#endif /* MYSH_ERROR_H */