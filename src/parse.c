#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "parse.h"
#include "language.h"
#include "constructs.h"
#include "list.h"

#include "tab.h"
#include "lex.h"

/**
 * Process an equation and format the response
 * @param equation The expression to solve
 * @param response value to put the result in
 *
 * @return int 0 if failed, non 0 otherwise
 */
extern int yyparse (yyscan_t, value*);
int parse_equation(const wchar_t* equation, value* response){
	char parse_error[EXPRESSION_MAX_LEN];
	wchar_t stored_function[EXPRESSION_MAX_LEN];
	yyscan_t myscanner;

	yylex_init(&myscanner);
	wcscpy(stored_function, L"");

	char* equation_mbs = (char*) malloc(sizeof(char)*(wcslen(equation)+1));
	if (equation_mbs == NULL) {
		wcscpy(response->sv, language_str(LANG_STR_ERR_ALLOCATION));

    	yylex_destroy(myscanner);
		return FAILURE_ALLOCATION;
	}

	wcstombs(equation_mbs, equation, wcslen(equation));
	equation_mbs[wcslen(equation)] = L'\0';
	yy_scan_string(equation_mbs, myscanner);

	if (yyparse(myscanner, stored_function, response) == 1) {
		free(equation_mbs);
    	yylex_destroy(myscanner);
		return response->iv;
	}

	free(equation_mbs);
    yylex_destroy(myscanner);
	return NO_FAILURE;
}

int float_value(const value* v, float_value_t *out) {
	value resolved;
	int result;

	switch (v->type) {
		case VALUE_INT:
			*out = (float_value_t) v->iv;
			return NO_FAILURE;
		break;

		case VALUE_FLOAT:
			*out = v->fv;
			return NO_FAILURE;
		break;

		case VALUE_STRING:
			if( (result = get_variable(v->sv, &resolved)) == NO_FAILURE )
				return float_value(&resolved, out);
			else return result;
		break;
	}

	return FAILURE_ALLOCATION;
}

int int_value(const value* v, int_value_t *out) {
	value resolved;
	int result;

	switch (v->type) {
		case VALUE_INT:
			*out = v->iv;
			return NO_FAILURE;
		break;

		case VALUE_FLOAT:
			*out = (int_value_t) v->fv;
			return NO_FAILURE;
		break;

		case VALUE_STRING:
			if( (result = get_variable(v->sv, &resolved)) == NO_FAILURE )
				return int_value(&resolved, out);
			else return result;
		break;
	}

	return FAILURE_ALLOCATION;
}

int value_type(const value* v, char* type) {
	value resolved;
	int result;

	if (v->type == VALUE_STRING) {
		result = get_variable(v->sv, &resolved);
		if (result != NO_FAILURE)
			return result;
		return value_type(&resolved, type);
	}
		
	*type = v->type;
	return NO_FAILURE;
}

/**
 * Return the expected type of an expression
 * @param left Pointer to left value or NULL
 * @param right Pointer to right value or NULL
 *
 * @return A value type
 */
char expression_type(const value* left, const value* right, int *result) {
	char left_type;
	char right_type;
	*result = NO_FAILURE;
	
	if (left != NULL) {
		*result = value_type(left, &left_type);
		if (*result != NO_FAILURE)
			return VALUE_ERROR;
		
		if (left_type == VALUE_FLOAT)
			return VALUE_FLOAT;
	}
	
	if (right != NULL) {
		*result = value_type(right, &right_type);
		if (*result != NO_FAILURE)
			return VALUE_ERROR;
		
		if (right_type == VALUE_FLOAT)
			return VALUE_FLOAT;
	}

	return VALUE_INT;
}

/* On error, put code in value->iv */
value verify_expression(const value* left, const value* right) {
	value v;
	int error;

	v.type = expression_type(left, right, &error);
	if (v.type == VALUE_ERROR) {
		v.iv = error;
	}

	return v;
}