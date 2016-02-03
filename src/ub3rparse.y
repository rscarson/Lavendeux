%{
	#include <stdio.h>
	#include <stdlib.h>
%}

/* Valid tokens */
%token IDENTIFIER HEX BIN OCT SCI FLOAT INT
%token PLUS MINUS MUL DIV MOD POW
%token NOT AND OR XOR LSHIFT RSHIFT
%token COMMA DECORATOR EQUAL LPAREN RPAREN NEWLINE

%%

input:
	expression

add_op:
	PLUS | MINUS
	;

bool_op:
	AND | OR | XOR | LSHIFT | RSHIFT
	;

mul_op:
	MUL | DIV | MOD | POW
	;

value:
	IDENTIFIER | HEX | BIN | OCT | SCI | FLOAT | INT
	;

base_expression:
	value
	| LPAREN base_expression RPAREN
	;

not_expression:
	base_expression
	| NOT base_expression
	;

bool_expression:
	not_expression
	| bool_expression bool_op bool_expression
	;

mul_expression:
	bool_expression
	| mul_expression mul_op mul_expression
	;

add_expression:
	mul_expression
	| add_expression add_op add_expression
	;

functioncall_expression:
	add_expression
	| IDENTIFIER LPAREN function_arguments RPAREN
	;

function_arguments:
	IDENTIFIER
	| function_arguments COMMA IDENTIFIER

assign_expression:
	functioncall_expression
	| IDENTIFIER EQUAL functioncall_expression
	| IDENTIFIER LPAREN function_arguments RPAREN EQUAL functioncall_expression
	;

expression:
	assign_expression
	| assign_expression DECORATOR IDENTIFIER

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

