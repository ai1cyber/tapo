%{
#include "parser.tab.h"
%}

number [0-9]+
blanks [ \t\n]+
cause [_A-Z0-9]+
variable [_a-zA-Z]+

addop [\+\-]
mulop [\*\/\%]
cmpop ("<"|">"|"<="|">="|"==")
logop ("&&"|"||")

string L?\"(\\.|[^\\"])*\"

%%

{blanks} {}
"condition" return (CONDITION);
"true" return (TRUE);
"false" return (FALSE);
"type" return (TYPE);
"cause" return (CAUSE);
"detail" return(DETAIL);
"!" return(NOT);
"(" return(OP);
")" return(CP);

{number} {
	yylval.sval = malloc(strlen(yytext)+1);
	sprintf(yylval.sval, "%s", yytext); 
	return(NUMBER);
}

{cause} {
	yylval.sval = malloc(strlen(yytext)+1);
	sprintf(yylval.sval, "%s", yytext); 
	return(CAUSE);
}

{variable} {
	yylval.sval = malloc(strlen(yytext)+strlen("tss->")+1);
	sprintf(yylval.sval, "%s%s", "tss->", yytext); 
	return(IDENTIFIER);
}

{string} {
	yylval.sval = malloc(strlen(yytext)+1);
	sprintf(yylval.sval, "%s", yytext); 
	return(STRING);
}

{addop} {
	yylval.sval = malloc(strlen(yytext)+1);
	sprintf(yylval.sval, "%s", yytext); 
	return(ADDOP);
}

{mulop} {
	yylval.sval = malloc(strlen(yytext)+1);
	sprintf(yylval.sval, "%s", yytext); 
	return(MULOP);
}

{cmpop} {
	yylval.sval = malloc(strlen(yytext)+1);
	sprintf(yylval.sval, "%s", yytext); 
	return(CMPOP);
}

{logop} {
	yylval.sval = malloc(strlen(yytext)+1);
	sprintf(yylval.sval, "%s", yytext); 
	return(LOGOP);
}
