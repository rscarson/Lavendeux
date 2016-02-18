#include <stdlib.h>
#include <stdio.h>

#include "test.h"
#include "builtins.h"
#include "interface.h"
#include "constructs.h"
#include "parse.h"

static const value value_zero = {VALUE_INT, 0};
static const value value_small = {VALUE_FLOAT, 0, 0.12345};
static const value value_big = {VALUE_FLOAT, 0, 12345.12345};
static const value value_255 = {VALUE_INT, 255};
static const value value_neg = {VALUE_INT, -255};

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

	ASSERT_EQUAL(NO_FAILURE, builtin_floor(args, &v, SETTING_ANGLE_DEG));
	return 0;
}

int test_round( void ) {
	value args[] = {
		value_zero,
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_floor(args, &v, SETTING_ANGLE_DEG));
	return 0;
}

int test_abs( void ) {
	value args[] = {
		value_zero,
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_floor(args, &v, SETTING_ANGLE_DEG));
	return 0;
}


int test_tan( void ) {
	value args[] = {
		value_zero,
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_floor(args, &v, SETTING_ANGLE_DEG));
	return 0;
}

int test_cos( void ) {
	value args[] = {
		value_zero,
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_floor(args, &v, SETTING_ANGLE_DEG));
	return 0;
}

int test_sin( void ) {
	value args[] = {
		value_zero,
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_floor(args, &v, SETTING_ANGLE_DEG));
	return 0;
}

int test_atan( void ) {
	value args[] = {
		value_zero,
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_floor(args, &v, SETTING_ANGLE_DEG));
	return 0;
}

int test_acos( void ) {
	value args[] = {
		value_zero,
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_floor(args, &v, SETTING_ANGLE_DEG));
	return 0;
}

int test_asin( void ) {
	value args[] = {
		value_zero,
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_floor(args, &v, SETTING_ANGLE_DEG));
	return 0;
}

int test_tanh( void ) {
	value args[] = {
		value_zero,
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_floor(args, &v, SETTING_ANGLE_DEG));
	return 0;
}

int test_cosh( void ) {
	value args[] = {
		value_zero,
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_floor(args, &v, SETTING_ANGLE_DEG));
	return 0;
}

int test_sinh( void ) {
	value args[] = {
		value_zero,
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_floor(args, &v, SETTING_ANGLE_DEG));
	return 0;
}

int test_log10( void ) {
	value args[] = {
		value_zero,
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_floor(args, &v, SETTING_ANGLE_DEG));
	return 0;
}

int test_ln( void ) {
	value args[] = {
		value_zero,
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_floor(args, &v, SETTING_ANGLE_DEG));
	return 0;
}

int test_log( void ) {
	value args[] = {
		value_zero,
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_floor(args, &v, SETTING_ANGLE_DEG));
	return 0;
}


int test_sqrt( void ) {
	value args[] = {
		value_zero,
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_floor(args, &v, SETTING_ANGLE_DEG));
	return 0;
}

int test_root( void ) {
	value args[] = {
		value_zero,
	};
	value v;

	ASSERT_EQUAL(NO_FAILURE, builtin_floor(args, &v, SETTING_ANGLE_DEG));
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
	TEST_RUN("tan", test_tan);
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

	/* Clean up */
	TEST_TEARDOWN(destroy_tests);
}