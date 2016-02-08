%{
	#include <stdio.h>
	#include <stdlib.h>

	#include "decorators.h"
	#include "parse.h"

	#define YYSTYPE value

	wchar_t* parse_result;
	char PARSER_ERROR[EXPRESSION_MAX_LEN];

	int yylex(void);
	int yyerror(char*);

	int is_stored_expression = 0;
	wchar_t *expression_name;
	wchar_t *expression_text;
	function stored_expression;

	int e_list_size;
	value e_list[] = NULL;

	int i_list_size;
	wchar_t* i_list[] = NULL;
%}

/* Valid tokens */
%token IDENTIFIER HEX BIN OCT SCI FLOAT INT
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

output:
	expression {
		parse_result = $1.sv;
	}
	;

expression:
	assignment_expression {
		$$.type = TYPE_STRING;
		$$.sv = malloc(sizeof(wchar_t)*(EXPRESSION_MAX_LEN+1));
		if ($$.sv == NULL) {
			yyerror(error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}

		switch ($1.type) {
			case TYPE_FLOAT:
				sprintf($$.sv, "%Lf", $1.fv);
				break;
			case TYPE_INT:
				sprintf($$.sv, "%Ld", $1.iv);
				break;
		}
	}
	| assignment_expression DECORATOR IDENTIFIER {
		$$.type = TYPE_STRING;
		$$.sv = malloc(sizeof(wchar_t)*(EXPRESSION_MAX_LEN+1));
		if ($$.sv == NULL) {
			yyerror(error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}

		wcscpy(decorate($1, $3.sv), $$.sv);
	}
	;

atomic_value:
	IDENTIFIER {
		$$ = $1;
	}
	| HEX {
		$$ = $1;
	}
	| BIN {
		$$ = $1;
	}
	| OCT {
		$$ = $1;
	}
	| SCI {
		$$ = $1;
	}
	| FLOAT {
		$$ = $1;
	}
	| INT {
		$$ = $1;
	}
	;

constant_expression:
	atomic_value {
		$$ = $1;
	}
	| LPAREN constant_expression RPAREN {
		$$ = $2;
	}
	| constant_expression OR constant_expression {
		int result;
		char type = expression_type($1, $3, &result);
		if (type == TYPE_ERROR) {
			yyerror(error_msg(result));
			YYABORT;
		}

		$$.type = type;
		switch (type) {
			case TYPE_FLOAT:
				yyerror(error_msg(FAILURE_TYPE));
				YYABORT;
			break;

			case TYPE_INT:
				$$.iv = $1.iv | $3.iv;
		}
	}
	| constant_expression XOR constant_expression {
		int result;
		char type = expression_type($1, $3, &result);
		if (type == TYPE_ERROR) {
			yyerror(error_msg(result));
			YYABORT;
		}

		$$.type = type;
		switch (type) {
			case TYPE_FLOAT:
				yyerror(error_msg(FAILURE_TYPE));
				YYABORT;
			break;

			case TYPE_INT:
				$$.iv = $1.iv ^ $3.iv;
		}
	}
	| constant_expression AND constant_expression {
		int result;
		char type = expression_type($1, $3, &result);
		if (type == TYPE_ERROR) {
			yyerror(error_msg(result));
			YYABORT;
		}

		$$.type = type;
		switch (type) {
			case TYPE_FLOAT:
				yyerror(error_msg(FAILURE_TYPE));
				YYABORT;
			break;

			case TYPE_INT:
				$$.iv = $1.iv & $3.iv;
		}
	}
	| constant_expression LSHIFT constant_expression {
		int result;
		char type = expression_type($1, $3, &result);
		if (type == TYPE_ERROR) {
			yyerror(error_msg(result));
			YYABORT;
		}

		$$.type = type;
		switch (type) {
			case TYPE_FLOAT:
				yyerror(error_msg(FAILURE_TYPE));
				YYABORT;
			break;

			case TYPE_INT:
				$$.iv = $1.iv << $3.iv;
		}
	}
	| constant_expression RSHIFT constant_expression {
		int result;
		char type = expression_type($1, $3, &result);
		if (type == TYPE_ERROR) {
			yyerror(error_msg(result));
			YYABORT;
		}

		$$.type = type;
		switch (type) {
			case TYPE_FLOAT:
				yyerror(error_msg(FAILURE_TYPE));
				YYABORT;
			break;

			case TYPE_INT:
				$$.iv = $1.iv >> $3.iv;
		}
	}
	| constant_expression PLUS constant_expression {
		int result;
		char type = expression_type($1, $3, &result);
		if (type == TYPE_ERROR) {
			yyerror(error_msg(result));
			YYABORT;
		}

		$$.type = type;
		switch (type) {
			case TYPE_FLOAT:
				float_value_t left, right;
				float_value($1, &left);
				float_value($3, &right);

				$$.fv = left + right;
			break;

			case TYPE_INT:
				int_value_t left, right;
				int_value($1, &left);
				int_value($3, &right);

				$$.iv = left + right;
		}
	}
	| constant_expression MINUS constant_expression {
		int result;
		char type = expression_type($1, $3, &result);
		if (type == TYPE_ERROR) {
			yyerror(error_msg(result));
			YYABORT;
		}

		$$.type = type;
		switch (type) {
			case TYPE_FLOAT:
				float_value_t left, right;
				float_value($1, &left);
				float_value($3, &right);

				$$.fv = left - right;
			break;

			case TYPE_INT:
				int_value_t left, right;
				int_value($1, &left);
				int_value($3, &right);

				$$.iv = left - right;
		}
	}
	| constant_expression MUL constant_expression {
		int result;
		char type = expression_type($1, $3, &result);
		if (type == TYPE_ERROR) {
			yyerror(error_msg(result));
			YYABORT;
		}

		$$.type = type;
		switch (type) {
			case TYPE_FLOAT:
				float_value_t left, right;
				float_value($1, &left);
				float_value($3, &right);

				$$.fv = left * right;
			break;

			case TYPE_INT:
				int_value_t left, right;
				int_value($1, &left);
				int_value($3, &right);

				$$.iv = left * right;
		}
	}
	| constant_expression DIV constant_expression {
		int result;
		char type = expression_type($1, $3, &result);
		if (type == TYPE_ERROR) {
			yyerror(error_msg(result));
			YYABORT;
		}

		$$.type = type;
		switch (type) {
			case TYPE_FLOAT:
				float_value_t left, right;
				float_value($1, &left);
				float_value($3, &right);

				if (right == 0.0) {
					yyerror(error_msg(FAILURE_INVALID_ARGS));
					YYABORT;
				}
				$$.fv = left / right;
			break;

			case TYPE_INT:
				int_value_t left, right;
				int_value($1, &left);
				int_value($3, &right);

				if (right == 0) {
					yyerror(error_msg(FAILURE_INVALID_ARGS));
					YYABORT;
				}
				$$.iv = left / right;
		}
	}
	| constant_expression MOD constant_expression {
		int result;
		char type = expression_type($1, $3, &result);
		if (type == TYPE_ERROR) {
			yyerror(error_msg(result));
			YYABORT;
		}

		$$.type = type;
		switch (type) {
			case TYPE_FLOAT:
				float_value_t left, right;
				float_value($1, &left);
				float_value($3, &right);

				if (right == 0.0) {
					yyerror(error_msg(FAILURE_INVALID_ARGS));
					YYABORT;
				}
				$$.fv = left % right;
			break;

			case TYPE_INT:
				int_value_t left, right;
				int_value($1, &left);
				int_value($3, &right);

				if (right == 0) {
					yyerror(error_msg(FAILURE_INVALID_ARGS));
					YYABORT;
				}
				$$.iv = left % right;
		}
	}
	| constant_expression POW constant_expression {
		int result;
		char type = expression_type($1, $3, &result);
		if (type == TYPE_ERROR) {
			yyerror(error_msg(result));
			YYABORT;
		}

		$$.type = type;
		switch (type) {
			case TYPE_FLOAT:
				float_value_t left, right;
				float_value($1, &left);
				float_value($3, &right);

				if (right == 0.0) {
					yyerror(error_msg(FAILURE_INVALID_ARGS));
					YYABORT;
				}
				$$.fv = powl(left, right);
			break;

			case TYPE_INT:
				int_value_t left, right;
				int_value($1, &left);
				int_value($3, &right);

				if (right == 0) {
					yyerror(error_msg(FAILURE_INVALID_ARGS));
					YYABORT;
				}
				$$.iv = (int_type_t) powl(left, right);
		}
	}
	| constant_expression FACTORIAL {
		int result;
		char type = expression_type($1, NULL, &result);
		if (type == TYPE_ERROR) {
			yyerror(error_msg(result));
			YYABORT;
		}

		$$.type = type;
		switch (type) {
			case TYPE_FLOAT:
				yyerror(error_msg(FAILURE_TYPE));
				YYABORT;
			break;

			case TYPE_INT:
				int_value_t left, right;
				int_value($1, &left);

				if (left < 0) {
					yyerror(error_msg(FAILURE_INVALID_ARGS));
					YYABORT;
				}
				$$.iv = ifactorial(left);
		}
	}
	| NOT constant_expression {
		int result;
		char type = expression_type(NULL, $2, &result);
		if (type == TYPE_ERROR) {
			yyerror(error_msg(result));
			YYABORT;
		}

		$$.type = type;
		switch (type) {
			case TYPE_FLOAT:
				yyerror(error_msg(FAILURE_TYPE));
				YYABORT;
			break;

			case TYPE_INT:
				$$.iv = !$2.iv;
		}
	}
	| IDENTIFIER LPAREN RPAREN {
		solve_function($1.sv, NULL, 0, &$$);
	}
	| IDENTIFIER LPAREN constant_expression RPAREN {
		value args = value[1];
		value[0] = $3;
		solve_function($1.sv, args, 1, &$$);
	}
	| IDENTIFIER LPAREN expression_list RPAREN {
		solve_function($1.sv, e_list, e_list_size, &$$);
		free(e_list);
	}
	;

expression_list:
	constant_expression COMMA constant_expression {
		e_list = (value[]) malloc(sizeof(value)*2);
		if (e_list == NULL) {
			yyerror(error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}

		e_list_size = 2;
		e_list[0] = $1;
		e_list[1] = $3;
	}
	| expression_list COMMA constant_expression {
		e_list_size++;
		e_list = (value[]) realloc(sizeof(value)*(e_list_size));
		if (e_list == NULL) {
			yyerror(error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}

		e_list[e_list_size-1] = $3;
	}
	;

leftside_funct_expression:
	| IDENTIFIER LPAREN RPAREN EQUAL {
		is_stored_expression = 1;
		expression_name = $1.sv;

		stored_expression.expression = (wchar_t*) malloc(sizeof(wchar_t)*(wcslen($4.sv)+1));
		if (stored_expression.expression == NULL) {
			yyerror(error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}

		stored_expression.arguments = NULL;
		stored_expression.n_args = 0;
	}
	| IDENTIFIER LPAREN IDENTIFIER RPAREN EQUAL {
		is_stored_expression = 1;
		expression_name = $1.sv;

		stored_expression.expression = (wchar_t*) malloc(sizeof(wchar_t)*(wcslen($4.sv)+1));
		if (stored_expression.expression == NULL) {
			yyerror(error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}

		stored_expression.arguments = malloc(sizeof(wchar_t*));
		if (stored_expression.arguments == NULL) {
			yyerror(error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}

		stored_expression.arguments[0] = (wchar_t*) malloc(sizeof(wchar_t)*(wcslen($1.sv)+1));
		if (stored_expression.arguments[0] == NULL) { 
			yyerror(error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}
		wcscpy(stored_expression.arguments[0], $3.sv);
		stored_expression.n_args = 1;
	}
	| IDENTIFIER LPAREN identifier_list RPAREN EQUAL {
		is_stored_expression = 1;
		expression_name = $1.sv;

		stored_expression.expression = (wchar_t*) malloc(sizeof(wchar_t)*(wcslen($4.sv)+1));
		if (stored_expression.expression == NULL)  {
			yyerror(error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}

		stored_expression.arguments = malloc(sizeof(wchar_t*));
		if (stored_expression.arguments == NULL) {
			yyerror(error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}

		stored_expression.n_args = i_list_size;
		memcpy(stored_expression.arguments, i_list, sizeof(wchar_t*)*i_list_size);
		free(i_list);
	}
	;


assignment_expression:
	assignment_expression {
		$$ = $1;
	}
	| IDENTIFIER EQUAL constant_expression {
		value *v = (value*) malloc(sizeof(value));
		if (v == NULL) {
			yyerror(error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}

		*v = $3;
		put_variable($1.sv, v);

		$$ = $3;
	}
	| leftside_funct_expression constant_expression {
		is_stored_expression = 0;
		$$ = $2;
	}
	;

identifier_list:
	IDENTIFIER COMMA IDENTIFIER {
		i_list_size = 2;
		i_list = malloc(sizeof(wchar_t*)*i_list_size);
		if (i_list == NULL) {
			yyerror(error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}

		i_list[0] = (wchar_t*) malloc(sizeof(wchar_t)*(wcslen($1.sv)+1));
		if (i_list[0] == NULL) {
			yyerror(error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}
		wcscpy(i_list[0], $1.sv);

		i_list[1] = (wchar_t*) malloc(sizeof(wchar_t)*(wcslen($3.sv)+1));
		if (i_list[1] == NULL) {
			yyerror(error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}
		wcscpy(i_list[1], $3.sv);
	}
	| identifier_list COMMA IDENTIFIER {
		i_list_size++;
		i_list = realloc(sizeof(wchar_t*)*i_list_size);
		if (i_list == NULL) {
			yyerror(error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}

		i_list[i_list_size-1] = (wchar_t*) malloc(sizeof(wchar_t)*(wcslen($3.sv)+1));
		if (i_list[i_list_size-1] == NULL) {
			yyerror(error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}
		wcscpy(i_list[i_list_size-1], $3.sv);
	}
	;

%%

#include <stdio.h>
#include <stdlib.h>

extern int yylex();
extern int yyparse();


int yyerror(char *s) {
	extern int yylineno;
	extern char *yytext;

	sprintf(PARSER_ERROR, "%s at symbol '%c', line %d", s, *yytext, yylineno);
	return 1;
}

char* get_error( void ) {
	return PARSER_ERROR;
}