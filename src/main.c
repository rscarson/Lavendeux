#include <stdio.h>
#include <stdlib.h>

#include "ub3rparse.h"
#include "interface.h"

int main() {
	init_interface(exit_callback, parse_callback, setting_callback);

	while (1) {
		update_interface();
	}

	return 0;
}

void parse_callback(const char *target) {
	int next;
	add_history(target);
	printf("%s\n", target);

	yyin = yy_scan_string(target);
	while (!feof(yyin)) {
		next = yyparse();
		if (next == 1)
			break;
		else
			printf("Found a %d\n", next);
	}
}

void setting_callback(int setting, int value) {
	switch (setting) {
		case SETTING_ANGLE:
			angle_mode = value;
		break;

		case SETTING_SILENT:
			silent_mode = value;
		break;
	}
}

void exit_callback( void ) {
	printf("Bye!\n");
	exit(0);
}