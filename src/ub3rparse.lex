%{
#include "y.tab.h"
int yyerror(char *s);
int linenumber = 1;
%}

%option noyywrap

%%
[a-zA-Z_][a-zA-Z0-9_]* { return IDENTIFIER; }
0x[0-9a-fA-F]+ { return HEX; }
0b[0-1]+ { return BIN; }
0o[0-7]+ { return OCT; }
-?[0-9]*(\\.[0-9]+)?(E|e)(\\+|-)?[0-9]+ { return SCI; }
-?[0-9]*\\.[0-9]+ { return FLOAT; }
-?[0-9]+ { return INT; }

"+" { return PLUS; }
"-" { return MINUS; }
"!" { return FACTORIAL; }
"*" { return MUL; }
"/" { return DIV; }
"%" { return MOD; }
"**" { return POW;}

"~" { return NOT; }
"&" { return AND; }
"|" { return OR; }
"^" { return XOR; }
"<" { return LSHIFT; }
">" { return RSHIFT; }

"," { return COMMA; }
"@" { return DECORATOR; }
"=" { return EQUAL; }
"(" { return LPAREN; }
")" { return RPAREN; }
[ \t] ;
\n { ++linenumber; return NEWLINE; }
%%