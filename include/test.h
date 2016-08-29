#ifndef TEST_H
	#define TEST_H

	#include <stdlib.h>
	#include <string.h>
	#include <wchar.h>

	#define TEST_FAIL(s){ printf("Failed in %s, line %d\n", __FILE__, __LINE__); return 1; }
	
	#define TEST_SETUP(name, fn)  if (init_testsuite(name, fn) != 0) return 1;
	#define TEST_TEARDOWN(fn)  if (teardown_testsuite(fn) != 0) return 1;
	#define TEST_RUN(name, fn) if (call_test(name, fn) != 0) return 1;
	
	#define ASSERT_NULL(v) if (v != NULL) TEST_FAIL(); 
	#define ASSERT_NOT_NULL(v) if (v == NULL) TEST_FAIL(); 

	#define ASSERT_TRUE(v) if (!v) TEST_FAIL(); 
	#define ASSERT_FALSE(v) if (v) TEST_FAIL(); 

	#define ASSERT_EQUAL(l, r) if (l != r) TEST_FAIL();
	#define ASSERT_NOT_EQUAL(l, r) if (l == r) TEST_FAIL();

	#define ASSERT_FLOAT_APROX(l, r) if ((int)(100.0*((double) l)) != (int)(100.0*((double)r))) TEST_FAIL();

	#define ASSERT_STR_EQUAL(l, r) if (strcmp(l, r) != 0) TEST_FAIL();
	#define ASSERT_STR_NOT_EQUAL(l, r) if (strcmp(l, r) == 0) TEST_FAIL();

	#define ASSERT_WSTR_EQUAL(l, r) if (wcscmp(l, r) != 0) TEST_FAIL();
	#define ASSERT_WSTR_NOT_EQUAL(l, r) if (wcscmp(l, r) == 0) TEST_FAIL();
	
	typedef int (*test_fn) ( void );

	int init_testsuite(const char*, test_fn);
	int teardown_testsuite(test_fn);
	int call_test(const char*, test_fn);
#endif