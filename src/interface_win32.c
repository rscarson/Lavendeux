#ifdef _WIN32

    #include <stdio.h>
    #include <windows.h>
    #include <shellapi.h>
    #include <conio.h>
    #include <Shlobj.h>

    #include "interface_win32.h"
    #include "interface.h"
    #include "language.h"

    /* Event globals */
    NOTIFYICONDATA nid;

    /* Stored callbacks */
    parseCallback _parse_callback;
    exitCallback _exit_callback;

    /* Menu equations */
    wchar_t *stored_entries[MAX_EQUATIONS];

    /* Default options */
    int silent_mode;
    int angle_mode;
    int lang_mode;

    /** 
     * Prepare and draw the interface 
     * @param exit_callback Method to be called in event of a quit
     * @param parse_callback Method to be called in event of the hotkeys being pressed
     */
    void init_interface(exitCallback exit_callback, parseCallback parse_callback) {
        int i;
        HWND hWnd;
        HICON hIcon;
        WNDCLASSEX hClass;
        HINSTANCE hInstance;

        /* Callback methods */
        _parse_callback = parse_callback;
        _exit_callback = exit_callback;

        /* Init equation stores */
        for (i=0; i<MAX_EQUATIONS; ++i)
            stored_entries[i] = NULL;

        /* Default options */
        silent_mode = SETTING_SILENT_ON;
        angle_mode = SETTING_ANGLE_DEG;
        lang_mode = LANG_EN;

        /* Get module instance */
        hInstance = GetModuleHandle(NULL);
        if (!hInstance)
            error_msg(L"Error while starting", L"Cannot get handle to module", 1);

        /* Load icon */
        hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ICON_ID)); 
        if (!hIcon) {
            exit(EXIT_FAILURE);
        }

        /* Register class */
        hClass.cbSize         = sizeof(hClass);
        hClass.style          = CS_HREDRAW | CS_VREDRAW;
        hClass.lpfnWndProc    = wnd_callback;
        hClass.cbClsExtra     = 0;
        hClass.cbWndExtra     = 0;
        hClass.hInstance      = hInstance;
        hClass.hIcon          = hIcon;
        hClass.hCursor        = LoadCursor(NULL, IDC_ARROW);
        hClass.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
        hClass.lpszMenuName   = 0;
        hClass.lpszClassName  = WINDOW_CALLBACK;
        hClass.hIconSm        = hIcon;
        RegisterClassEx(&hClass);

        /* Create the window */
        hWnd = CreateWindowEx(0, WINDOW_CALLBACK, WINDOW_TITLE, 0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);
        if (GetLastError() != 0)
            error_msg(L"Error while starting", L"Cannot get handle to window", 1);

        /* Register hotkey */
        if (!RegisterHotKey(hWnd, HOTKEY_ID, MOD_CONTROL, VK_SPACE))
            error_msg(L"Error while starting", L"Cannot register hotkey!", 1);

        /* Start window */
        ShowWindow(hWnd, 0);
        UpdateWindow(hWnd);

        /* Populate notification data */
        nid.cbSize = sizeof(nid);
        nid.hWnd = hWnd;
        nid.uID = 100;
        nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
        nid.uCallbackMessage = RegisterWindowMessage(WINDOW_CALLBACK);
        nid.hIcon = hIcon;
        strcpy(nid.szTip, WINDOW_TITLE);

        /* Add notification icon to tray */
        Shell_NotifyIcon(NIM_ADD, &nid);
    }

    /** 
     * Check window messages, process events
     */
    void update_interface( void ) {
        MSG msg;
        
        if (GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    /** 
     * Windows event callbacks
     */
    LRESULT CALLBACK wnd_callback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        static UINT wndMsg = 0;

        char about_title[0xFF];
        char about_msg[0xFF];

        /* Register for callbacks, but only once */
        if (wndMsg == 0)
            wndMsg = RegisterWindowMessage(WINDOW_CALLBACK);

        /* What type of event is it */
        switch (message) {

            /* New thing to process */
            case WM_HOTKEY:
                _parse_callback(from_clipboard());
            break;

            /* Time to exit */
            case WM_DESTROY:
                Shell_NotifyIcon(NIM_DELETE, &nid);
                PostQuitMessage(0);
                _exit_callback();
            break;

            /* Some other command type */
            case WM_COMMAND:

                /* Which type of custom command */
                switch (LOWORD(wParam)) {

                    /* Exit through menu */
                    case CMD_EXIT:
                        Shell_NotifyIcon(NIM_DELETE, &nid);
                        PostQuitMessage(0);
                        _exit_callback();
                    break;
                    
                    /* Show about box */
                    case CMD_ABOUT:
                        sprintf(about_title, ABOUT_TITLE, WINDOW_TITLE);
                        sprintf(about_msg, ABOUT_MSG, WINDOW_TITLE, MAJOR_VERSION, MINOR_VERSION, RELEASE_NUMBER);

                        MessageBox(hWnd, 
                            about_msg, 
                            about_title,
                        MB_OK);
                    break;

                    /* Change to degrees mode */
                    case CMD_ANGLE_DEG:
                        angle_mode = SETTING_ANGLE_DEG;
                    break;

                    /* Change to radians mode */
                    case CMD_ANGLE_RAD:
                        angle_mode = SETTING_ANGLE_RAD;
                    break;

                    /* Turn silent errors on and off mode */
                    case CMD_TOGGLE_SILENT_MODE:
                        silent_mode = (silent_mode==SETTING_SILENT_ON) ? SETTING_SILENT_OFF : SETTING_SILENT_ON;
                    break;

                    /* English language */
                    case CMD_LANG_EN:
                        lang_mode = LANG_EN;
                    break;

                    /* French language */
                    case CMD_LANG_FR:
                        lang_mode = LANG_FR;
                    break;

                    /* One of the equations was clicked. Put in it the clipboard */
                    default:
                        if (LOWORD(wParam) < CMD_CPX) break;
                        to_clipboard(stored_entries[LOWORD(wParam) - CMD_CPX]);
                }
            break;

            /* Show the menu */
            default:
                if (message == wndMsg && lParam == WM_RBUTTONUP)
                    show_menu(hWnd);
        }

        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    /* Show the menu interface */
    void show_menu(HWND hWnd)
    {
        wchar_t buffer[MAX_HISTORY_LEN + wcslen(HISTORY_SUFFIX) + 1];
        int has_equation = 0;
        int i;
        POINT p;
        HMENU hMenu;
        HMENU hAnglesMenu;
        HMENU hLangMenu;

        /* Get position of cursor */
        GetCursorPos(&p);

        /* Create the empty menud */
        hMenu = CreatePopupMenu();
        hAnglesMenu = CreatePopupMenu();
        hLangMenu = CreatePopupMenu();

        /* Equations */
        for (i=0; i<MAX_EQUATIONS; ++i)
            if (stored_entries[i] != NULL) {
                wcsncpy(buffer, stored_entries[i], MAX_HISTORY_LEN);
                buffer[MAX_HISTORY_LEN] = L'\0';
                if (wcslen(stored_entries[i]) > MAX_HISTORY_LEN)
                    wcscat(buffer, HISTORY_SUFFIX);
                AppendMenuW(hMenu, MF_STRING, CMD_CPX+i, buffer);
                has_equation = 1;
            }
        if (!has_equation) {
            AppendMenuW(hMenu, MF_STRING, 0, lang_lookup[LANG_STR_NO_EQUATIONS][lang_mode]);
        }

        /* Settings */
        AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
        AppendMenuW(hLangMenu, (lang_mode==LANG_EN)?MF_CHECKED:MF_UNCHECKED, CMD_LANG_EN, L"English");
        AppendMenuW(hLangMenu, (lang_mode==LANG_FR)?MF_CHECKED:MF_UNCHECKED, CMD_LANG_FR, L"Français");
        AppendMenuW(hMenu, MF_POPUP, (UINT) hLangMenu, lang_lookup[LANG_STR_LANGUAGE][lang_mode]);

        AppendMenuW(hMenu, (silent_mode==SETTING_SILENT_ON)?MF_CHECKED:MF_UNCHECKED, CMD_TOGGLE_SILENT_MODE, lang_lookup[LANG_STR_SILENT_ERRS][lang_mode]);

        AppendMenuW(hAnglesMenu, (angle_mode==SETTING_ANGLE_DEG)?MF_CHECKED:MF_UNCHECKED, CMD_ANGLE_DEG, lang_lookup[LANG_STR_DEGREES][lang_mode]);
        AppendMenuW(hAnglesMenu, (angle_mode==SETTING_ANGLE_RAD)?MF_CHECKED:MF_UNCHECKED, CMD_ANGLE_RAD, lang_lookup[LANG_STR_RADIANS][lang_mode]);
        AppendMenuW(hMenu, MF_POPUP, (UINT) hAnglesMenu, lang_lookup[LANG_STR_ANGLE_UNITS][lang_mode]);

        /* System menu */
        AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
        AppendMenuW(hMenu, MF_STRING, CMD_ABOUT, lang_lookup[LANG_STR_ABOUT][lang_mode]);
        AppendMenuW(hMenu, MF_STRING, CMD_EXIT, lang_lookup[LANG_STR_EXIT][lang_mode]);

        SetForegroundWindow(hWnd); // Win32 bug work-around
        TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, p.x, p.y, 0, hWnd, NULL);

    }

    /** 
     * Add an entry to the menu history
     * @param entry The string to add
     */
    void add_history(const wchar_t *entry) {
        int i;

        /* Free memory for last entry */
        if (stored_entries[MAX_EQUATIONS-1] != NULL)
            free(stored_entries[MAX_EQUATIONS-1]);

        /* Shift all current entries by one */
        for (i=MAX_EQUATIONS-2; i>=0; --i)
            stored_entries[i+1] = stored_entries[i];

        /* Allocate new entry */
        stored_entries[0] = (wchar_t*) malloc(sizeof(wchar_t)*(wcslen(entry)+1));
        if (stored_entries[0] == NULL)
            error_msg(lang_lookup[LANG_STR_RUNTIME_ERR][lang_mode], L"Failed to allocate memory!", 1);
        wcscpy( stored_entries[0], entry);
    }

    /** 
     * Copy string to system clipboard
     * @param entry The string to add
     */
    void to_clipboard(const wchar_t *entry) {
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, sizeof(wchar_t)*(wcslen(entry)+1));
        memcpy(GlobalLock(hMem), entry, sizeof(wchar_t)*(wcslen(entry)+1));

        GlobalUnlock(hMem);
        if (OpenClipboard(0)) {
            EmptyClipboard();
            SetClipboardData(CF_UNICODETEXT, hMem);
            CloseClipboard();
        }
    }

    /** 
     * Get string from system clipboard
     * @return const char* The string fetched
     */
    const wchar_t* from_clipboard( void ) {
        HGLOBAL hMem;
        const wchar_t* clipText = NULL;

        if (OpenClipboard(0)) {
            hMem = GetClipboardData(CF_UNICODETEXT);
            if (hMem != NULL) {
                clipText = GlobalLock(hMem);
                GlobalUnlock(hMem);
            }

            CloseClipboard();
        }

        return clipText;
    }

    /**
     * Get the path to a valid configuration file
     * Search in current directory, then relevant home dir
     */
    const wchar_t* config_path( void ) {
        wchar_t *path = (wchar_t*) malloc(sizeof(wchar_t)*(MAX_PATH+1));
        if (path == NULL)
            error_msg(lang_lookup[LANG_STR_RUNTIME_ERR][lang_mode], L"Failed to allocate memory!", 1);
        if (SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, path) == S_OK) {
            wcscat(path, CONFIG_FILENAME);
            return path;
        }
        return NULL;
     }

    /**
     * Get the path to a valid configuration file
     * @param title The title of the error message
     * @param msg The error message
     * @param fatal if non 0, exit
     */
    void error_msg(const wchar_t* title, const wchar_t* msg, char fatal) {
        if (silent_mode == SETTING_SILENT_OFF || fatal) {
            MessageBoxW(NULL, 
                msg,
                title,
            MB_OK);
        }

        if (fatal)
            exit(EXIT_FAILURE);
    }

    /**
     * Get the value of a given setting
     * @param setting The setting to fetch
     *
     * @return The setting's current value. -1 if setting is invalid
     */
    int get_setting(int setting) {
        switch (setting) {
            case SETTING_ANGLE:
                return angle_mode;
            case SETTING_SILENT:
                return silent_mode;
            case SETTING_LANG:
                return lang_mode;
        }

        return -1;
    }

#endif