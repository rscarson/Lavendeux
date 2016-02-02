#ifdef _WIN32
    #include <stdio.h>
    #include <windows.h>
    #include <shellapi.h>
    #include <conio.h>

    #include "menu.h"

    /* Window settings */
    #define WINDOW_CALLBACK "Ub3rParseMessage"
    #define HOTKEY_ID 100
    #define ICON_ID 100

    /* Menu commands */
    #define CMD_EXIT 1000
    #define CMD_ABOUT 1001
    #define CMD_CPX 1010

    /* Ugly globals */
    NOTIFYICONDATA nid;
    int current_index;
    char last_equation[MAX_LEN];
    char stored_equations[MAX_EQUATIONS][MAX_LEN];

    /* System specific stuff */
    LRESULT CALLBACK wndCallback(HWND, UINT, WPARAM, LPARAM);
    void displayMenu(HWND);

    int updateMenu() {
        MSG msg;
        
        /* Main loop */
        if (GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            return 1;
        }

        return 0;
    }

    void initMenu() {
        int i;
        HWND hWnd;
        HICON hIcon;
        WNDCLASSEX hClass;
        HINSTANCE hInstance;

        /* Init equation stores */
        current_index = 0;
        last_equation[0] = 0;
        for (i=0; i<MAX_EQUATIONS; ++i)
            addEquation("");

        /* Get module instance */
        hInstance = GetModuleHandle(NULL);
        if (!hInstance) {
            MessageBox(hWnd, 
                "Cannot get handle to module", 
                "Error while starting",
            MB_OK);
            exit(EXIT_FAILURE);
        }

        /* Load icon */
        hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ICON_ID)); 
        if (!hIcon) {
            exit(EXIT_FAILURE);
        }

        /* Register class */
        hClass.cbSize         = sizeof(hClass);
        hClass.style          = CS_HREDRAW | CS_VREDRAW;
        hClass.lpfnWndProc    = wndCallback;
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
        if (GetLastError() != 0) {
            MessageBox(hWnd, 
                "Cannot get handle to window", 
                "Error while starting",
            MB_OK);
            exit(EXIT_FAILURE);
        }

        /* Register hotkey */
        if (!RegisterHotKey(hWnd, HOTKEY_ID, MOD_CONTROL, VK_SPACE)) {
            MessageBox(hWnd, 
                "Cannot register hotkey!", 
                "Error while starting",
            MB_OK);
            exit(EXIT_FAILURE);
        }

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

    /* Callback function for the window */
    LRESULT CALLBACK wndCallback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        static UINT wndMsg = 0;
        if (wndMsg == 0)
            wndMsg = RegisterWindowMessage(WINDOW_CALLBACK);

        const char* equation;
        char about_title[255];
        char about_msg[255];

        switch (message) {
            case WM_HOTKEY:
                strcpy(last_equation, fromClipboard());
            break;

            case WM_DESTROY:
                Shell_NotifyIcon(NIM_DELETE, &nid);
                PostQuitMessage(0);
            break;

            case WM_COMMAND:
                switch (LOWORD(wParam)) {
                    case CMD_EXIT:
                        Shell_NotifyIcon(NIM_DELETE, &nid);
                        PostQuitMessage(0);
                    break;
                    
                    case CMD_ABOUT:
                        sprintf(about_title, ABOUT_TITLE, WINDOW_TITLE);
                        sprintf(about_msg, ABOUT_MSG, WINDOW_TITLE, MAJOR_VERSION, MINOR_VERSION, RELEASE_NUMBER);

                        MessageBox(hWnd, 
                            about_msg, 
                            about_title,
                        MB_OK);
                    break;

                    default:
                        if (LOWORD(wParam) < CMD_CPX) break;
                        toClipboard(stored_equations[LOWORD(wParam) - CMD_CPX]);
                }
            break;

            default:
                if (message == wndMsg && lParam == WM_RBUTTONUP)
                    displayMenu(hWnd);
        }

        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    void displayMenu(HWND hWnd)
    {
        wchar_t wideString[MAX_LEN];
        int has_equation = 0;
        int i;
        POINT p;
        HMENU hSubMenu = CreatePopupMenu();

        /* Get position of cursor */
        GetCursorPos(&p);

        /* Create the empty menu */
        hSubMenu = CreatePopupMenu();

        /* Equations */
        for (i=0; i<MAX_EQUATIONS; ++i)
            if (strlen(stored_equations[i]) > 0) {
                mbstowcs(wideString, stored_equations[i], strlen(stored_equations[i])+1);
                AppendMenuW(hSubMenu, MF_STRING, CMD_CPX+i, wideString);
                has_equation = 1;
            }
        if (!has_equation) {
            AppendMenuW(hSubMenu, MF_STRING, 0, L"No equations in history");
        }

        /* System menu */
        AppendMenuW(hSubMenu, MF_SEPARATOR, 0, NULL);
        AppendMenuW(hSubMenu, MF_STRING, CMD_ABOUT, L"About Ub3rParse");
        AppendMenuW(hSubMenu, MF_STRING, CMD_EXIT, L"Exit");

        SetForegroundWindow(hWnd); // Win32 bug work-around
        TrackPopupMenu(hSubMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, p.x, p.y, 0, hWnd, NULL);

    }

    void addEquation(const char *equation) {
        printf("Adding %s", equation);
        strcpy(stored_equations[current_index++], equation);
        current_index %= MAX_EQUATIONS;
    }

    void toClipboard(const char *equation) {
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, strlen(equation)+1);
        memcpy(GlobalLock(hMem), equation, strlen(equation)+1);

        GlobalUnlock(hMem);
        if (OpenClipboard(0)) {
            EmptyClipboard();
            SetClipboardData(CF_TEXT, hMem);
            CloseClipboard();
        }
    }

    const char* fromClipboard(void) {
        HGLOBAL hMem;
        const char* clipText = NULL;

        if (OpenClipboard(0)) {
            hMem = GetClipboardData(CF_TEXT);
            if (hMem != NULL) {
                clipText = GlobalLock(hMem);
                GlobalUnlock(hMem);
            }

            CloseClipboard();
        }

        return clipText;
    }

    const char *lastEquation(void) {
        char* value;

        if (strlen(last_equation) == 0)
            return NULL;

        value = (char*) malloc(sizeof(char)*strlen(last_equation)+1);
        strcpy(value, last_equation);

        last_equation[0] = 0;
        return value;
    }
#endif