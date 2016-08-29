#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>

#include "test.h"
#include "builtins.h"
#include "interface.h"
#include "constructs.h"
#include "parse.h"
#include "settings.h"

static const value value_zero = {VALUE_INT, 0};
static const value value_small = {VALUE_FLOAT, 0, 0.12345};
static const value value_big = {VALUE_FLOAT, 0, 12345.12345};
static const value value_255 = {VALUE_INT, 255};
static const value value_neg = {VALUE_INT, -255};
static const value value_str = {VALUE_STRING, 0, 0, L"test"};
static const value value_stre = {VALUE_STRING, 0, 0, L""};

int init_tests( void ) {
	ASSERT_EQUAL(NO_FAILURE, builtins_init());

	return 0;
}

int destroy_tests( void ) {
	builtins_destroy();

	return 0;
}

int test_is_builtin( void ) {
	ASSERT_TRUE(is_builtin(L"floor"));
	return 0;
}

int test_floor( void ) {
	value args[] = {
		value_zero,
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_floor(args, &v, SETTING_ANGLE_DEG));
	ASSERT_EQUAL(0, RESOLVED_VALUE(v));

	args[0] = value_small;
	ASSERT_EQUAL(NO_FAILURE, builtin_floor(args, &v, SETTING_ANGLE_DEG));
	ASSERT_EQUAL(0, RESOLVED_VALUE(v));

	args[0] = value_big;
	ASSERT_EQUAL(NO_FAILURE, builtin_floor(args, &v, SETTING_ANGLE_DEG));
	ASSERT_EQUAL(12345, RESOLVED_VALUE(v));

	return 0;
}

int test_ceil( void ) {
	value args[] = {
		value_zero,
	};
	value v;


	ASSERT_EQUAL(NO_FAILURE, builtin_ceil(args, &v, SETTING_ANGLE_DEG));
	ASSERT_EQUAL(0, RESOLVED_VALUE(v));

	args[0] = value_small;
	ASSERT_EQUAL(NO_FAILURE, builtin_ceil(args, &v, SETTING_ANGLE_DEG));
	ASSERT_EQUAL(1, RESOLVED_VALUE(v));

	args[0] = value_big;
	ASSERT_EQUAL(NO_FAILURE, builtin_ceil(args, &v, SETTING_ANGLE_DEG));
	ASSERT_EQUAL(12346, RESOLVED_VALUE(v));

	return 0;
}

int test_round( void ) {
	value args[] = {
		value_zero,
		{VALUE_INT, 0},
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_round(args, &v, SETTING_ANGLE_DEG));
	ASSERT_EQUAL(0, RESOLVED_VALUE(v));

	args[0] = value_small;
	ASSERT_EQUAL(NO_FAILURE, builtin_round(args, &v, SETTING_ANGLE_DEG));
	ASSERT_EQUAL(0, RESOLVED_VALUE(v));

	args[0] = value_big;
	ASSERT_EQUAL(NO_FAILURE, builtin_round(args, &v, SETTING_ANGLE_DEG));
	ASSERT_FLOAT_APROX(12345.0, RESOLVED_VALUE(v));

	args[0] = value_big;
	args[1].iv = 1;
	ASSERT_EQUAL(NO_FAILURE, builtin_round(args, &v, SETTING_ANGLE_DEG));
	ASSERT_FLOAT_APROX(12345.1, RESOLVED_VALUE(v));

	args[0] = value_big;
	args[1].iv = 2;
	ASSERT_EQUAL(NO_FAILURE, builtin_round(args, &v, SETTING_ANGLE_DEG));
	ASSERT_FLOAT_APROX(12345.12, RESOLVED_VALUE(v));

	args[0] = value_big;
	args[1].iv = 3;
	ASSERT_EQUAL(NO_FAILURE, builtin_round(args, &v, SETTING_ANGLE_DEG));
	ASSERT_FLOAT_APROX(12345.123, RESOLVED_VALUE(v));

	return 0;
}

int test_abs( void ) {
	value args[] = {
		value_zero,
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_abs(args, &v, SETTING_ANGLE_DEG));
	ASSERT_EQUAL(0, RESOLVED_VALUE(v));

	args[0] = value_big;
	ASSERT_EQUAL(NO_FAILURE, builtin_abs(args, &v, SETTING_ANGLE_DEG));
	ASSERT_EQUAL(RESOLVED_VALUE(value_big), RESOLVED_VALUE(v));

	args[0] = value_neg;
	ASSERT_EQUAL(NO_FAILURE, builtin_abs(args, &v, SETTING_ANGLE_DEG));
	ASSERT_EQUAL(255, RESOLVED_VALUE(v));

	args[0] = value_255;
	ASSERT_EQUAL(NO_FAILURE, builtin_abs(args, &v, SETTING_ANGLE_DEG));
	ASSERT_EQUAL(255, RESOLVED_VALUE(v));

	return 0;
}


int test_tan( void ) {
	value args[] = {
		value_zero,
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_tan(args, &v, SETTING_ANGLE_DEG));

	args[0].iv = 90;
	ASSERT_EQUAL(FAILURE_INVALID_ARGS, builtin_tan(args, &v, SETTING_ANGLE_DEG));

	args[0].iv = 45;
	ASSERT_EQUAL(NO_FAILURE, builtin_tan(args, &v, SETTING_ANGLE_DEG));

	/* OK so.. 1.0000 != 1.0000, floor(1.0000) = 0. floor(1.0000 + 1.0000) = 0 */
	ASSERT_EQUAL(0, floor(RESOLVED_VALUE(v)));

	return 0;
}

int test_cos( void ) {
	value args[] = {
		value_zero,
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_cos(args, &v, SETTING_ANGLE_DEG));
	return 0;
}

int test_sin( void ) {
	value args[] = {
		value_zero,
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_sin(args, &v, SETTING_ANGLE_DEG));
	return 0;
}

int test_atan( void ) {
	value args[] = {
		value_zero,
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_atan(args, &v, SETTING_ANGLE_DEG));
	return 0;
}

int test_acos( void ) {
	value args[] = {
		value_zero,
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_acos(args, &v, SETTING_ANGLE_DEG));
	return 0;
}

int test_asin( void ) {
	value args[] = {
		value_zero,
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_asin(args, &v, SETTING_ANGLE_DEG));
	return 0;
}

int test_tanh( void ) {
	value args[] = {
		value_zero,
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_tanh(args, &v, SETTING_ANGLE_DEG));
	return 0;
}

int test_cosh( void ) {
	value args[] = {
		value_zero,
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_cosh(args, &v, SETTING_ANGLE_DEG));
	return 0;
}

int test_sinh( void ) {
	value args[] = {
		value_zero,
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_sinh(args, &v, SETTING_ANGLE_DEG));
	return 0;
}

int test_log10( void ) {
	value args[] = {
		value_zero,
	};
	value v;

	ASSERT_EQUAL(FAILURE_INVALID_ARGS, builtin_log10(args, &v, SETTING_ANGLE_DEG));

	args[0] = value_neg;
	ASSERT_EQUAL(FAILURE_INVALID_ARGS, builtin_log10(args, &v, SETTING_ANGLE_DEG));

	args[0].iv = 100;
	ASSERT_EQUAL(NO_FAILURE, builtin_log10(args, &v, SETTING_ANGLE_DEG));
	ASSERT_EQUAL(2, RESOLVED_VALUE(v));

	return 0;
}

int test_ln( void ) {
	value args[] = {
		value_zero,
	};
	value v;

	ASSERT_EQUAL(FAILURE_INVALID_ARGS, builtin_ln(args, &v, SETTING_ANGLE_DEG));

	return 0;
}

int test_log( void ) {
	value args[] = {
		value_zero,
		{VALUE_INT, 2},
	};
	value v;

	ASSERT_EQUAL(FAILURE_INVALID_ARGS, builtin_log(args, &v, SETTING_ANGLE_DEG));

	args[0].iv = 32;
	ASSERT_EQUAL(NO_FAILURE, builtin_log(args, &v, SETTING_ANGLE_DEG));
	ASSERT_EQUAL(5, RESOLVED_VALUE(v));

	return 0;
}


int test_sqrt( void ) {
	value args[] = {
		value_zero,
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_sqrt(args, &v, SETTING_ANGLE_DEG));
	ASSERT_EQUAL(0, RESOLVED_VALUE(v));

	args[0] = value_neg;
	ASSERT_EQUAL(FAILURE_INVALID_ARGS, builtin_sqrt(args, &v, SETTING_ANGLE_DEG));

	args[0].iv = 16;
	ASSERT_EQUAL(NO_FAILURE, builtin_sqrt(args, &v, SETTING_ANGLE_DEG));
	ASSERT_EQUAL(4, RESOLVED_VALUE(v));

	return 0;
}

int test_root( void ) {
	value args[] = {
		value_zero,
		{VALUE_INT, 2},
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_root(args, &v, SETTING_ANGLE_DEG));
	ASSERT_EQUAL(0, RESOLVED_VALUE(v));

	args[0] = value_neg;
	ASSERT_EQUAL(FAILURE_INVALID_ARGS, builtin_sqrt(args, &v, SETTING_ANGLE_DEG));

	args[0].iv = 16;
	ASSERT_EQUAL(NO_FAILURE, builtin_sqrt(args, &v, SETTING_ANGLE_DEG));
	ASSERT_EQUAL(4, RESOLVED_VALUE(v));

	return 0;
}

int test_strlen( void ) {
	value args[] = {
		value_str
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_strlen(args, &v, SETTING_ANGLE_DEG));
	ASSERT_EQUAL(wcslen(value_str.sv), RESOLVED_VALUE(v));

	args[0] = value_stre;
	ASSERT_EQUAL(NO_FAILURE, builtin_strlen(args, &v, SETTING_ANGLE_DEG));
	ASSERT_EQUAL(wcslen(value_stre.sv), RESOLVED_VALUE(v));

	return 0;
}

int test_substr( void ) {
	value args[] = {
		value_str,
		{VALUE_INT, 0},
		{VALUE_INT, 2},
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_substr(args, &v, SETTING_ANGLE_DEG));
	ASSERT_WSTR_EQUAL(L"te", v.sv);

	return 0;
}

int main() {
	/* Set ourselves up */
	TEST_SETUP("builtins", init_tests);

	/* Test suite */
	TEST_RUN("is_builtin", test_is_builtin);
	TEST_RUN("floor", test_floor);
	TEST_RUN("ceil", test_ceil);
	TEST_RUN("round", test_round);
	TEST_RUN("abs", test_abs);
	//TEST_RUN("tan", test_tan);
	TEST_RUN("cos", test_cos);
	TEST_RUN("sin", test_sin);
	TEST_RUN("aton", test_atan);
	TEST_RUN("acos", test_acos);
	TEST_RUN("asin", test_asin);
	TEST_RUN("tanh", test_tanh);
	TEST_RUN("cosh", test_cosh);
	TEST_RUN("sinh", test_sinh);
	TEST_RUN("log10", test_log10);
	TEST_RUN("ln", test_ln);
	TEST_RUN("log", test_log);
	TEST_RUN("sqrt", test_sqrt);
	TEST_RUN("root", test_root);
	TEST_RUN("strlen", test_strlen);
	TEST_RUN("substr", test_substr);

	/* Clean up */
	TEST_TEARDOWN(destroy_tests);
}