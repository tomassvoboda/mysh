%{
    #include <stdio.h>

    #include "error.h"
    #include "queue.h"
    #include "utils.h"

    char *yytext;
    int yylex();
    void yyerror(const char *msg);
%}

%union {
    char* string;
    struct arg_queue_head* args_ptr;
    struct command* command_ptr;
    struct cmd_queue_head* cmds_ptr;
    struct job_queue_head* jobs_ptr;
}

%token SEPARATOR PIPE REDIR_IN REDIR_OUT EOL
%token <string> STRING

%type <args_ptr> args_list
%type <command_ptr> command
%type <cmds_ptr> job
%type <jobs_ptr> line
%type <jobs_ptr> input

%%

start: input {
        job_queue_execute($1);
        job_queue_clear($1);
    }
    ;

input: %empty {
        $$ = job_queue_initialize();
    }
    | EOL {
        $$ = job_queue_initialize();
    }
    | line SEPARATOR EOL
    | line EOL
    | line SEPARATOR
    | line
    ;

line: job {
        job_queue_head_t* job_head = job_queue_initialize();
        job_queue_push_back(job_head, $1);
        $$ = job_head;
    }
    | line SEPARATOR job {
        job_queue_push_back($1, $3);
        $$ = $1;
    }
    ;

job: command {
        cmd_queue_head_t* cmd_head = cmd_queue_initialize();
        cmd_queue_push_back(cmd_head, $1);
        $$ = cmd_head;
    }
    ;

command: STRING {
        arg_queue_head_t* arg_head = arg_queue_initialize();
        $$ = command_initialize($1, arg_head);
        free($1);
        arg_queue_clear(arg_head);
    }
    | STRING args_list {
        $$ = command_initialize($1, $2);
        free($1);
        arg_queue_clear($2);
    }
    | command REDIR_IN STRING
    | command REDIR_OUT STRING
    ;

args_list: STRING {
        arg_queue_head_t* arg_head = arg_queue_initialize();
        arg_queue_push_back(arg_head, $1);
        free($1);
        $$ = arg_head;
    }
    | args_list STRING {
        arg_queue_push_back($1, $2);
        free($2);
        $$ = $1;
    }
    ;

%%

void yyerror(const char *msg)
{
    err_msg("error:%d: %s near unexpected token '%s'", line_num, msg, yytext);
    ret_val = 254;
}