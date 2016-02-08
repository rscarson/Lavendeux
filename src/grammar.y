%{
	#include <stdio.h>
	#include <stdlib.h>

	#include "parse.h"

	int yylex(void);
	int yyerror(char*);

	int is_stored_expression = 0;
	wchar_t *expression_name;
	wchar_t expression_text[];
	function stored_expression;
%}

/* Valid tokens */
%token value IDENTIFIER HEX BIN OCT SCI FLOAT INT
%token COMMA DECORATOR EQUAL LPAREN RPAREN
%token END 0 "end of expression"
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
	;

atomic_value:
	IDENTIFIER
	| HEX
	| BIN
	| OCT
	| SCI
	| FLOAT
	| INT
	;

constant_expression:
	atomic_value
	| LPAREN constant_expression RPAREN
	| constant_expression OR constant_expression {
		if (value_type($1) == VALUE_ERROR || value_type($3) == VALUE_ERROR)
		if (value_type($1) == VALUE_FLOAT || value_type($3) == VALUE_FLOAT)
	}
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
	| IDENTIFIER LPAREN RPAREN EQUAL assignment_expression {}
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

