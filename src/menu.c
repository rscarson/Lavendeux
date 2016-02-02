#ifdef _WIN32
    #include <stdio.h>
    #include <windows.h>
    #include <shellapi.h>
    #include <conio.h>

    #include "menu.h"

    /* Window settings */
    #define WINDOW_CALLBACK "Ub3rParseMessage"
    #define ICON_ID 100

    /* Menu commands */
    #define CMD_EXIT 1000
    #define CMD_ABOUT 1001
    #define CMD_CPX 1010

    /* Ugly globals */
    NOTIFYICONDATA nid;
    int current_index;
    const wchar_t* stored_equations[MAX_EQUATIONS];

    /* System specific stuff */
    LRESULT CALLBACK wndCallback(HWND, UINT, WPARAM, LPARAM);
    void displayMenu(HWND);

    int updateMenu() {
        MSG msg;
        char c;

        if(_kbhit()) {
            printf("Got a %d\n", _getch());
        }
        
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
        for (i=0; i<MAX_EQUATIONS; ++i)
            addEquation(L"");

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
        int equationIndex;
        char about_title[255];
        char about_msg[255];

        const UINT wndMsg = RegisterWindowMessage(WINDOW_CALLBACK);

        if (message == WM_COMMAND && LOWORD(wParam) == CMD_EXIT) {
            Shell_NotifyIcon(NIM_DELETE, &nid);
            PostQuitMessage(0);
            return 0;
        }

        if (message == WM_COMMAND && LOWORD(wParam) == CMD_ABOUT) {
            sprintf(about_title, ABOUT_TITLE, WINDOW_TITLE);
            sprintf(about_msg, ABOUT_MSG, WINDOW_TITLE, MAJOR_VERSION, MINOR_VERSION, RELEASE_NUMBER);

            MessageBox(hWnd, 
                about_msg, 
                about_title,
            MB_OK);
        }

        if (message == WM_COMMAND && LOWORD(wParam) >= CMD_CPX) {
            equationIndex = LOWORD(wParam) - CMD_CPX;
            toClipboard(stored_equations[equationIndex]);
        }
        
        if (message == WM_DESTROY) {
            Shell_NotifyIcon(NIM_DELETE, &nid);
            PostQuitMessage(0);
            return 0;
        }

        if (message == wndMsg && lParam == WM_RBUTTONUP) {
            displayMenu(hWnd);
        }

        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    void displayMenu(HWND hWnd)
    {
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
            if (wcslen(stored_equations[i]) > 0) {
                AppendMenuW(hSubMenu, MF_STRING, CMD_CPX+i, stored_equations[i]);
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

    void addEquation(const wchar_t* equation) {
        stored_equations[current_index] = equation;
        current_index = (current_index+1) % MAX_EQUATIONS;
    }

    void toClipboard(const wchar_t *equation) {
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, wcslen(equation)+1);
        memcpy(GlobalLock(hMem), equation, wcslen(equation)+1);

        GlobalUnlock(hMem);
        OpenClipboard(0);
        EmptyClipboard();
        SetClipboardData(CF_TEXT, hMem);
        CloseClipboard();
    }
#endif