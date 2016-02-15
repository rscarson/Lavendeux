#ifndef DECORATORS_H
	#define DECORATORS_H

	#include "parse.h"
	#include "hashing.h"
	
	typedef void (*decorator_fn)(const value*, wchar_t*);

	/* Decorator functions */
	int decorate(const wchar_t*, const value*, wchar_t*);
	void decorator_int(const value*, wchar_t*);
	void decorator_unsigned(const value*, wchar_t*);
	void decorator_float(const value*, wchar_t*);
	void decorator_sci(const value*, wchar_t*);
	void decorator_bin(const value*, wchar_t*);
	void decorator_oct(const value*, wchar_t*);
	void decorator_hex(const value*, wchar_t*);

	/* Decorator table */
	void init_decorators( void );
	hash_table decorators;
#endif