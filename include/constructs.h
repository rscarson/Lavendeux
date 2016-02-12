#ifndef CONSTRUCTS_H
	#define CONSTRUCTS_H

	#include "hashing.h"

	typedef struct {
		const wchar_t* expression;

		int n_args;
		wchar_t** arguments;
	} function;

	/* Defined variables. string->value */
	hash_table variables;

	/* User defined methods. string->function */
	hash_table functions;

	int constructs_init( void );
	void constructs_destroy( void );
	
	void function_destroy(void*);
	void variable_destroy(void*);

	int get_variable(const wchar_t*, value*);
	int put_variable(const wchar_t*, value*);

	int solve_function(const wchar_t*, value[], int, value*);
	int put_function(const wchar_t*, function*);
	void function_remove(const wchar_t*);
#endif