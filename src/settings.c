#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "settings.h"
#include "language.h"
	
/* Options */
int config_active = 0;
char* config_location;
int settings[N_SETTINGS];

/**
 * Read in settings
 * @param path Destination file
 */
void init_settings(const char* path) {
	int i, setting, value;
	FILE* config;
	if (config_active)
		return;

    /* Default options */
    for (i=0; i<N_SETTINGS; ++i)
        settings[i] = 0;

    /* Init */
    if (path != NULL) {
    	config_location = malloc(sizeof(char)*(strlen(path)+1));
    	strcpy(config_location, path);
    } else {
    	config_location = NULL;
    	config_active = 1;
    }

	/* Read in settings */
    config = fopen(config_location, "r");
	if (config != NULL) {
		while (fscanf(config, " %d = %d \n", &setting, &value) == 2) {
			if (setting < N_SETTINGS)
				set_setting(setting, value);
		}

		fclose(config);
	}

	if (get_setting(SETTING_LANG) < NUM_LANGS)
		language_set_current(get_setting(SETTING_LANG));
}

/**
 * Save settings
 * @param path Destination file
 */
void save_settings( void ) {
	FILE* config;

	/* Open config */
	config = fopen(config_location, "w+");
	if (config != NULL) {
		/* Write all settings out */
		fprintf(config, "%d=%d\n", SETTING_ANGLE, get_setting(SETTING_ANGLE));
		fprintf(config, "%d=%d\n", SETTING_SILENT, get_setting(SETTING_SILENT));
		fprintf(config, "%d=%d\n", SETTING_LANG, get_setting(SETTING_LANG));
		fprintf(config, "%d=%d\n", SETTING_AUTOCOPY, get_setting(SETTING_AUTOCOPY));
		fprintf(config, "%d=%d\n", SETTING_SILENTSTART, get_setting(SETTING_SILENTSTART));
		fprintf(config, "%d=%d\n", SETTING_HOTMOD, get_setting(SETTING_HOTMOD));
		fprintf(config, "%d=%d\n", SETTING_HOTKEY, get_setting(SETTING_HOTKEY));

		/* Close up and leave */
		fclose(config);
	}

	if (config_location != NULL)
		free(config_location);
}

/**
 * Get the value of a given setting
 * @param setting The setting to fetch
 *
 * @return The setting's current value. -1 if setting is invalid
 */
int get_setting(unsigned int setting) {
    if (setting < N_SETTINGS)
        return settings[setting];

    return -1;
}

/**
 * Set the value of a given setting
 * @param setting The setting to modify
 * @param value The value to store
 */
void set_setting(unsigned int setting, int value) {
    if (setting < N_SETTINGS)
        settings[setting] = value;
}