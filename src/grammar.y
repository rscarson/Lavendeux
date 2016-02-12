%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <math.h>

	#include "decorators.h"
	#include "parse.h"
	#include "list.h"

	typedef union YYSTYPE YYSTYPE;
	#include "lex.h"

	#define YYERROR_MSG(c,s) result->iv=c; yyerror(scanner, stored_function, result, parse_error, s); YYABORT;
	#define YYERROR_CODE(c) YYERROR_MSG(c, code_to_msg(c));

	int yyerror (yyscan_t, wchar_t[], value*, char[], const char*);
%}

%glr-parser
%define api.pure
%lex-param {yyscan_t scanner}
%parse-param {yyscan_t scanner}
%parse-param {wchar_t stored_function[]}
%parse-param {value *result}
%parse-param {char parse_error[]}

%union {
	value val;
	list lst;
}

/* Valid tokens */
%token <val> IDENTIFIER HEX BIN OCT SCI FLOAT INT
%token <val> DECORATOR EQUAL LPAREN RPAREN
%token <list> COMMA
%token END 0 "end of expression"
%left <val> OR
%left <val> XOR
%left <val> AND
%left <val> LSHIFT RSHIFT
%left <val> PLUS MINUS
%left <val> MUL DIV MOD
%left <val> POW
%left <val> FACTORIAL
%left <val> NOT

%type<val> expression atomic_value constant_expression assignment_expression left_opside_funct_expression
%type<lst> expression_list identifier_list

%%

expression:
	constant_expression {
		value *v;
		$$ = $1;
		if ($$.type == VALUE_STRING) {
			if (get_variable($1.sv, &v) != NO_FAILURE) {
				YYERROR_CODE(FAILURE_INVALID_NAME);
			}
			$$ = *v;
		}

		switch ($$.type) {
			case VALUE_FLOAT:
				swprintf($$.sv, EXPRESSION_MAX_LEN, L"%llf", $1.fv);
				break;
			case VALUE_INT:
				swprintf($$.sv, EXPRESSION_MAX_LEN, L"%lld", $1.iv);
				break;
		}

		*result = $$;
	}
	| assignment_expression {
		$$ = $1;
		if ($$.type == VALUE_STRING)
			if (get_variable($1.sv, &$$) != NO_FAILURE) {
				YYERROR_CODE(FAILURE_INVALID_NAME);
			}

		switch ($$.type) {
			case VALUE_FLOAT:
				swprintf($$.sv, EXPRESSION_MAX_LEN, L"%llf", $1.fv);
				break;
			case VALUE_INT:
				swprintf($$.sv, EXPRESSION_MAX_LEN, L"%lld", $1.iv);
				break;
		}

		*result = $$;
	}
	| expression DECORATOR IDENTIFIER {
		decorate($1.sv, &$3, $$.sv);

		*result = $$;
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
		if (wcslen(stored_function) == 0) {
			$$ = verify_expression(&$1, &$3);
			if ($$.type == VALUE_ERROR) {
				YYERROR_CODE($$.iv);
			}

			switch ($$.type) {
				case VALUE_FLOAT:
					YYERROR_MSG(FAILURE_TYPE, "Floating point arguments not applicable to binary operators");
				break;

				case VALUE_INT:
					$$.iv = $1.iv | $3.iv;
			}
		}
	}
	| constant_expression XOR constant_expression {
		if (wcslen(stored_function) == 0) {
			$$ = verify_expression(&$1, &$3);
			if ($$.type == VALUE_ERROR) {
				YYERROR_CODE($$.iv);
			}

			switch ($$.type) {
				case VALUE_FLOAT:
					YYERROR_MSG(FAILURE_TYPE, "Floating point arguments not applicable to binary operators");
				break;

				case VALUE_INT:
					$$.iv = $1.iv ^ $3.iv;
			}
		}
	}
	| constant_expression AND constant_expression {
		if (wcslen(stored_function) == 0) {
			$$ = verify_expression(&$1, &$3);
			if ($$.type == VALUE_ERROR) {
				YYERROR_CODE($$.iv);
			}

			switch ($$.type) {
				case VALUE_FLOAT:
					YYERROR_MSG(FAILURE_TYPE, "Floating point arguments not applicable to binary operators");
				break;

				case VALUE_INT:
					$$.iv = $1.iv & $3.iv;
			}
		}
	}
	| constant_expression LSHIFT constant_expression {
		if (wcslen(stored_function) == 0) {
			$$ = verify_expression(&$1, &$3);
			if ($$.type == VALUE_ERROR) {
				YYERROR_CODE($$.iv);
			}

			switch ($$.type) {
				case VALUE_FLOAT:
					YYERROR_MSG(FAILURE_TYPE, "Floating point arguments not applicable to binary operators");
				break;

				case VALUE_INT:
					$$.iv = $1.iv << $3.iv;
			}
		}
	}
	| constant_expression RSHIFT constant_expression {
		if (wcslen(stored_function) == 0) {
			$$ = verify_expression(&$1, &$3);
			if ($$.type == VALUE_ERROR) {
				YYERROR_CODE($$.iv);
			}

			switch ($$.type) {
				case VALUE_FLOAT:
					YYERROR_MSG(FAILURE_TYPE, "Floating point arguments not applicable to binary operators");
				break;

				case VALUE_INT:
					$$.iv = $1.iv >> $3.iv;
			}
		}
	}
	| constant_expression PLUS constant_expression {
		if (wcslen(stored_function) == 0) {
			$$ = verify_expression(&$1, &$3);
			if ($$.type == VALUE_ERROR) {
				YYERROR_CODE($$.iv);
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
		if (wcslen(stored_function) == 0) {
			$$ = verify_expression(&$1, &$3);
			if ($$.type == VALUE_ERROR) {
				YYERROR_CODE($$.iv);
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
		if (wcslen(stored_function) == 0) {
			$$ = verify_expression(&$1, &$3);
			if ($$.type == VALUE_ERROR) {
				YYERROR_CODE($$.iv);
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
		if (wcslen(stored_function) == 0) {
			$$ = verify_expression(&$1, &$3);
			if ($$.type == VALUE_ERROR) {
				YYERROR_CODE($$.iv);
			}

			float_value_t fleft_op, fright_op;
			int_value_t ileft_op, iright_op;
			switch ($$.type) {
				case VALUE_FLOAT:
					float_value(&$1, &fleft_op);
					float_value(&$3, &fright_op);

					if (fright_op == 0.0) {
						YYERROR_MSG(FAILURE_INVALID_ARGS, "Division by 0");
					}
					$$.fv = fleft_op / fright_op;
				break;

				case VALUE_INT:
					int_value(&$1, &ileft_op);
					int_value(&$3, &iright_op);

					if (iright_op == 0) {
						YYERROR_MSG(FAILURE_INVALID_ARGS, "Division by 0");
					}
					$$.iv = ileft_op / iright_op;
			}
		}
	}
	| constant_expression MOD constant_expression {
		if (wcslen(stored_function) == 0) {
			$$ = verify_expression(&$1, &$3);
			if ($$.type == VALUE_ERROR) {
				YYERROR_CODE($$.iv);
			}

			int_value_t left_op, right_op;
			int_value(&$1, &left_op);
			int_value(&$3, &right_op);

			if (right_op == 0) {
				YYERROR_MSG(FAILURE_INVALID_ARGS, "Division by 0");
			}

			$$.iv = left_op % right_op;
		}
	}
	| constant_expression POW constant_expression {
		if (wcslen(stored_function) == 0) {
			$$ = verify_expression(&$1, &$3);
			if ($$.type == VALUE_ERROR) {
				YYERROR_CODE($$.iv);
			}

			float_value_t fleft_op, fright_op;
			int_value_t ileft_op, iright_op;
			switch ($$.type) {
				case VALUE_FLOAT:
					float_value(&$1, &fleft_op);
					float_value(&$3, &fright_op);

					$$.fv = powl(fleft_op, fright_op);
				break;

				case VALUE_INT:
					int_value(&$1, &ileft_op);
					int_value(&$3, &iright_op);

					$$.iv = (int_value_t) powl(ileft_op, iright_op);
			}
		}
	}
	| constant_expression FACTORIAL {
		if (wcslen(stored_function) == 0) {
			$$ = verify_expression(&$1, NULL);
			if ($$.type == VALUE_ERROR) {
				YYERROR_CODE($$.iv);
			}

			int_value_t left_op;
			int_value(&$1, &left_op);
			if (left_op < 0) {
				YYERROR_MSG(FAILURE_INVALID_ARGS, "Factorial is undefined for arguments < 0");
			}

			$$.iv = ifactorial(left_op);
		}
	}
	| NOT constant_expression {
		if (wcslen(stored_function) == 0) {
			$$ = verify_expression(NULL, &$2);
			if ($$.type == VALUE_ERROR) {
				YYERROR_CODE($$.iv);
			}

			switch ($$.type) {
				case VALUE_FLOAT:
					YYERROR_MSG(FAILURE_TYPE, "Floating point arguments not applicable to binary operators");
				break;

				case VALUE_INT:
					$$.iv = !$2.iv;
			}
		}
	}
	| IDENTIFIER LPAREN RPAREN {
		if (wcslen(stored_function) == 0) {
			solve_function($1.sv, NULL, 0, &$$);
		} else if (wcscmp(stored_function, $1.sv) == 0) {
			YYERROR_MSG(FAILURE_INVALID_NAME, "A function cannot call itself!");
		}
	}
	| IDENTIFIER LPAREN constant_expression RPAREN {
		if (wcslen(stored_function) == 0) {
			value args[] = { $3 };
			solve_function($1.sv, args, 1, &$$);
		} else if (wcscmp(stored_function, $1.sv) == 0) {
			YYERROR_MSG(FAILURE_INVALID_NAME, "A function cannot call itself!");
		}
	}
	| IDENTIFIER LPAREN expression_list RPAREN {
		if (wcslen(stored_function) == 0) {
			solve_function($1.sv, $3.elements, $3.size, &$$);
			list_destroy(&$3);
		} else if (wcscmp(stored_function, $1.sv) == 0) {
			YYERROR_MSG(FAILURE_INVALID_NAME, "A function cannot call itself!");
		}
	}
	;

expression_list:
	constant_expression COMMA constant_expression {
		if (wcslen(stored_function) == 0) {
			if (list_create(&$$, DEFAULT_LIST_CAPACITY) != NO_FAILURE) {
				YYERROR_CODE(FAILURE_ALLOCATION);
			}

			list_add(&$$, $1);
			list_add(&$$, $3);
		}
	}
	| expression_list COMMA constant_expression {
		if (wcslen(stored_function) == 0) {
			if (list_add(&$$, $3) != NO_FAILURE) {
				YYERROR_CODE(FAILURE_ALLOCATION);
			}
		}
	}
	;

left_opside_funct_expression:
	IDENTIFIER LPAREN RPAREN EQUAL {
		function *fn = (function*) malloc(sizeof(function));
		if (fn->expression == NULL) {
			YYERROR_CODE(FAILURE_ALLOCATION);
		}

		fn->expression = (wchar_t*) malloc(sizeof(wchar_t)*(wcslen($4.sv)+1));
		if (fn->expression == NULL) {
			YYERROR_CODE(FAILURE_ALLOCATION);
		}

		fn->n_args = 0;
		wcscpy(stored_function, $1.sv);
		put_function($1.sv, fn);
	}
	| IDENTIFIER LPAREN IDENTIFIER RPAREN EQUAL {
		function *fn = (function*) malloc(sizeof(function));
		if (fn->expression == NULL) {
			YYERROR_CODE(FAILURE_ALLOCATION);
		}

		fn->expression = (wchar_t*) malloc(sizeof(wchar_t)*(wcslen($4.sv)+1));
		if (fn->expression == NULL) {
			YYERROR_CODE(FAILURE_ALLOCATION);
		}

		fn->arguments[0] = malloc(sizeof(wchar_t)*(wcslen($3.sv)+1));
		if (fn->arguments[0] == NULL) {
			YYERROR_CODE(FAILURE_ALLOCATION);
		}

		wcscpy(fn->arguments[0], $3.sv);
		fn->n_args = 1;

		wcscpy(stored_function, $1.sv);
		put_function($1.sv, fn);
	}
	| IDENTIFIER LPAREN identifier_list RPAREN EQUAL {
		int i;
		function *fn = (function*) malloc(sizeof(function));
		if (fn->expression == NULL) {
			YYERROR_CODE(FAILURE_ALLOCATION);
		}

		fn->expression = (wchar_t*) malloc(sizeof(wchar_t)*(wcslen($4.sv)+1));
		if (fn->expression == NULL) {
			YYERROR_CODE(FAILURE_ALLOCATION);
		}

		fn->n_args = $3.size;
		for (i=0; i<$3.size; ++i)
			fn->arguments[i] = $3.elements[i].sv;

		list_destroy(&$3);
		wcscpy(stored_function, $1.sv);
		put_function($1.sv, fn);
	}
	;


assignment_expression:
	IDENTIFIER EQUAL constant_expression {
		value *v = (value*) malloc(sizeof(value));
		if (v == NULL) {
			YYERROR_CODE(FAILURE_ALLOCATION);
		}

		*v = $3;
		put_variable($1.sv, v);

		$$ = $3;
	}
	| left_opside_funct_expression constant_expression {
		wcscpy(stored_function, L"");
		$$ = $2;
	}
	;

identifier_list:
	IDENTIFIER COMMA IDENTIFIER {
		if (list_create(&$$, DEFAULT_LIST_CAPACITY) != NO_FAILURE) {
			YYERROR_CODE(FAILURE_ALLOCATION);
		}

		list_add(&$$, $1);
		list_add(&$$, $3);
	}
	| identifier_list COMMA IDENTIFIER {
		if (list_add(&$$, $3) != NO_FAILURE) {
			YYERROR_CODE(FAILURE_ALLOCATION);
		}
	}
	;

%%

int yyerror(yyscan_t scanner, wchar_t stored_function[], value* result, char response[], const char* msg) {
	char* pos = yyget_text(scanner);
	result->iv = FAILURE_SYNTAX_ERROR;

	if (strlen(pos) == 0)
		sprintf(response, "Error: %s", msg);
	else
		sprintf(response, "Error: %s at '%s'", msg, pos);
	return 1;
}