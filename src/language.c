#include <stdlib.h>
#include <string.h>
#include "language.h"

int current_lang = LANG_EN;

void language_set_current(int l) {
	current_lang = l;
}

const wchar_t* language_str(int s) {
	return language_lookup[s][current_lang];
}

char* language_char_str(int s) {
	const wchar_t* wstr = language_str(s);
	char* str = (char*) malloc(sizeof(char) * (wcslen(wstr)+1));
	wcstombs(str, wstr, wcslen(wstr));

	return str;
}