all: tapo

TARGET=tapo
PARSER_DIR=./parser
RULE_PARSER=rule_parser

CC=gcc
YACC=bison
LEX=flex
CFLAGS=-g -Wall
LD=gcc
LDFLAGS=
LIBS=-lpcap
CTAGS=ctags

HEADER=$(wildcard *.h)
SRC=$(wildcard *.c)
ifeq (,$(findstring $(RULE_PARSER), $(SRC)))
	HEADER+=$(RULE_PARSER).h
	SRC+=$(RULE_PARSER).c
endif
OBJS=$(patsubst %.c, %.o, $(SRC))

$(RULE_PARSER).h: $(RULE_PARSER).c

$(RULE_PARSER).c: $(PARSER_DIR)/Makefile $(PARSER_DIR)/parser.y $(PARSER_DIR)/parser.lex $(PARSER_DIR)/rules.txt
	cd $(PARSER_DIR); make 

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@  

tapo: $(OBJS)
	$(LD) $(LDFLAGS) $(LIBS) $(OBJS) -o $(TARGET)

tags: $(wildcard *.[hc]) 
	$(CTAGS) $(wildcard *.[hc])

clean:
	@rm -f *.o tapo $(RULE_PARSER).*
	cd $(PARSER_DIR); make clean
