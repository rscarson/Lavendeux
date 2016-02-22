#ifdef _WIN32
#define NTDDI_VERSION NTDDI_WIN2K
#define _WIN32_IE 0x0500

    #include <stdio.h>
    #include <windows.h>
    #include <winable.h>
    #include <shellapi.h>
    #include <conio.h>
    #include <Shlobj.h>

    #include "language.h"
    #include "lavendeux.h"
    #include "interface_win32.h"
    #include "interface.h"

    /* Event globals */
    HWND hWnd;
    NOTIFYICONDATA nid;

    /* Stored callbacks */
    parseCallback _parse_callback;
    exitCallback _exit_callback;

    /* Menu equations */
    wchar_t *stored_entries[MAX_EQUATIONS];

    /* Options */
    int settings[N_SETTINGS];

    /** 
     * Prepare and draw the interface 
     * @param exit_callback Method to be called in event of a quit
     * @param parse_callback Method to be called in event of the hotkeys being pressed
     */
    void init_interface(exitCallback exit_callback, parseCallback parse_callback) {
        int i;
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
        for (i=0; i<N_SETTINGS; ++i)
            settings[i] = 0;

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
        hWnd = CreateWindowEx(0, WINDOW_CALLBACK, APPLICATION_NAME, 0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);
        if (GetLastError() != 0)
            error_msg(L"Error while starting", L"Cannot get handle to window", 1);

        /* Register hotkey */
        if (!RegisterHotKey(hWnd, HOTKEY_ID, MOD_CONTROL, VK_SPACE))
            error_msg(L"Error while starting", L"Cannot register hotkey. Is Lavendeux already running?", 1);

        /* Start window */
        ShowWindow(hWnd, 0);
        UpdateWindow(hWnd);

        /* Populate notification data */
        nid.cbSize = sizeof(nid);
        nid.hWnd = hWnd;
        nid.uID = 100;
        nid.uFlags = NIF_INFO | NIF_MESSAGE | NIF_ICON | NIF_TIP;
        nid.uCallbackMessage = RegisterWindowMessage(WINDOW_CALLBACK);
        nid.hIcon = hIcon;
        nid.dwInfoFlags = NIIF_INFO;
        strcpy(nid.szTip, APPLICATION_NAME);
        strcpy(nid.szInfoTitle, RUNNING_TITLE);
        strcpy(nid.szInfo, RUNNING_MSG);

        /* Add notification icon to tray */
        Shell_NotifyIcon(NIM_ADD, &nid);
    }

    /**
     * Enable debug console
     */
    void debug_enable( void ) {
        AllocConsole();
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
        printf("Debug mode enabled.\n");
    }
    
    /** 
     * Print help message to stdout
     */
    void print_help( void ) {
        PostMessage(hWnd, WM_COMMAND, CMD_ABOUT, 0);
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
        static INPUT control_c[] = {
            {INPUT_KEYBOARD, .ki={VK_CONTROL,0,KEYEVENTF_KEYUP,0,0}},
            {INPUT_KEYBOARD, .ki={VK_CONTROL,0,0,0,0}},
            {INPUT_KEYBOARD, .ki={0x43,0,0,0,0}},
            {INPUT_KEYBOARD, .ki={VK_CONTROL,0,KEYEVENTF_KEYUP,0,0}},
            {INPUT_KEYBOARD, .ki={0x43,0,KEYEVENTF_KEYUP,0,0}},
        };
        static INPUT control_v[] = {
            {INPUT_KEYBOARD, .ki={VK_CONTROL,0,KEYEVENTF_KEYUP,0,0}},
            {INPUT_KEYBOARD, .ki={VK_CONTROL,0,0,0,0}},
            {INPUT_KEYBOARD, .ki={0x56,0,0,0,0}},
            {INPUT_KEYBOARD, .ki={0x56,0,KEYEVENTF_KEYUP,0,0}},
        };
        int i;

        /* Register for callbacks, but only once */
        if (wndMsg == 0)
            wndMsg = RegisterWindowMessage(WINDOW_CALLBACK);

        /* What type of event is it */
        switch (message) {

            /* New thing to process */
            case WM_HOTKEY:
                if (settings[SETTING_AUTOCOPY]==SETTING_AUTOCOPY_ON) {
                    /* Control C */
                    SendInput(sizeof(control_c)/sizeof(INPUT), control_c, sizeof(INPUT));
                    Sleep(50);
                }

                _parse_callback(from_clipboard());

                if (settings[SETTING_AUTOCOPY]==SETTING_AUTOCOPY_ON) {
                    /* Control V */
                    SendInput(sizeof(control_v)/sizeof(INPUT), control_v, sizeof(INPUT));
                }
            break;

            /* Time to exit */
            case WM_DESTROY:
                Shell_NotifyIcon(NIM_DELETE, &nid);

                /* Delete stored entries */
                for (i=0; i<MAX_EQUATIONS; i++)
                    if (stored_entries[i] != NULL)
                        free(stored_entries[i]);

                PostQuitMessage(0);
                _exit_callback();
            break;

            case WM_HELP:
                ShellExecuteA(
                    hWnd, 
                    "open", 
                    HELP_URL, 
                    NULL, 
                    NULL, 
                    SW_SHOWDEFAULT
                );
            break;

            /* Some other command type */
            case WM_COMMAND:

                /* Which type of custom command */
                switch (LOWORD(wParam)) {

                    /* Exit through menu */
                    case CMD_EXIT:
                        Shell_NotifyIcon(NIM_DELETE, &nid);

                    /* Delete stored entries */
                    for (i=0; i<MAX_EQUATIONS; i++)
                        if (stored_entries[i] != NULL)
                            free(stored_entries[i]);

                        PostQuitMessage(0);
                        _exit_callback();
                    break;
                    
                    /* Show about box */
                    case CMD_ABOUT:
                        SetForegroundWindow(hWnd);
                        MessageBox(hWnd, 
                            HELP_MSG, HELP_TITLE,
                            MB_OK | MB_HELP | MB_DEFBUTTON1);
                    break;

                    /* Change to degrees mode */
                    case CMD_ANGLE_DEG:
                        settings[SETTING_ANGLE] = SETTING_ANGLE_DEG;
                    break;

                    /* Change to radians mode */
                    case CMD_ANGLE_RAD:
                        settings[SETTING_ANGLE] = SETTING_ANGLE_RAD;
                    break;

                    /* Turn silent errors on and off mode */
                    case CMD_TOGGLE_SILENT_MODE:
                        settings[SETTING_SILENT] = (settings[SETTING_SILENT]==SETTING_SILENT_ON) ? SETTING_SILENT_OFF : SETTING_SILENT_ON;
                    break;

                    /* Turn autocopy on and off */
                    case CMD_TOGGLE_AUTOCOPY:
                        settings[SETTING_AUTOCOPY] = (settings[SETTING_AUTOCOPY]==SETTING_AUTOCOPY_ON) ? SETTING_AUTOCOPY_OFF : SETTING_AUTOCOPY_ON;
                    break;

                    /* English language */
                    case CMD_LANG_EN:
                        settings[SETTING_LANG] = LANG_EN;
                        language_set_current(LANG_EN);
                    break;

                    /* French language */
                    case CMD_LANG_FR:
                        settings[SETTING_LANG] = LANG_FR;
                        language_set_current(LANG_FR);
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
            AppendMenuW(hMenu, MF_STRING, 0, language_str(LANG_STR_NO_EQUATIONS));
        }

        /* Settings */
        AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
        AppendMenuW(hLangMenu, (settings[SETTING_LANG]==LANG_EN)?MF_CHECKED:MF_UNCHECKED, CMD_LANG_EN, L"English");
        AppendMenuW(hLangMenu, (settings[SETTING_LANG]==LANG_FR)?MF_CHECKED:MF_UNCHECKED, CMD_LANG_FR, L"Français");
        AppendMenuW(hMenu, MF_POPUP, (UINT) hLangMenu, language_str(LANG_STR_LANGUAGE));

        AppendMenuW(hMenu, (settings[SETTING_SILENT]==SETTING_SILENT_ON)?MF_CHECKED:MF_UNCHECKED, CMD_TOGGLE_SILENT_MODE, language_str(LANG_STR_SILENT_ERRS));
        AppendMenuW(hMenu, (settings[SETTING_AUTOCOPY]==SETTING_AUTOCOPY_ON)?MF_CHECKED:MF_UNCHECKED, CMD_TOGGLE_AUTOCOPY, language_str(LANG_STR_ENABLEAUTOCOPY));

        AppendMenuW(hAnglesMenu, (settings[SETTING_ANGLE]==SETTING_ANGLE_DEG)?MF_CHECKED:MF_UNCHECKED, CMD_ANGLE_DEG, language_str(LANG_STR_DEGREES));
        AppendMenuW(hAnglesMenu, (settings[SETTING_ANGLE]==SETTING_ANGLE_RAD)?MF_CHECKED:MF_UNCHECKED, CMD_ANGLE_RAD, language_str(LANG_STR_RADIANS));
        AppendMenuW(hMenu, MF_POPUP, (UINT) hAnglesMenu, language_str(LANG_STR_ANGLE_UNITS));
        

        /* System menu */
        AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
        AppendMenuW(hMenu, MF_STRING, CMD_ABOUT, language_str(LANG_STR_ABOUT));
        AppendMenuW(hMenu, MF_STRING, CMD_EXIT, language_str(LANG_STR_EXIT));

        SetForegroundWindow(hWnd);
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
            error_msg(language_str(LANG_STR_RUNTIME_ERR), language_str(LANG_STR_ERR_ALLOCATION), 1);
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

        printf("Saved a string to clipboard: %S\n", entry);
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

        printf("Read a string from clipboard: %S\n", clipText);
        return clipText;
    }

    /**
     * Get the path to a valid configuration file
     * Search in current directory, then relevant home dir
     */
    const char* config_path( void ) {
        char *path;
        FILE* test;

        /* Test current directory */
        test = fopen(CONFIG_FILENAME, "r");
        if (test != NULL) {
            fclose(test);

            printf("Found configuration path: %s\n", CONFIG_FILENAME);
            return CONFIG_FILENAME;
        }

        /* Prepare to hold path */
        path = (char*) malloc(sizeof(char)*(MAX_PATH+1));
        if (path == NULL)
            error_msg(language_str(LANG_STR_RUNTIME_ERR), language_str(LANG_STR_ERR_ALLOCATION), 1);

        /* Get home dir path */
        if (SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, path) == S_OK) {
            strcat(path, CONFIG_FILENAME);

            printf("Found configuration path: %s\n", path);
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
        if (settings[SETTING_SILENT] == SETTING_SILENT_OFF || fatal) {
            SetForegroundWindow(hWnd);
            MessageBoxW(hWnd, 
                msg,
                title,
            MB_OK | MB_ICONWARNING | MB_DEFBUTTON1);

            printf("Displaying an error message: %S\n", msg);
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

#endif