CSTD=c99
CFLAGS=-std=$(CSTD) -Wall -Wextra -Wpedantic -Wshadow -Wcast-qual \
-Wstrict-prototypes -Werror=implicit-function-declaration
LFLAGS=-lfl -lreadline

PROG = mysh
TEMPFILES = core core.* *.o temp.* *.out

$(PROG): parser.y lexer.l utils.o error.o queue.o command.o builtin.o
	bison -d parser.y
	flex --header-file=lex.yy.h lexer.l
	$(CC) $(CFLAGS) -c main.c
	$(CC) -o $@ main.o utils.o error.o queue.o command.o builtin.o parser.tab.c lex.yy.c $(LFLAGS)


.PHONY: debug
debug: CFLAGS += -g
debug: $(PROG)

.PHONY: clean
clean:
	rm -f $(PROG) $(TEMPFILES) *.o *.tab.h *.tab.c *.yy.h *.yy.c