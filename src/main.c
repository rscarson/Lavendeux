#include <stdio.h>
#include <stdlib.h>

#include "interface.h"

int main() {
	init_interface(exit_callback, parse_callback, setting_callback);

	while (1) {
		update_interface();
	}

	return 0;
}

void parse_callback(const char *target) {
	add_history(target);
	printf("%s\n", target);
}

void setting_callback(int setting, int value) {
	printf("Setting %d = %d\n", setting, value);
}

void exit_callback( void ) {
	printf("Bye!\n");
	exit(0);
}