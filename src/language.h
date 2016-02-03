#ifndef LANGUAGE_H
	#define LANGUAGE_H
	
	/* Language strings */
	#define NUM_LANGS 2
	#define LANG_EN 0
	#define LANG_FR 1
	#define LANG_STR_EXIT 				0x00
	#define LANG_STR_ABOUT 				0x01
	#define LANG_STR_LANGUAGE		 	0x02 //Language
	#define LANG_STR_ERR_STARTING 		0x03 //Error while starting
	#define LANG_STR_NO_EQUATIONS	 	0x04 //No equations in history
	#define LANG_STR_SILENT_ERRS	 	0x05 //Silent Errors
	#define LANG_STR_ANGLE_UNITS	 	0x06 //Angle Units
	#define LANG_STR_DEGREES	 		0x07 //Degrees
	#define LANG_STR_RADIANS		 	0x08 //Radians
	#define LANG_STR_RUNTIME_ERR	 	0x09 //Runtime error

	/* Language structure */
	static const wchar_t* lang_lookup[][NUM_LANGS] = {
		[LANG_STR_EXIT] = {
			[LANG_EN] = L"Exit",
			[LANG_FR] = L"Sortie"
		},

		[LANG_STR_ABOUT] = {
			[LANG_EN] = L"About",
			[LANG_FR] = L"À proppos"
		},

		[LANG_STR_LANGUAGE] = {
			[LANG_EN] = L"Language",
			[LANG_FR] = L"Langue"
		},

		[LANG_STR_ERR_STARTING] = {
			[LANG_EN] = L"Error while starting",
			[LANG_FR] = L"Erreur au démarrage"
		},

		[LANG_STR_NO_EQUATIONS] = {
			[LANG_EN] = L"No equations in history",
			[LANG_FR] = L"Aucun équations dans l'histoire"
		},

		[LANG_STR_SILENT_ERRS] = {
			[LANG_EN] = L"Silent Errors",
			[LANG_FR] = L"Erreurs Silencieux"
		},

		[LANG_STR_ANGLE_UNITS] = {
			[LANG_EN] = L"Angle Units",
			[LANG_FR] = L"Unités des Angles"
		},

		[LANG_STR_DEGREES] = {
			[LANG_EN] = L"Degrees",
			[LANG_FR] = L"Degrés"
		},

		[LANG_STR_RADIANS] = {
			[LANG_EN] = L"Radians",
			[LANG_FR] = L"Radians"
		},

		[LANG_STR_RUNTIME_ERR] = {
			[LANG_EN] = L"Runtime error",
			[LANG_FR] = L"Erreur en exécution"
		},
	};
#endif