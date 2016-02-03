#ifndef INTERFACE_WIN32_H
	#define INTERFACE_WIN32_H

    /* Window settings */
    #define WINDOW_CALLBACK "Ub3rParseMessage"
    #define HOTKEY_ID 100
    #define ICON_ID 100

    /* Menu commands */
    #define CMD_EXIT                1000
    #define CMD_ABOUT               1001
    #define CMD_ANGLE_DEG           1002
    #define CMD_ANGLE_RAD           1003
    #define CMD_TOGGLE_SILENT_MODE  1004
    #define CMD_CPX                 1010

    LRESULT CALLBACK wnd_callback(HWND, UINT, WPARAM, LPARAM);
    void show_menu(HWND);
#endif