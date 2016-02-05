#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "ub3rparse.h"
#include "language.h"
#include "interface.h"

int main() {
	init_interface(exit_callback, parse_callback, setting_callback);

	while (1) {
		update_interface();
	}

	return 0;
}

void parse_callback(const wchar_t *target) {
}

void parse_expression(const wchar_t* expression) {
}

void setting_callback(int setting, int value) {
}

void exit_callback( void ) {
	printf("Bye!\n");
	exit(0);
}