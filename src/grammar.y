%{
	#include <stdio.h>
	#include <stdlib.h>

	#include "decorators.h"
	#include "parse.h"

	#define YYSTYPE value

	int yylex(void);
	int yyerror(char*);

	typedef struct {
		int size;
		value elements[];
	} list;
%}

%glr-parser
%define api.pure
%lex-param {yyscan_t scanner}
%parse-param {yyscan_t scanner}
%parse-param {int parsing_off}
%parse-param {list **value_list[]}
%parse-param {value *result}
%parse-param {char parse_error[EXPRESSION_MAX_LEN]}

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

%%

output:
	expression {
		*result = $1;
	}
	;

expression:
	assignment_expression {
		$$.sv = malloc(sizeof(wchar_t)*(EXPRESSION_MAX_LEN+1));
		if ($$.sv == NULL) {
			strcpy(parse_error, error_msg(FAILURE_ALLOCATION));
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
		$$.sv = malloc(sizeof(wchar_t)*(EXPRESSION_MAX_LEN+1));
		if ($$.sv == NULL) {
			strcpy(parse_error, error_msg(FAILURE_ALLOCATION));
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
		if (!parsing_off) {
			$$ = verify_expression($1, $3);
			if ($$.type == TYPE_ERROR) {
				strcpy(parse_error, error_msg(result));
				YYABORT;
			}

			switch (type) {
				case TYPE_FLOAT:
					strcpy(parse_error, error_msg(FAILURE_TYPE));
					YYABORT;
				break;

				case TYPE_INT:
					$$.iv = $1.iv | $3.iv;
			}
		}
	}
	| constant_expression XOR constant_expression {
		if (!parsing_off) {
			$$ = verify_expression($1, $3);
			if ($$.type == TYPE_ERROR) {
				strcpy(parse_error, error_msg(result));
				YYABORT;
			}

			switch (type) {
				case TYPE_FLOAT:
					strcpy(parse_error, error_msg(FAILURE_TYPE));
					YYABORT;
				break;

				case TYPE_INT:
					$$.iv = $1.iv ^ $3.iv;
			}
		}
	}
	| constant_expression AND constant_expression {
		if (!parsing_off) {
			$$ = verify_expression($1, $3);
			if ($$.type == TYPE_ERROR) {
				strcpy(parse_error, error_msg(result));
				YYABORT;
			}

			switch (type) {
				case TYPE_FLOAT:
					strcpy(parse_error, error_msg(FAILURE_TYPE));
					YYABORT;
				break;

				case TYPE_INT:
					$$.iv = $1.iv & $3.iv;
			}
		}
	}
	| constant_expression LSHIFT constant_expression {
		if (!parsing_off) {
			$$ = verify_expression($1, $3);
			if ($$.type == TYPE_ERROR) {
				strcpy(parse_error, error_msg(result));
				YYABORT;
			}

			switch (type) {
				case TYPE_FLOAT:
					strcpy(parse_error, error_msg(FAILURE_TYPE));
					YYABORT;
				break;

				case TYPE_INT:
					$$.iv = $1.iv << $3.iv;
			}
		}
	}
	| constant_expression RSHIFT constant_expression {
		if (!parsing_off) {
			$$ = verify_expression($1, $3);
			if ($$.type == TYPE_ERROR) {
				strcpy(parse_error, error_msg(result));
				YYABORT;
			}

			switch (type) {
				case TYPE_FLOAT:
					strcpy(parse_error, error_msg(FAILURE_TYPE));
					YYABORT;
				break;

				case TYPE_INT:
					$$.iv = $1.iv >> $3.iv;
			}
		}
	}
	| constant_expression PLUS constant_expression {
		if (!parsing_off) {
			$$ = verify_expression($1, $3);
			if ($$.type == TYPE_ERROR) {
				strcpy(parse_error, error_msg(result));
				YYABORT;
			}

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
	}
	| constant_expression MINUS constant_expression {
		if (!parsing_off) {
			$$ = verify_expression($1, $3);
			if ($$.type == TYPE_ERROR) {
				strcpy(parse_error, error_msg(result));
				YYABORT;
			}

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
	}
	| constant_expression MUL constant_expression {
		if (!parsing_off) {
			$$ = verify_expression($1, $3);
			if ($$.type == TYPE_ERROR) {
				strcpy(parse_error, error_msg(result));
				YYABORT;
			}

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
	}
	| constant_expression DIV constant_expression {
		if (!parsing_off) {
			$$ = verify_expression($1, $3);
			if ($$.type == TYPE_ERROR) {
				strcpy(parse_error, error_msg(result));
				YYABORT;
			}

			switch (type) {
				case TYPE_FLOAT:
					float_value_t left, right;
					float_value($1, &left);
					float_value($3, &right);

					if (right == 0.0) {
						strcpy(parse_error, error_msg(FAILURE_INVALID_ARGS));
						YYABORT;
					}
					$$.fv = left / right;
				break;

				case TYPE_INT:
					int_value_t left, right;
					int_value($1, &left);
					int_value($3, &right);

					if (right == 0) {
						strcpy(parse_error, error_msg(FAILURE_INVALID_ARGS));
						YYABORT;
					}
					$$.iv = left / right;
			}
		}
	}
	| constant_expression MOD constant_expression {
		if (!parsing_off) {
			$$ = verify_expression($1, $3);
			if ($$.type == TYPE_ERROR) {
				strcpy(parse_error, error_msg(result));
				YYABORT;
			}

			switch (type) {
				case TYPE_FLOAT:
					float_value_t left, right;
					float_value($1, &left);
					float_value($3, &right);

					if (right == 0.0) {
						strcpy(parse_error, error_msg(FAILURE_INVALID_ARGS));
						YYABORT;
					}
					$$.fv = left % right;
				break;

				case TYPE_INT:
					int_value_t left, right;
					int_value($1, &left);
					int_value($3, &right);

					if (right == 0) {
						strcpy(parse_error, error_msg(FAILURE_INVALID_ARGS));
						YYABORT;
					}
					$$.iv = left % right;
			}
		}
	}
	| constant_expression POW constant_expression {
		if (!parsing_off) {
			$$ = verify_expression($1, $3);
			if ($$.type == TYPE_ERROR) {
				strcpy(parse_error, error_msg(result));
				YYABORT;
			}

			switch (type) {
				case TYPE_FLOAT:
					float_value_t left, right;
					float_value($1, &left);
					float_value($3, &right);

					if (right == 0.0) {
						strcpy(parse_error, error_msg(FAILURE_INVALID_ARGS));
						YYABORT;
					}
					$$.fv = powl(left, right);
				break;

				case TYPE_INT:
					int_value_t left, right;
					int_value($1, &left);
					int_value($3, &right);

					if (right == 0) {
						strcpy(parse_error, error_msg(FAILURE_INVALID_ARGS));
						YYABORT;
					}
					$$.iv = (int_type_t) powl(left, right);
			}
		}
	}
	| constant_expression FACTORIAL {
		if (!parsing_off) {
			$$ = verify_expression($1, NULL);
			if ($$.type == TYPE_ERROR) {
				strcpy(parse_error, error_msg(result));
				YYABORT;
			}

			switch (type) {
				case TYPE_FLOAT:
					strcpy(parse_error, error_msg(FAILURE_TYPE));
					YYABORT;
				break;

				case TYPE_INT:
					int_value_t left, right;
					int_value($1, &left);

					if (left < 0) {
						strcpy(parse_error, error_msg(FAILURE_INVALID_ARGS));
						YYABORT;
					}
					$$.iv = ifactorial(left);
			}
		}
	}
	| NOT constant_expression {
		if (!parsing_off) {
			$$ = verify_expression(NULL, $2);
			if ($$.type == TYPE_ERROR) {
				strcpy(parse_error, error_msg(result));
				YYABORT;
			}

			switch (type) {
				case TYPE_FLOAT:
					strcpy(parse_error, error_msg(FAILURE_TYPE));
					YYABORT;
				break;

				case TYPE_INT:
					$$.iv = !$2.iv;
			}
		}
	}
	| IDENTIFIER LPAREN RPAREN {
		if (!parsing_off) {
			solve_function($1.sv, NULL, 0, &$$);
		}
	}
	| IDENTIFIER LPAREN constant_expression RPAREN {
		if (!parsing_off) {
			value args = { $3 };
			solve_function($1.sv, args, 1, &$$);
		}
	}
	| IDENTIFIER LPAREN expression_list RPAREN {
		if (!parsing_off) {
			solve_function($1.sv, (*value_list)->elements, (*value_list)->size, &$$);
			free((*value_list));
		}
	}
	;

expression_list:
	constant_expression COMMA constant_expression {
		if (!parsing_off) {
			(*value_list)->size = 2;
			(*value_list)->elements = (value[]) malloc(sizeof(value)*(*value_list)->size);
			if ((*value_list)->elements == NULL) {
				strcpy(parse_error, error_msg(FAILURE_ALLOCATION));
				YYABORT;
			}

			(*value_list)->elements[0] = $1;
			(*value_list)->elements[1] = $3;
		}
	}
	| expression_list COMMA constant_expression {
		if (!parsing_off) {
			(*value_list)->size++;
			(*value_list)->elements = (value[]) realloc(sizeof(value)*((*value_list)->size));
			if ((*value_list)->elements == NULL) {
				strcpy(parse_error, error_msg(FAILURE_ALLOCATION));
				YYABORT;
			}

			(*value_list)->elements[(*value_list)->size-1] = $3;
		}
	}
	;

leftside_funct_expression:
	| IDENTIFIER LPAREN RPAREN EQUAL {
		function *fn;
		parsing_off = 1;

		fn = (function*) malloc(sizeof(function));
		if (fn == NULL) {
			strcpy(parse_error, error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}

		fn.expression = (wchar_t*) malloc(sizeof(wchar_t)*(wcslen($4.sv)+1));
		if (fn.expression == NULL) {
			strcpy(parse_error, error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}

		fn.arguments = NULL;
		fn.n_args = 0;

		put_function($1.sv, fn);
	}
	| IDENTIFIER LPAREN IDENTIFIER RPAREN EQUAL {
		function *fn;
		parsing_off = 1;

		fn = (function*) malloc(sizeof(function));
		if (fn == NULL) {
			strcpy(parse_error, error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}

		fn.expression = (wchar_t*) malloc(sizeof(wchar_t)*(wcslen($4.sv)+1));
		if (fn.expression == NULL) {
			strcpy(parse_error, error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}

		fn.arguments = malloc(sizeof(wchar_t*));
		if (fn.arguments == NULL) {
			strcpy(parse_error, error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}

		fn.arguments[0] = (wchar_t*) malloc(sizeof(wchar_t)*(wcslen($1.sv)+1));
		if (fn.arguments[0] == NULL) { 
			strcpy(parse_error, error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}
		wcscpy(fn.arguments[0], $3.sv);
		fn.n_args = 1;

		put_function($1.sv, fn);
	}
	| IDENTIFIER LPAREN identifier_list RPAREN EQUAL {
		function *fn;
		int i;
		parsing_off = 1;

		fn = (function*) malloc(sizeof(function));
		if (fn == NULL) {
			strcpy(parse_error, error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}

		fn.expression = (wchar_t*) malloc(sizeof(wchar_t)*(wcslen($4.sv)+1));
		if (fn.expression == NULL) {
			strcpy(parse_error, error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}

		fn.arguments = malloc(sizeof(wchar_t*)*(*value_list)->size);
		if (fn.arguments == NULL) {
			strcpy(parse_error, error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}

		fn.n_args = (*value_list)->size;
		for (i=0; i<(*value_list)->size; ++i)
			fn->arguments[i] = (*value_list)->elements[i].sv;
		free((*value_list)->elements);

		put_function($1.sv, fn);
	}
	;


assignment_expression:
	assignment_expression {
		$$ = $1;
	}
	| IDENTIFIER EQUAL constant_expression {
		value *v = (value*) malloc(sizeof(value));
		if (v == NULL) {
			strcpy(parse_error, error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}

		*v = $3;
		put_variable($1.sv, v);

		$$ = $3;
	}
	| leftside_funct_expression constant_expression {
		parsing_off = 0;
		$$ = $2;
	}
	;

identifier_list:
	IDENTIFIER COMMA IDENTIFIER {
		(*value_list)->size = 2;
		(*value_list)->elements = malloc(sizeof(wchar_t*)*(*value_list)->size);
		if ((*value_list)->elements == NULL) {
			strcpy(parse_error, error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}

		(*value_list)->elements[0] = (wchar_t*) malloc(sizeof(wchar_t)*(wcslen($1.sv)+1));
		if ((*value_list)->elements[0] == NULL) {
			strcpy(parse_error, error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}
		wcscpy((*value_list)->elements[0], $1.sv);

		(*value_list)->elements[1] = (wchar_t*) malloc(sizeof(wchar_t)*(wcslen($3.sv)+1));
		if ((*value_list)->elements[1] == NULL) {
			strcpy(parse_error, error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}
		wcscpy((*value_list)->elements[1], $3.sv);
	}
	| identifier_list COMMA IDENTIFIER {
		(*value_list)->size++;
		(*value_list)->elements = realloc(sizeof(wchar_t*)*(*value_list)->size);
		if ((*value_list)->elements == NULL) {
			strcpy(parse_error, error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}

		(*value_list)->elements[(*value_list)->size-1] = (wchar_t*) malloc(sizeof(wchar_t)*(wcslen($3.sv)+1));
		if ((*value_list)->elements[(*value_list)->size-1] == NULL) {
			strcpy(parse_error, error_msg(FAILURE_ALLOCATION));
			YYABORT;
		}
		wcscpy((*value_list)->elements[(*value_list)->size-1], $3.sv);
	}
	;

%%

int yyerror(char *s) {
	return 1;
}