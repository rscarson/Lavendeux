%{
	#include "parse.h"
	#include "list.h"

	#include "tab.h"

	#include <stdlib.h>
	#include <math.h>
	#include <wchar.h>
	#include <stdio.h>

	int yyerror(char *s);
	int linenumber = 1;
%}

%option noyywrap reentrant bison-bridge

%%

[a-zA-Z_][a-zA-Z0-9_]* {
	/* Store token value */
	yylval->val.type = VALUE_STRING;
	mbstowcs(yylval->val.sv, yytext, yyget_leng(yyscanner));

	return IDENTIFIER; 
}

0x[0-9a-fA-F]+ { 
	/* Store token value */
	yylval->val.type = VALUE_INT;
	yylval->val.iv = strtoll(&(yytext[2]), NULL, 16);

	return HEX;
}

0b[0-1]+ { 
	/* Store token value */
	yylval->val.type = VALUE_INT;
	yylval->val.iv = strtoll(&(yytext[2]), NULL, 2);

	return BIN;
}

0o[0-7]+ { 
	/* Store token value */
	yylval->val.type = VALUE_INT;
	yylval->val.iv = strtoll(&(yytext[2]), NULL, 8);

	return OCT;
}

-?[0-9]*(\.[0-9]+)?(E|e)(\+|-)?[0-9]+ {
	float_value_t left;
	int_value_t right;
	scanf("%lfE%d", (double*) &left, (int*) &right);

	/* Store token value */
	yylval->val.type = VALUE_FLOAT;
	yylval->val.fv = frexpl(left, (int*) &right);

	return SCI;
}

-?[0-9]*\.[0-9]+ { 
	/* Store token value */
	yylval->val.type = VALUE_FLOAT;
	yylval->val.fv = strtold(yytext, NULL);

	return FLOAT;
}
-?[0-9]+ { 
	/* Store token value */
	yylval->val.type = VALUE_INT;
	yylval->val.iv = strtoll(yytext, NULL, 10);

	return INT;
}

"+" { 
	return PLUS;
}

"-" { 
	return MINUS;
}

"!" { 
	return FACTORIAL;
}

"*" { 
	return MUL;
}

"/" { 
	return DIV;
}

"%" { 
	return MOD;
}

"**" { 
	return POW;
}

"~" { 
	return NOT;
}

"&" { 
	return AND;
}

"|" { 
	return OR;
}

"^" { 
	return XOR;
}

"<" { 
	return LSHIFT; 
}

">" { 
	return RSHIFT; 
}

"," { 
	return COMMA; 
}

"@" { 
	return DECORATOR; 
}

"=" { 
	/* Store remaining text */
	yylval->val.type = VALUE_STRING;
	mbstowcs(yylval->val.sv, &yytext[1], strlen(yytext)-yyget_leng(yyscanner));

	return EQUAL; 
}

"(" { 
	return LPAREN; 
}

")" { 
	return RPAREN; 
}

[ \t] ;
\n { ++linenumber; }
\r ;
[#[^\n]*] ;

%%