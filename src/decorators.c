#include "hashing.h"
#include "parse.h"
#include "decorators.h"

void init_decorators( void ) {
	table_create(decorators, HASH_DEFAULT_SIZE);

	table_put(decorators, "i", decorator_int);
	table_put(decorators, "int", decorator_int);
	table_put(decorators, "integer", decorator_int);

	table_put(decorators, "f", decorator_float);
	table_put(decorators, "float", decorator_float);

	table_put(decorators, "o", decorator_oct);
	table_put(decorators, "oct", decorator_oct);
	table_put(decorators, "octal", decorator_oct);

	table_put(decorators, "h", decorator_hex);
	table_put(decorators, "hex", decorator_hex);
	table_put(decorators, "hexadecimal", decorator_hex);

	table_put(decorators, "sci", decorator_sci);
	table_put(decorators, "scientific", decorator_sci);
}

const wchar_t* decorate(const char* name, value* v) {
	decorator_fn *fn;
	if (table_get(decorators, name, fn) != NO_FAILURE)
		return NULL;
	return fn(v);
}

const wchar_t* decorator_int(value* v) {
	wchar_t decorated[255];
	swprintf(decorated, L"%Ld", int_value(v));
}

const wchar_t* decorator_float(value* v) {
	wchar_t decorated[255];
	swprintf(decorated, L"%Lf", float_value(v));
}

const wchar_t* decorator_oct(value* v) {
	wchar_t decorated[255];
	swprintf(decorated, L"0o%Lo", int_value(v));
}

const wchar_t* decorator_hex(value* v) {
	wchar_t decorated[255];
	swprintf(decorated, L"0x%Lx", int_value(v));
}

const wchar_t* decorator_sci(value* v) {
	wchar_t decorated[255];
	swprintf(decorated, L"%LE", int_value(v));
}