#ifndef LANGUAGE_H
	#define LANGUAGE_H
	
	/* Language strings */
	#define NUM_LANGS 2
	#define LANG_EN 0
	#define LANG_FR 1
	#define LANG_STR_EXIT 	0x00
	#define LANG_STR_ABOUT 	0x01

	/* Language structure */
	static const char* lang_lookup[][NUM_LANGS] = {
		[LANG_STR_EXIT] = {
			[LANG_EN] = "Exit",
			[LANG_FR] = "Sortie"
		},

		[LANG_STR_ABOUT] = {
			[LANG_EN] = "About",
			[LANG_FR] = "À proppos"
		}
	};
#endif