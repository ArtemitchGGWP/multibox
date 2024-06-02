#include "multibox.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"MultiboxClass"; // Window class name

    // Window class definition
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;      // Window procedure function
    wc.hInstance = hInstance;         // Instance handle
    wc.lpszClassName = CLASS_NAME;    // Class name

    RegisterClass(&wc);               // Register the window class

    // Create the main application window
    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, L"Multibox", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
        nullptr, nullptr, hInstance, nullptr);

    if (hwnd == nullptr) {
        return 0; // Return 0 if window creation failed
    }

    ShowWindow(hwnd, nCmdShow); // Show the window

    // Message loop
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0; // Return after the message loop ends
}
