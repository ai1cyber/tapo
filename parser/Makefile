CC=gcc
YACC=bison
LEX=flex
CFLAGS=-g -Wall
RULE_PARSER=parser
RULE_FILE=rule.txt
SOURCE=rule_parser.c rule_parser.h

all: $(RULE_PARSER) rules.txt
	./$(RULE_PARSER) < rules.txt
	@mv $(SOURCE) ../

$(RULE_PARSER): lex.yy.o parser.tab.o
	$(CC) -g -o parser lex.yy.o parser.tab.o -lfl

lex.yy.o: parser.tab.h lex.yy.c
	$(CC) -g -c lex.yy.c -o lex.yy.o

parser.tab.o: parser.tab.c
	$(CC) -g -c parser.tab.c -o parser.tab.o

parser.tab.c: parser.y
	$(YACC) -d parser.y

parser.tab.h: parser.y
	$(YACC) -d parser.y

lex.yy.c: parser.lex
	$(LEX) parser.lex

clean:
	@rm -f *.o parser parser.tab.[ch] lex.yy.c
