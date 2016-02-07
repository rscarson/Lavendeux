%{
	#include <stdio.h>
	#include <stdlib.h>
	//#include "parse.h"

	int yylex(void);
	int yyerror(char*);
%}

/* Valid tokens */
%token IDENTIFIER HEX BIN OCT SCI FLOAT INT
%token COMMA DECORATOR EQUAL LPAREN RPAREN
%left OR
%left XOR
%left AND
%left LSHIFT RSHIFT
%left PLUS MINUS
%left MUL DIV MOD
%left POW
%left FACTORIAL
%left NOT

%glr-parser
%%

expression:
	assignment_expression { printf("expression/assignment_expression\n"); }
	| expression DECORATOR IDENTIFIER { printf("DECORATOR\n"); }

value:
	IDENTIFIER { printf("IDENTIFIER\n"); }
	| HEX  { printf("HEX\n"); }
	| BIN  { printf("BIN\n"); }
	| OCT { printf("OCT\n"); }
	| SCI  { printf("SCI\n"); }
	| FLOAT  { printf("FLOAT\n"); }
	| INT { printf("%s\n", $1); }
	;

constant_expression:
	value { printf("value\n"); }
	| LPAREN constant_expression RPAREN { printf("parened expression\n"); }
	| constant_expression OR constant_expression { printf("OR\n"); }
	| constant_expression XOR constant_expression { printf("XOR\n"); }
	| constant_expression AND constant_expression { printf("AND\n"); }
	| constant_expression LSHIFT constant_expression { printf("LSHIFT\n"); }
	| constant_expression RSHIFT constant_expression { printf("RSHIFT\n"); }
	| constant_expression PLUS constant_expression { printf("%s PLUS %s\n", $1, $3); }
	| constant_expression MINUS constant_expression { printf("%s MINUS %s\n", $1, $3); }
	| constant_expression MUL constant_expression { printf("%s MUL %s\n", $1, $3); }
	| constant_expression DIV constant_expression { printf("%s DIV %s\n", $1, $3); }
	| constant_expression MOD constant_expression { printf("%s MOD %s\n", $1, $3); }
	| constant_expression POW constant_expression { printf("%s POW %s\n", $1, $3); }
	| constant_expression FACTORIAL { printf("FACTORIAL\n"); }
	| NOT constant_expression { printf("NOT\n"); }
	| IDENTIFIER LPAREN RPAREN { printf("f()\n"); }
	| IDENTIFIER LPAREN constant_expression RPAREN { printf("f(x)\n"); }
	| IDENTIFIER LPAREN expression_list RPAREN { printf("f(x,y)\n"); }
	;

expression_list:
	constant_expression COMMA constant_expression { printf("1+1,1+1\n"); }
	| expression_list COMMA constant_expression { printf("1+1,1+1,...\n"); }
	;

assignment_expression:
	constant_expression { printf("constant_expression/assignment_expression\n"); }
	| IDENTIFIER EQUAL assignment_expression { printf("x=\n"); }
	| IDENTIFIER LPAREN RPAREN EQUAL assignment_expression { printf("f()=x\n"); }
	| IDENTIFIER LPAREN IDENTIFIER RPAREN EQUAL assignment_expression { printf("f(x)=\n"); }
	| IDENTIFIER LPAREN identifier_list RPAREN EQUAL assignment_expression { printf("f(x,y)=\n"); }
	;

identifier_list:
	IDENTIFIER COMMA IDENTIFIER { printf("x,y\n"); }
	| identifier_list COMMA IDENTIFIER { printf("x,y,...\n"); }
	;

%%

#include <stdio.h>
#include <stdlib.h>

extern int yylex();
extern int yyparse();


int yyerror(char *s)
{
	extern int yylineno;
	extern char *yytext;

	printf("%s at symbol '%c', line %d", s, *yytext, yylineno);
	return 1;
}

