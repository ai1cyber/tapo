%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

const char *dir = "./";
const char *src_name = "rule_parser";
FILE *cfile = NULL;
FILE *hfile = NULL;

int maxindent = 3;
int indent = 0;

char *enums = NULL;
char *texts = NULL;
char *details = NULL;
char *body = NULL;

char *print_string(const char *, ...);
void fill_tabs(char *, int);
void append_to_string(char **string, const char *str);
%}

// Symbols.
%union
{
	char *sval;
}

%token CONDITION TRUE FALSE TYPE CAUSE DETAIL
%token IDENTIFIER STRING NUMBER 
%token CMPOP ADDOP MULOP LOGOP NOT OP CP

%left GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/'

%type<sval> CONDITION TRUE FALSE TYPE CAUSE DETAIL IDENTIFIER STRING ADDOP MULOP CMPOP LOGOP OP CP NUMBER NOT
%type<sval> infer logexprs logexpr cmpexpr expr factor term

%%

program:
	declares infer { append_to_string(&body, $2); };

declares:
	|
	declares declare;

declare:
	TYPE CAUSE DETAIL STRING {
		char buff[1024];
		sprintf(buff, "%s, ", $2);
		append_to_string(&enums, buff);
		sprintf(buff, "\"%s\", ", $2);
		append_to_string(&texts, buff);
		sprintf(buff, "\t%s,\n", $4);
		append_to_string(&details, buff);
	};
	
infer:
	CONDITION logexprs
		TRUE infer 
		FALSE infer
		{
			char tabs[1024] = "";
			// fill_tabs(tabs, maxindent-indent);
			// fprintf(stderr, "logexprs: %s\n", $2);

			$$ = print_string("%sif (%s) {\n%s%s\n%s} else {\n%s%s\n%s}", tabs, $2, tabs, $4, tabs, tabs, $6, tabs, NULL);
			indent += 1;
		}
	| TYPE CAUSE
		{
			char tabs[1024] = "";
			// fill_tabs(tabs, maxindent-indent+1);
			$$ = print_string("%sreturn %s;", tabs, $2, NULL);
		}
	;

logexprs:
	logexprs LOGOP logexpr { $$ = print_string("%s %s %s", $1, $2, $3, NULL); }
	| logexpr { $$ = print_string("%s", $1, NULL); }
	;

logexpr:
	NOT logexpr { $$ = print_string("!%s", $2, NULL); }
	| cmpexpr { $$ = $1; }
	;

cmpexpr:
	expr CMPOP expr { $$ = print_string("%s %s %s", $1, $2, $3, NULL); }
	| expr { $$ = $1; }
	;

expr:
	expr ADDOP factor { $$ = print_string("%s %s %s", $1, $2, $3, NULL); }
	|  factor { $$ = $1; }
	;

factor: 
	factor MULOP term { $$ = print_string("%s %s %s", $1, $2, $3,NULL); }	
	| term { $$ = $1; }
	;

term:
	OP logexprs CP { $$ = print_string("(%s)", $2, NULL); }
	| NUMBER { $$ = $1; }
	| IDENTIFIER { $$ = $1; }
	;

%%

void init()
{
	char name[1024];
	sprintf(name, "%s/%s.c", dir, src_name);
	cfile = fopen(name, "w");
	if (cfile == NULL) {
		fprintf(stderr, "could not open %s for writing rules.", name); 
		exit(1);
	}

	// init c source file
	fprintf(cfile, "#include \"%s.h\"\n", src_name);
	fprintf(cfile, "\n\n");

	sprintf(name, "%s/%s.h", dir, src_name);
	hfile = fopen(name, "w");
	if (hfile == NULL) {
		fprintf(stderr, "could not open %s for writing rules.", name); 
		exit(1);
	}

	// init header file
	fprintf(hfile, "#ifndef __RULE_PARSER_H__\n");
	fprintf(hfile, "#define __RULE_PARSER_H__\n\n");
	fprintf(hfile, "#include \"tcp_stall_state.h\"\n\n");

	append_to_string(&enums, "enum stall_type { ");
	append_to_string(&texts, "const char *stall_text[] = {\n");
	append_to_string(&details, "const char *stall_details[] = {\n\t");
	append_to_string(&body, "enum stall_type parse_stall(struct tcp_stall_state *tss)\n{\n");
}

char *print_string(const char *fmt, ...)
{
	va_list args;
	const char *arg;
	int len = strlen(fmt);

	va_start(args, fmt);
	while ((arg = va_arg(args, const char *)) != NULL) {
		len += (strlen(arg) + 1);
	}
	va_end(args);

	char *str = (char *)malloc(len);
	va_start(args, fmt);
	vsprintf(str, fmt, args);
	va_end(args);

	return str;
}

void fill_tabs(char *tabs, int num)
{
	*tabs = '\0';
	while (num--)
		strcat(tabs, "");
}

void append_to_string(char **string, const char *str)
{
	if (*string == NULL) {
		*string = (char *)malloc(strlen(str) + 1);
		*string[0] = '\0';
	}
	else {
		*string = (char *)realloc(*string, strlen(*string) + strlen(str) + 1);
	}

	strcat(*string, str);
}

void finish()
{
	append_to_string(&enums, "};\n");
	append_to_string(&texts, "};\n");
	append_to_string(&details, "};\n");
	append_to_string(&body, "\n\treturn UNDETERMINED;\n}\n");

	fprintf(cfile, texts);
	fprintf(cfile, details);
	fprintf(cfile, body);

	fprintf(hfile, enums);
	fprintf(hfile, "extern const char *stall_text[];\n");
	fprintf(hfile, "extern const char *stall_details[];\n");
	fprintf(hfile, "extern enum stall_type parse_stall(struct tcp_stall_state *);\n");
	fprintf(hfile, "\n#endif\n");

	fclose(cfile);
	fclose(hfile);

	free(enums);
	free(details);
	free(body);
}

int yyerror(char *s)
{
	fprintf(stderr, "yyerror: %s\n", s);
	return 0;
}

int main()
{
	init();
	yyparse();
	finish();

	return 0;
}
