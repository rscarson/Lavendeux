#ifndef DECORATORS_H
	#define DECORATORS_H

	#include "hashing.h"
	
	typedef const wchar_t* (*decorator_fn)(value*);

	/* Decorator functions */
	const wchar_t* decorate(const char*, value*);
	const wchar_t* decorator_int(value*);
	const wchar_t* decorator_float(value*);
	const wchar_t* decorator_oct(value*);
	const wchar_t* decorator_hex(value*);
	const wchar_t* decorator_sci(value*);

	/* Decorator table */
	void init_decorators( void );
	hash_table *decorators;
#endif