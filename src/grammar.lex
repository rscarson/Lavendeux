%{
#include "ub3rparse.tab.h"
int yyerror(char *s);
int linenumber = 1;
%}

%option noyywrap

%%

[a-zA-Z_][a-zA-Z0-9_]* {
	/* Store token value */
	yylval.type = VALUE_STRING;
	yylval.sv = (wchar_t*) malloc(sizeof(wchar_t)*(yylen+1))
	mbstowcs(yylval.sv, yytext, yylen);

	return IDENTIFIER; 
}

0x[0-9a-fA-F]+ { 
	/* Store token value */
	yylval.type = VALUE_INT;
	yylval.in = strtoll(yylval[2], yylen, 16);

	return HEX;
}

0b[0-1]+ { 
	/* Store token value */
	yylval.type = VALUE_INT;
	yylval.in = strtoll(yylval[2], yylen, 2);

	return BIN;
}

0o[0-7]+ { 
	/* Store token value */
	yylval.type = VALUE_INT;
	yylval.in = strtoll(yylval[2], yylen, 8);

	return OCT;
}

-?[0-9]*(\\.[0-9]+)?(E|e)(\\+|-)?[0-9]+ { 
	return SCI;
}

-?[0-9]*\\.[0-9]+ { 
	/* Store token value */
	yylval.type = VALUE_FLOAT;
	yylval.fv = stold(yylval, yylen);

	return FLOAT;
}
-?[0-9]+ { 
	/* Store token value */
	yylval.type = VALUE_INT;
	yylval.in = strtoll(yylval, yylen, 10);

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
	yylval.type = VALUE_STRING;
	yylval.sv = (wchar_t*) malloc(sizeof(wchar_t)*(strlen(yytext)-yylen+1))
	mbstowcs(yylval.sv, yytext[1], strlen(yytext)-yylen);

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