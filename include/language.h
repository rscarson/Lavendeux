#ifndef LANGUAGE_H
	#define LANGUAGE_H

	void language_set_current(int l);
	const wchar_t* language_str(int s);
	char* language_char_str(int s);

	/* Supported Languages */
	enum language_t {
		LANG_EN,
		LANG_FR,

		NUM_LANGS
	};

	/* Language strings */
	enum language_string_t {
		LANG_STR_EXIT,
		LANG_STR_ABOUT,
		LANG_STR_LANGUAGE,
		LANG_STR_ERR_STARTING,
		LANG_STR_NO_EQUATIONS,
		LANG_STR_SILENT_ERRS,
		LANG_STR_ANGLE_UNITS,
		LANG_STR_DEGREES,
		LANG_STR_RADIANS,
		LANG_STR_RUNTIME_ERR,
		LANG_STR_SYNTAX_ERR,
		LANG_STR_ERR_UNKNOWN,
		LANG_STR_ERR_INVALID_ARGS,
		LANG_STR_ERR_INVALID_NAME,
		LANG_STR_ERR_SYNTAX_ERROR,
		LANG_STR_ERR_ALLOCATION,
		LANG_STR_ERR_TYPE,
		LANG_STR_BOOLEAN_FLOAT,
		LANG_STR_FACTORIAL_LT_ZERO,
		LANG_STR_FN_CALL_SELF,
		LANG_STR_DIV_BY_ZERO,

		NUM_LANG_STR
	};

	static const wchar_t* language_lookup[][NUM_LANGS] = {
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

		[LANG_STR_SYNTAX_ERR] = {
			[LANG_EN] = L"Syntax error",
			[LANG_FR] = L"Erreur de syntaxe"
		},

		[LANG_STR_ERR_UNKNOWN] = {
			[LANG_EN] = L"Unknown error",
			[LANG_FR] = L""
		},

		[LANG_STR_ERR_INVALID_ARGS] = {
			[LANG_EN] = L"Invalid arguments supplied to function",
			[LANG_FR] = L""
		},

		[LANG_STR_ERR_INVALID_NAME] = {
			[LANG_EN] = L"Unrecognized or invalid function or variable name",
			[LANG_FR] = L""
		},

		[LANG_STR_ERR_SYNTAX_ERROR] = {
			[LANG_EN] = L"Syntax error",
			[LANG_FR] = L""
		},

		[LANG_STR_ERR_ALLOCATION] = {
			[LANG_EN] = L"Failed to allocate memory",
			[LANG_FR] = L""
		},

		[LANG_STR_ERR_TYPE] = {
			[LANG_EN] = L"Operation not valid for given type",
			[LANG_FR] = L""
		},

		[LANG_STR_BOOLEAN_FLOAT] = {
			[LANG_EN] =L"Floating point arguments not applicable to binary operators",
			[LANG_FR] = L""
		},

		[LANG_STR_FACTORIAL_LT_ZERO] = {
			[LANG_EN] =L"Factorial is undefined for arguments < 0",
			[LANG_FR] = L""
		},

		[LANG_STR_FN_CALL_SELF] = {
			[LANG_EN] =L"A function cannot call itself",
			[LANG_FR] = L""
		},

		[LANG_STR_DIV_BY_ZERO] = {
			[LANG_EN] = L"Division by 0",
			[LANG_FR] = L""
		}
	};

#endif