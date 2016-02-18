#ifndef PARSE_H
	#define PARSE_H

	#define EXPRESSION_MAX_LEN 4096

	/* Helper macros */
	#define TO_RADIANS(x) x * PI / 180.0
	#define RESOLVED_VALUE(v) (v.type == VALUE_FLOAT) ? v.fv : v.iv

	enum failure_t {
		FAILURE_UNKNOWN,
		NO_FAILURE,
		FAILURE_INVALID_ARGS,
		FAILURE_INVALID_NAME,
		FAILURE_SYNTAX_ERROR,
		FAILURE_ALLOCATION,
		FAILURE_TYPE
	};
	
	/* Values of variable types */
	enum valuetype_t {
		VALUE_ERROR,
		VALUE_FLOAT,
		VALUE_INT,
		VALUE_STRING
	};
	typedef long long int int_value_t;

	/* In microsoft land these are the same */
	#ifdef _WIN32
		typedef double float_value_t;
	#else
		typedef long double float_value_t;
	#endif
		
	typedef struct {
		char type;

		int_value_t iv;
		float_value_t fv;
		wchar_t sv[EXPRESSION_MAX_LEN];
	} value;

	int parser_init( void );
	void parser_destroy( void );

	int parse_equation(const wchar_t*, value*, int);
	int call_function(const wchar_t*, value[], int, value*, int);

	int float_value(const value*, float_value_t*);
	int int_value(const value*, int_value_t*);
	int value_type(value*, char*);
	char expression_type(const value*, const value*, int*);
	value verify_expression(const value*, const value*);
#endif