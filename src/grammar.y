%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <math.h>

	#include "parse.h"

	#define YYERROR_CODE(c) result->iv=c; yyerror(scanner, parsing_off, value_list, result, parse_error, code_to_msg(c));

	#ifndef YYSTYPE
		typedef value YYSTYPE;
		#define YYSTYPE value
	#endif

	#include "lex.h"
	#include "decorators.h"

	typedef struct {
		int size;
		value elements[];
	} list;

	int yyerror (yyscan_t, int, list **value_list, value*, char[], const char*);
%}

%glr-parser
%define api.pure
%lex-param {yyscan_t scanner}
%parse-param {yyscan_t scanner}
%parse-param {int parsing_off}
%parse-param {list **value_list}
%parse-param {value *result}
%parse-param {char parse_error[]}

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
			YYERROR_CODE(FAILURE_ALLOCATION);
			YYABORT;
		}

		switch ($1.type) {
			case VALUE_FLOAT:
				swprintf($$.sv, EXPRESSION_MAX_LEN, L"%llf", $1.fv);
				break;
			case VALUE_INT:
				swprintf($$.sv, EXPRESSION_MAX_LEN, L"%lld", $1.iv);
				break;
		}
	}
	| assignment_expression DECORATOR IDENTIFIER {
		$$.sv = malloc(sizeof(wchar_t)*(EXPRESSION_MAX_LEN+1));
		if ($$.sv == NULL) {
			YYERROR_CODE(FAILURE_ALLOCATION);
			YYABORT;
		}

		decorate($1.sv, &$3, $$.sv);
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
			$$ = verify_expression(&$1, &$3);
			if ($$.type == VALUE_ERROR) {
				YYERROR_CODE($$.iv);
				YYABORT;
			}

			switch ($$.type) {
				case VALUE_FLOAT:
					YYERROR_CODE(FAILURE_TYPE);
					YYABORT;
				break;

				case VALUE_INT:
					$$.iv = $1.iv | $3.iv;
			}
		}
	}
	| constant_expression XOR constant_expression {
		if (!parsing_off) {
			$$ = verify_expression(&$1, &$3);
			if ($$.type == VALUE_ERROR) {
				YYERROR_CODE($$.iv);
				YYABORT;
			}

			switch ($$.type) {
				case VALUE_FLOAT:
					YYERROR_CODE(FAILURE_TYPE);
					YYABORT;
				break;

				case VALUE_INT:
					$$.iv = $1.iv ^ $3.iv;
			}
		}
	}
	| constant_expression AND constant_expression {
		if (!parsing_off) {
			$$ = verify_expression(&$1, &$3);
			if ($$.type == VALUE_ERROR) {
				YYERROR_CODE($$.iv);
				YYABORT;
			}

			switch ($$.type) {
				case VALUE_FLOAT:
					YYERROR_CODE(FAILURE_TYPE);
					YYABORT;
				break;

				case VALUE_INT:
					$$.iv = $1.iv & $3.iv;
			}
		}
	}
	| constant_expression LSHIFT constant_expression {
		if (!parsing_off) {
			$$ = verify_expression(&$1, &$3);
			if ($$.type == VALUE_ERROR) {
				YYERROR_CODE($$.iv);
				YYABORT;
			}

			switch ($$.type) {
				case VALUE_FLOAT:
					YYERROR_CODE(FAILURE_TYPE);
					YYABORT;
				break;

				case VALUE_INT:
					$$.iv = $1.iv << $3.iv;
			}
		}
	}
	| constant_expression RSHIFT constant_expression {
		if (!parsing_off) {
			$$ = verify_expression(&$1, &$3);
			if ($$.type == VALUE_ERROR) {
				YYERROR_CODE($$.iv);
				YYABORT;
			}

			switch ($$.type) {
				case VALUE_FLOAT:
					YYERROR_CODE(FAILURE_TYPE);
					YYABORT;
				break;

				case VALUE_INT:
					$$.iv = $1.iv >> $3.iv;
			}
		}
	}
	| constant_expression PLUS constant_expression {
		if (!parsing_off) {
			$$ = verify_expression(&$1, &$3);
			if ($$.type == VALUE_ERROR) {
				YYERROR_CODE($$.iv);
				YYABORT;
			}

			float_value_t fleft_op, fright_op;
			int_value_t ileft_op, iright_op;
			switch ($$.type) {
				case VALUE_FLOAT:
					float_value(&$1, &fleft_op);
					float_value(&$3, &fright_op);

					$$.fv = fleft_op + fright_op;
				break;

				case VALUE_INT:
					int_value(&$1, &ileft_op);
					int_value(&$3, &iright_op);

					$$.iv = ileft_op + iright_op;
			}
		}
	}
	| constant_expression MINUS constant_expression {
		if (!parsing_off) {
			$$ = verify_expression(&$1, &$3);
			if ($$.type == VALUE_ERROR) {
				YYERROR_CODE($$.iv);
				YYABORT;
			}

			float_value_t fleft_op, fright_op;
			int_value_t ileft_op, iright_op;
			switch ($$.type) {
				case VALUE_FLOAT:
					float_value(&$1, &fleft_op);
					float_value(&$3, &fright_op);

					$$.fv = fleft_op - fright_op;
				break;

				case VALUE_INT:
					int_value(&$1, &ileft_op);
					int_value(&$3, &iright_op);

					$$.iv = ileft_op - iright_op;
			}
		}
	}
	| constant_expression MUL constant_expression {
		if (!parsing_off) {
			$$ = verify_expression(&$1, &$3);
			if ($$.type == VALUE_ERROR) {
				YYERROR_CODE($$.iv);
				YYABORT;
			}

			float_value_t fleft_op, fright_op;
			int_value_t ileft_op, iright_op;
			switch ($$.type) {
				case VALUE_FLOAT:
					float_value(&$1, &fleft_op);
					float_value(&$3, &fright_op);

					$$.fv = fleft_op * fright_op;
				break;

				case VALUE_INT:
					int_value(&$1, &ileft_op);
					int_value(&$3, &iright_op);

					$$.iv = ileft_op * iright_op;
			}
		}
	}
	| constant_expression DIV constant_expression {
		if (!parsing_off) {
			$$ = verify_expression(&$1, &$3);
			if ($$.type == VALUE_ERROR) {
				YYERROR_CODE($$.iv);
				YYABORT;
			}

			float_value_t fleft_op, fright_op;
			int_value_t ileft_op, iright_op;
			switch ($$.type) {
				case VALUE_FLOAT:
					float_value(&$1, &fleft_op);
					float_value(&$3, &fright_op);

					if (fright_op == 0.0) {
						YYERROR_CODE(FAILURE_INVALID_ARGS);
						YYABORT;
					}
					$$.fv = fleft_op / fright_op;
				break;

				case VALUE_INT:
					int_value(&$1, &ileft_op);
					int_value(&$3, &iright_op);

					if (iright_op == 0) {
						YYERROR_CODE(FAILURE_INVALID_ARGS);
						YYABORT;
					}
					$$.iv = ileft_op / iright_op;
			}
		}
	}
	| constant_expression MOD constant_expression {
		if (!parsing_off) {
			$$ = verify_expression(&$1, &$3);
			if ($$.type == VALUE_ERROR) {
				YYERROR_CODE($$.iv);
				YYABORT;
			}

			int_value_t left_op, right_op;
			switch ($$.type) {
				case VALUE_FLOAT:
					YYERROR_CODE(FAILURE_TYPE);
					YYABORT;
				break;

				case VALUE_INT:
					int_value(&$1, &left_op);
					int_value(&$3, &right_op);

					if (right_op == 0) {
						YYERROR_CODE(FAILURE_INVALID_ARGS);
						YYABORT;
					}
					$$.iv = left_op % right_op;
			}
		}
	}
	| constant_expression POW constant_expression {
		if (!parsing_off) {
			$$ = verify_expression(&$1, &$3);
			if ($$.type == VALUE_ERROR) {
				YYERROR_CODE($$.iv);
				YYABORT;
			}

			float_value_t fleft_op, fright_op;
			int_value_t ileft_op, iright_op;
			switch ($$.type) {
				case VALUE_FLOAT:
					float_value(&$1, &fleft_op);
					float_value(&$3, &fright_op);

					if (fright_op == 0.0) {
						YYERROR_CODE(FAILURE_INVALID_ARGS);
						YYABORT;
					}
					$$.fv = powl(fleft_op, fright_op);
				break;

				case VALUE_INT:
					int_value(&$1, &ileft_op);
					int_value(&$3, &iright_op);

					if (iright_op == 0) {
						YYERROR_CODE(FAILURE_INVALID_ARGS);
						YYABORT;
					}
					$$.iv = (int_value_t) powl(fleft_op, fright_op);
			}
		}
	}
	| constant_expression FACTORIAL {
		if (!parsing_off) {
			$$ = verify_expression(&$1, NULL);
			if ($$.type == VALUE_ERROR) {
				YYERROR_CODE($$.iv);
				YYABORT;
			}

			int_value_t left_op;
			switch ($$.type) {
				case VALUE_FLOAT:
					YYERROR_CODE(FAILURE_TYPE);
					YYABORT;
				break;

				case VALUE_INT:
					int_value(&$1, &left_op);

					if (left_op < 0) {
						YYERROR_CODE(FAILURE_INVALID_ARGS);
						YYABORT;
					}
					$$.iv = ifactorial(left_op);
			}
		}
	}
	| NOT constant_expression {
		if (!parsing_off) {
			$$ = verify_expression(NULL, &$2);
			if ($$.type == VALUE_ERROR) {
				YYERROR_CODE($$.iv);
				YYABORT;
			}

			switch ($$.type) {
				case VALUE_FLOAT:
					YYERROR_CODE(FAILURE_TYPE);
					YYABORT;
				break;

				case VALUE_INT:
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
			value args[] = { $3 };
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
			*value_list = malloc(sizeof(list) + sizeof(value)*2);
			if ((*value_list) == NULL) {
				YYERROR_CODE(FAILURE_ALLOCATION);
				YYABORT;
			}
			(*value_list)->size = 2;

			(*value_list)->elements[0] = $1;
			(*value_list)->elements[1] = $3;
		}
	}
	| expression_list COMMA constant_expression {
		if (!parsing_off) {
			(*value_list)->size++;
			(*value_list) = realloc(*value_list, sizeof(list) + sizeof(value)*((*value_list)->size));
			if ((*value_list)->elements == NULL) {
				YYERROR_CODE(FAILURE_ALLOCATION);
				YYABORT;
			}

			(*value_list)->elements[(*value_list)->size-1] = $3;
		}
	}
	;

left_opside_funct_expression:
	| IDENTIFIER LPAREN RPAREN EQUAL {
		function *fn;
		parsing_off = 1;

		fn = (function*) malloc(sizeof(function));
		if (fn == NULL) {
			YYERROR_CODE(FAILURE_ALLOCATION);
			YYABORT;
		}

		fn->expression = (wchar_t*) malloc(sizeof(wchar_t)*(wcslen($4.sv)+1));
		if (fn->expression == NULL) {
			YYERROR_CODE(FAILURE_ALLOCATION);
			YYABORT;
		}

		fn->n_args = 0;

		put_function($1.sv, fn);
	}
	| IDENTIFIER LPAREN IDENTIFIER RPAREN EQUAL {
		function *fn;
		parsing_off = 1;

		fn = (function*) malloc(sizeof(function) + sizeof(wchar_t*));
		if (fn == NULL) {
			YYERROR_CODE(FAILURE_ALLOCATION);
			YYABORT;
		}

		fn->expression = (wchar_t*) malloc(sizeof(wchar_t)*(wcslen($4.sv)+1));
		if (fn->expression == NULL) {
			YYERROR_CODE(FAILURE_ALLOCATION);
			YYABORT;
		}

		fn->arguments[0] = malloc(sizeof(wchar_t)*(wcslen($3.sv)+1));
		if (fn->arguments[0] == NULL) {
			YYERROR_CODE(FAILURE_ALLOCATION);
			YYABORT;
		}

		wcscpy(fn->arguments[0], $3.sv);
		fn->n_args = 1;

		put_function($1.sv, fn);
	}
	| IDENTIFIER LPAREN identifier_list RPAREN EQUAL {
		function *fn;
		int i;
		parsing_off = 1;

		fn = (function*) malloc(sizeof(function) + sizeof(wchar_t*)*(*value_list)->size);
		if (fn == NULL) {
			YYERROR_CODE(FAILURE_ALLOCATION);
			YYABORT;
		}

		fn->expression = (wchar_t*) malloc(sizeof(wchar_t)*(wcslen($4.sv)+1));
		if (fn->expression == NULL) {
			YYERROR_CODE(FAILURE_ALLOCATION);
			YYABORT;
		}

		fn->n_args = (*value_list)->size;
		for (i=0; i<(*value_list)->size; ++i)
			fn->arguments[i] = (*value_list)->elements[i].sv;
		free((*value_list)->elements);

		put_function($1.sv, fn);
	}
	;


assignment_expression:
	IDENTIFIER EQUAL constant_expression {
		value *v = (value*) malloc(sizeof(value));
		if (v == NULL) {
			YYERROR_CODE(FAILURE_ALLOCATION);
			YYABORT;
		}

		*v = $3;
		put_variable($1.sv, v);

		$$ = $3;
	}
	| left_opside_funct_expression constant_expression {
		parsing_off = 0;
		$$ = $2;
	}
	;

identifier_list:
	IDENTIFIER COMMA IDENTIFIER {
		(*value_list) = malloc(sizeof(list) + sizeof(value)*2);
		if ((*value_list)->elements == NULL) {
			YYERROR_CODE(FAILURE_ALLOCATION);
			YYABORT;
		}
		(*value_list)->size = 2;

		wcscpy((*value_list)->elements[0].sv, $1.sv);
		wcscpy((*value_list)->elements[1].sv, $3.sv);
	}
	| identifier_list COMMA IDENTIFIER {
		(*value_list)->size++;
		(*value_list) = realloc(*value_list, sizeof(list) + sizeof(value)*(*value_list)->size);
		if ((*value_list) == NULL) {
			YYERROR_CODE(FAILURE_ALLOCATION);
			YYABORT;
		}

		wcscpy((*value_list)->elements[(*value_list)->size-1].sv, $3.sv);
	}
	;

%%

int yyerror(yyscan_t scanner, int parsing_off, list **value_list, value* result, char response[], const char* msg) {
	result->iv = FAILURE_SYNTAX_ERROR;
	sprintf(response, "Error: %s at '%s'", msg, yyget_text(scanner));
	return 1;
}