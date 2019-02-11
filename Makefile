LEX = flex
YACC = bison -d -t
CC = gcc

LIBS = -lm 
CCFLAGS = -Wall -ggdb

OBJ = analyseur_lexical_flex.o util.o

all: compilo

compilo: compilo.c analyseur_syntaxique.tab.o analyseur_lexical_flex.o
		$(CC) -o $@ $^

analyseur_syntaxique.tab.c: analyseur_syntaxique.y
	$(YACC) $<

analyseur_lexical_flex.c: analyseur_lexical.flex analyseur_syntaxique.tab.c
	$(LEX) -o $@ $<

%.o: %.c
	$(CC) $(CCFLAGS) -c $^

.PHONY : clean

clean:
	- rm -f $(OBJ)
	- rm -f compilo
	- rm -f test_yylex
	- rm -f analyseur_lexical_flex.c
	- rm -f analyseur_syntaxique.tab.c
