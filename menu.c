#include <stdio.h>
#include <windows.h>
#include <shellapi.h>

/* Window settings */
#define WINDOW_TITLE "Ub3rParse"
#define WINDOW_CALLBACK "Ub3rParseMessage"
#define ICON_ID 100
#define MAX_EQUATIONS 5

/* Menu commands */
#define CMD_EXIT 1000
#define CMD_ABOUT 1001
#define CMD_CPX 1010

/* Ugly globals */
NOTIFYICONDATA nid;
const wchar_t* stored_equations[MAX_EQUATIONS];

/* System specific stuff */
HWND initWindow();
LRESULT CALLBACK wndCallback(HWND, UINT, WPARAM, LPARAM);
void displayMenu(HWND);
void addToClipboard(int);

int main() {
    MSG msg;

    /* Create window */
    initWindow();
    
    /* Main loop */
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

HWND initWindow() {
    int i;
    HWND hWnd;
    HICON hIcon;
    WNDCLASSEX hClass;
    ATOM hClassAtom;
    HINSTANCE hInstance;

    /* Init equation stores */
    for (i=0; i<MAX_EQUATIONS; ++i)
        stored_equations[i] = L"";

    /* Get module instance */
    hInstance = GetModuleHandle(NULL);
    if (!hInstance) {
        printf("Unable to get handle to module!\n");
        exit(EXIT_FAILURE);
    }

    /* Load icon */
    printf("Load icon...\n");
    hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ICON_ID)); 
    if (!hIcon) {
        exit(EXIT_FAILURE);
    }

    /* Register class */
    printf("Class...\n");
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
    hClassAtom = RegisterClassEx(&hClass);

    /* Create the window */
    printf("Create window...\n");
    hWnd = CreateWindowEx(0, WINDOW_CALLBACK, WINDOW_TITLE, 0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);
    if (GetLastError() != 0) {
        printf("Unable to create window.\n");
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
    int targetEngry;
    const UINT wndMsg = RegisterWindowMessage(WINDOW_CALLBACK);

    if (message == WM_COMMAND && LOWORD(wParam) == CMD_EXIT) {
        Shell_NotifyIcon(NIM_DELETE, &nid);
        PostQuitMessage(0);
        return 0;
    }

    if (message == WM_COMMAND && LOWORD(wParam) == CMD_ABOUT) {
    }

    if (message == WM_COMMAND && LOWORD(wParam) >= CMD_CPX) {
        addToClipboard(LOWORD(wParam) - CMD_CPX);
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

void addToClipboard(int targetIndex) {
    const wchar_t *equation = stored_equations[targetIndex];
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, wcslen(equation)+1);
    memcpy(GlobalLock(hMem), equation, wcslen(equation)+1);

    GlobalUnlock(hMem);
    OpenClipboard(0);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();
}