#ifndef MENU_H
	#define MENU_H

    /* Window settings */
    #define WINDOW_TITLE "Ub3rParse"
    #define MAX_EQUATIONS 5

	/* Application settings */
	#define MAJOR_VERSION 2
	#define MINOR_VERSION 0
	#define RELEASE_NUMBER 0

	/* About settings */
	#define ABOUT_TITLE "About %s"
	#define ABOUT_MSG "%s Version %d.%d.%d \nShift-Space to solve equation(s) in clipboard.\n\nCopyright Richard Carson, 2016."

	void initMenu(void);
	void addEquation(const wchar_t*);
	void toClipboard(const wchar_t*);
	int updateMenu(void);
#endif