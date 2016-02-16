#ifndef CONSTRUCTS_H
	#define CONSTRUCTS_H

	#include "parse.h"
	#include "hashing.h"

	/* Defined variables. string->value */
	hash_table variables;

	/* User defined methods. string->function */
	hash_table functions;

	int constructs_init( void );
	void constructs_destroy( void );
	
	void variable_destroy(void*);

	int get_variable(const wchar_t*, value*);
	int put_variable(const wchar_t*, value*);
#endif