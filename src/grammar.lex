%{
	#include "parse.h"
	#include "list.h"
	#include "language.h"

	#include "tab.h"

	#include <stdlib.h>
	#include <math.h>
	#include <fenv.h>
	#include <wchar.h>
	#include <stdio.h>
	#include <errno.h>
	#include <limits.h>

	int yyerror (yyscan_t, value*, char[], const char*);
	int linenumber = 1;
%}

%option noyywrap
%option reentrant bison-bridge

%%

(\"([^\\\"]|\\.)*\")|(\'([^\\\']|\\.)*\') {
	unsigned int len = yyget_leng(yyscanner) - 2;
	if (len >= EXPRESSION_MAX_LEN)
		len = EXPRESSION_MAX_LEN-1;

	yylval->val.type = VALUE_STRING;
	yylval->val.sv[ mbstowcs(yylval->val.sv, yyget_text(yyscanner)+1, len) ] = L'\0';

	return STRING;
}

[a-zA-Z_][a-zA-Z0-9_]* {
	unsigned int id_len = yyget_leng(yyscanner);
	if (id_len >= EXPRESSION_MAX_LEN-1)
		id_len = EXPRESSION_MAX_LEN-2;

	/* Store token value */
	yylval->val.type = VALUE_IDENT;
	mbstowcs(yylval->val.sv, yyget_text(yyscanner), id_len);
	yylval->val.sv[id_len] = L'\0';

	return IDENTIFIER; 
}

(0x|0X)[0-9a-fA-F]+ { 
	/* Store token value */
	yylval->val.type = VALUE_INT;
	
	errno = 0;
	yylval->val.iv = strtoll(&(yyget_text(yyscanner)[2]), NULL, 16);
	if (errno == ERANGE && yylval->val.iv == LLONG_MAX) {
		yylval->val.iv = LANG_STR_OVERFLOW;
		return ERROR;
	} else if (errno == ERANGE && yylval->val.iv == LLONG_MAX) {
		yylval->val.iv = LANG_STR_UNDERFLOW;
		return ERROR;
	}

	return HEX;
}

#[0-9a-fA-F]+ { 
	/* Store token value */
	yylval->val.type = VALUE_INT;
	
	errno = 0;
	yylval->val.iv = strtoll(&(yyget_text(yyscanner)[1]), NULL, 16);
	if (errno == ERANGE && yylval->val.iv == LLONG_MAX) {
		yylval->val.iv = LANG_STR_OVERFLOW;
		return ERROR;
	} else if (errno == ERANGE && yylval->val.iv == LLONG_MAX) {
		yylval->val.iv = LANG_STR_UNDERFLOW;
		return ERROR;
	}

	return HEX;
}

(0b|0B)[0-1]+ { 
	/* Store token value */
	yylval->val.type = VALUE_INT;
	
	errno = 0;
	yylval->val.iv = strtoll(&(yyget_text(yyscanner)[2]), NULL, 2);
	if (errno == ERANGE && yylval->val.iv == LLONG_MAX) {
		yylval->val.iv = LANG_STR_OVERFLOW;
		return ERROR;
	} else if (errno == ERANGE && yylval->val.iv == LLONG_MAX) {
		yylval->val.iv = LANG_STR_UNDERFLOW;
		return ERROR;
	}

	return BIN;
}

(0o|0O|0)[0-7]+ { 
	/* Store token value */
	yylval->val.type = VALUE_INT;
	
	errno = 0;
	yylval->val.iv = strtoll(&(yyget_text(yyscanner)[2]), NULL, 8);
	if (errno == ERANGE && yylval->val.iv == LLONG_MAX) {
		yylval->val.iv = LANG_STR_OVERFLOW;
		return ERROR;
	} else if (errno == ERANGE && yylval->val.iv == LLONG_MAX) {
		yylval->val.iv = LANG_STR_UNDERFLOW;
		return ERROR;
	}

	return OCT;
}

[0-9]*(\.[0-9]+)?(E|e)(\+|-)?[0-9]+ {
	/* Store token value */
	yylval->val.type = VALUE_FLOAT;

	feclearexcept (FE_ALL_EXCEPT);
	yylval->val.fv = strtold(yyget_text(yyscanner), NULL);
	if (fetestexcept (FE_OVERFLOW)) {
		yylval->val.iv = LANG_STR_OVERFLOW;
		return ERROR;
	} else if (fetestexcept (FE_UNDERFLOW)) {
		yylval->val.iv = LANG_STR_UNDERFLOW;
		return ERROR;
	}

	return SCI;
}

[0-9]*\.[0-9]+ { 
	/* Store token value */
	yylval->val.type = VALUE_FLOAT;

	feclearexcept (FE_ALL_EXCEPT);
	yylval->val.fv = strtold(yyget_text(yyscanner), NULL);
	if (fetestexcept (FE_OVERFLOW)) {
		yylval->val.iv = LANG_STR_OVERFLOW;
		return ERROR;
	} else if (fetestexcept (FE_UNDERFLOW)) {
		yylval->val.iv = LANG_STR_UNDERFLOW;
		return ERROR;
	}


	return FLOAT;
}
[0-9]+ { 
	/* Store token value */
	yylval->val.type = VALUE_INT;

	errno = 0;
	yylval->val.iv = strtoll(yyget_text(yyscanner), NULL, 10);
	if (errno == ERANGE && yylval->val.iv == LLONG_MAX) {
		yylval->val.iv = LANG_STR_OVERFLOW;
		return ERROR;
	} else if (errno == ERANGE && yylval->val.iv == LLONG_MIN) {
		yylval->val.iv = LANG_STR_UNDERFLOW;
		return ERROR;
	}

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

%%