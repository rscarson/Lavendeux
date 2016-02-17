#include <stdlib.h>
#include <stdio.h>

#include "test.h"

int call_test(const char* name, test_fn fn) {
	printf("Running test: %s... ", name);
	if (fn() == 0) {
		printf("ok\n");
		return 0;
	}
	
	return 1;
}

int init_testsuite(const char* name, test_fn fn) {
	printf("\nSetting up test suite %s... ", name);
	if (fn() == 0) {
		printf("ok\n");
		return 0;
	}
	
	return 1;
}

int teardown_testsuite(test_fn fn) {
	printf("Cleaning up... ");
	if (fn() == 0) {
		printf("ok\n");
		return 0;
	}
	
	return 1;
}