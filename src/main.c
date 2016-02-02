#include <stdio.h>

#include "menu.h"

int main() {
	const char* equation;
	initMenu();

	while (updateMenu()) {
		equation = lastEquation();
		if (equation != NULL) {
			addEquation(equation);
			printf("%s\n", equation);
		}
	}

	return 0;
}