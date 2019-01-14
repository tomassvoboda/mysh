%{
#include "parser.tab.h"
%}

%option noyywrap nodefault

%%

;                                       { return SEPARATOR; }
\|                                      { return PIPE; }
\<                                      { return REDIR_IN; }
\>                                      { return REDIR_OUT; }
\n                                      { return EOL; }
[ \t]+                                  { /* ignore whitespace */ }
#.*                                     { /* ignore comment */ }

\"([^"\n\\]|\\(.|\n))*\"                { yylval.string = strdup(yytext); return STRING; }
'([^'\n\\]|\\(.|\n))*'                  { yylval.string = strdup(yytext); return STRING; }
([^ \t\n#;|]|(\\[ \t\n#;|]))+           { yylval.string = strdup(yytext); return STRING; }

%%
