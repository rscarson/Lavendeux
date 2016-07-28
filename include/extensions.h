#ifndef EXTENSIONS_H
	#define EXTENSIONS_H
	
	#define LAVENDEUX_MODULE "lavendeux"
	#define EXTENSIONS_PATH "./extensions"
	#define EXTENSIONS_LIB_PATH "./lib"
	#define EXTENSIONS_FUNCTION "call"
	#define EXTENSIONS_DECORATOR "decorate"
	#define EXTENSIONS_HELP "help"

	#include <stdlib.h>
	#include <wchar.h>

	#include "parse.h"
	
	void extensions_init( void );
	int extensions_homeset( void );
	int extensions_available( void );
	void extensions_destroy( void );
	int extensions_decorate(const char*, value, wchar_t*);
	int extensions_call(const char*, value[], int, value*);
	int extensions_help(const char*, value*);
#endif