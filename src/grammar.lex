%{
	#include "parse.h"

	#ifndef YYSTYPE
		typedef value YYSTYPE;
		#define YYSTYPE value
	#endif

	#include "tab.h"

	#include <stdlib.h>
	#include <math.h>
	#include <wchar.h>

	int yyerror(char *s);
	int linenumber = 1;
%}

%option noyywrap reentrant bison-bridge

%%

[a-zA-Z_][a-zA-Z0-9_]* {
	/* Store token value */
	yylval->type = VALUE_STRING;
	yylval->sv = (wchar_t*) malloc(sizeof(wchar_t)*(yyget_leng(yyscanner)+1));
	mbstowcs(yylval->sv, yytext, yyget_leng(yyscanner));

	return IDENTIFIER; 
}

0x[0-9a-fA-F]+ { 
	/* Store token value */
	yylval->type = VALUE_INT;
	yylval->iv = strtoll(&(yytext[2]), NULL, 16);

	return HEX;
}

0b[0-1]+ { 
	/* Store token value */
	yylval->type = VALUE_INT;
	yylval->iv = strtoll(&(yytext[2]), NULL, 2);

	return BIN;
}

0o[0-7]+ { 
	/* Store token value */
	yylval->type = VALUE_INT;
	yylval->iv = strtoll(&(yytext[2]), NULL, 8);

	return OCT;
}

-?[0-9]*(\.[0-9]+)?(E|e)(\+|-)?[0-9]+ {
	float_value_t left;
	int_value_t right;
	scanf("%lfE%d", (double*) &left, (int*) &right);

	/* Store token value */
	yylval->type = VALUE_FLOAT;
	yylval->fv = frexpl(left, (int*) &right);

	return SCI;
}

-?[0-9]*\.[0-9]+ { 
	/* Store token value */
	yylval->type = VALUE_FLOAT;
	yylval->fv = strtold(yytext, NULL);

	return FLOAT;
}
-?[0-9]+ { 
	/* Store token value */
	yylval->type = VALUE_INT;
	yylval->iv = strtoll(yytext, NULL, 10);

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
	yylval->type = VALUE_STRING;
	yylval->sv = (wchar_t*) malloc(sizeof(wchar_t)*(strlen(yytext)-yyget_leng(yyscanner)+1));
	mbstowcs(yylval->sv, &yytext[1], strlen(yytext)-yyget_leng(yyscanner));

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