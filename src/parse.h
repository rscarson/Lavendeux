#ifndef PARSE_H
	#define PARSE_H

	#define FAILURE_UNKNOWN			0
	#define NO_FAILURE				1
	#define FAILURE_INVALID_ARGS	2
	#define FAILURE_INVALID_NAME	3
	#define FAILURE_SYNTAX_ERROR	4

	#include "hashing.h"

	typedef struct {
		const wchar_t* expression;

		int n_args;
		const wchar_t* arguments[];
	} function;

	/* Defined variables. string->double */
	hash_table variables;

	/* User defined methods. string->function */
	hash_table functions;

	int parse_equation(const wchar_t**, const wchar_t*);
	int solve_equation(const wchar_t*, *double);

	int get_variable(const wchar_t*, double*);
	int put_variable(const wchar_t*, double);

	int solve_function(const wchar_t*, double[], int, double*);
	int put_function(const wchar_t*, function*);
#endif