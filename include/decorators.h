#ifndef DECORATORS_H
	#define DECORATORS_H

	#include "parse.h"
	#include "hashing.h"
	
	typedef void (*decorator_fn)(const value*, wchar_t*);

	/* For static table */
	typedef struct { const wchar_t* name; decorator_fn fn; } static_decorator_fn;

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
	int init_decorators( void );
	void decorators_destroy( void );
	hash_table decorators;

	/* Table entries */
	static const static_decorator_fn decorator_function_declarations[] = {
		{ L"u", decorator_unsigned },
		{ L"unsigned", decorator_unsigned },

		{ L"i", decorator_int },
		{ L"int", decorator_int },
		{ L"integer", decorator_int },

		{ L"f", decorator_float },
		{ L"float", decorator_float },

		{ L"sci", decorator_sci },
		{ L"scientific", decorator_sci },

		{ L"b", decorator_bin },
		{ L"bin", decorator_bin },
		{ L"binary", decorator_bin },

		{ L"o", decorator_oct },
		{ L"oct", decorator_oct },
		{ L"octal", decorator_oct },

		{ L"h", decorator_hex },
		{ L"hex", decorator_hex },
		{ L"hexadecimal", decorator_hex },
	};
#endif