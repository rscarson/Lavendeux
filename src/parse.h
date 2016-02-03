#ifndef PARSE_H
	#define PARSE_H

	#include "hashing.h"

	typedef struct {
		const wchar_t* expression;
		const wchar_t* arguments[];
	} function;

	/* Defined variables. string->float */
	hash_table variables;

	/* User defined methods. string->function */
	hash_table functions;

	int parse_equation(const wchar_t*, float*);

	int get_variable(const wchar_t*, float*);
	void put_variable(const wchar_t*, float);

	int get_function(function, float[], float*);
	void put_function(const wchar_t*, function);
#endif