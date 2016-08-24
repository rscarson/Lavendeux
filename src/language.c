#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "language.h"

int current_lang = LANG_EN;

/**
 * Set the current language
 * @param l Language code
 */
void language_set_current(int l) {
	current_lang = l;
}

/**
 * Wide language string
 * @param s String code
 *
 * @return WCS
 */
const wchar_t* language_str(int s) {
	return language_lookup[s][current_lang];
}

/**
 * Short language string. Must be freed after use
 * @param s String code
 *
 * @return MBS
 */
char* language_char_str(int s) {
	const wchar_t* wstr = language_str(s);
	char* str = (char*) malloc(sizeof(char) * (wcslen(wstr)+1));
	str[ wcstombs(str, wstr, wcslen(wstr)) ] = '\0';

	return str;
}