#include <stdio.h>

#include "menu.h"

int main() {
	initMenu();

	while (updateMenu()) {}

	return 0;
}