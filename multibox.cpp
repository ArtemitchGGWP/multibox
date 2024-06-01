#include <windows.h>    // Core Windows functions
#include <commctrl.h>   // Common controls library
#include <ctime>        // Time functions
#include <fstream>      // File input/output
#include <sstream>      // String stream
#include <shlobj.h>     // Shell API functions
#include <string>       // String functions

// Forward declarations of functions
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void AddTabs(HWND hwnd);
void DisplayTabContent(HWND hwnd, int tabIndex);
void ShowHelloWorld(HWND hwnd);
void ShowClock(HWND hwnd);
void ShowFileContent(HWND hwnd);
void ShowRainbow(HWND hwnd);
void UpdateClock(HWND hwnd);
void BrowseFolder(HWND hwnd);
void ListFilesInFolder(HWND hwnd, const std::wstring& folderPath);
void ResizeControls(HWND hwnd);

// Global variables
HWND hTab;              // Handle to the tab control
HWND hChildWnd;         // Handle to the child window (content of the tab)
HWND hButton;           // Handle to the "Browse Folder" button
int currentTabIndex = 0; // Index of the currently selected tab

// Main function
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

// Window procedure function
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            // Initialize common controls
            InitCommonControls();
            // Create the tab control
            hTab = CreateWindow(WC_TABCONTROL, nullptr, WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
                                0, 0, 600, 350, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
            AddTabs(hTab);            // Add tabs to the tab control
            DisplayTabContent(hwnd, 0);  // Display the content of the first tab by default
            break;

        case WM_SIZE:
            // Handle window resizing
            ResizeControls(hwnd);
            break;

        case WM_NOTIFY:
            // Handle tab control notifications
            if (((LPNMHDR)lParam)->hwndFrom == hTab && ((LPNMHDR)lParam)->code == TCN_SELCHANGE) {
                int tabIndex = TabCtrl_GetCurSel(hTab);
                DisplayTabContent(hwnd, tabIndex); // Change the tab content when a tab is selected
            }
            break;

        case WM_TIMER:
            // Handle timer events
            if (wParam == 1 && currentTabIndex == 1) {
                UpdateClock(hwnd); // Update the clock if the "Clock" tab is selected
            }
            break;

        case WM_DESTROY:
            // Handle window destruction
            KillTimer(hwnd, 1);       // Kill the timer
            PostQuitMessage(0);       // Post quit message
            break;

        case WM_KEYDOWN:
            // Handle key down events
            if (wParam == VK_TAB) {
                if (GetKeyState(VK_SHIFT) & 0x8000) {  // Shift+Tab for previous tab
                    currentTabIndex = (currentTabIndex - 1 + 4) % 4;
                } else {  // Tab for next tab
                    currentTabIndex = (currentTabIndex + 1) % 4;
                }
                TabCtrl_SetCurSel(hTab, currentTabIndex);
                DisplayTabContent(hwnd, currentTabIndex); // Change the tab content
            }
            break;

        case WM_COMMAND:
            // Handle button click events
            if (LOWORD(wParam) == 1 && currentTabIndex == 2) {
                BrowseFolder(hwnd); // Open folder browser when "Browse Folder" button is clicked
            }
            break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam); // Default window procedure
    }
    return 0;
}

// Add tabs to the tab control
void AddTabs(HWND hwnd) {
    TCITEM tie;
    tie.mask = TCIF_TEXT;

    // Array of tab names
    const wchar_t* tabNames[] = { L"Hello World", L"Clock", L"Read File", L"Rainbow" };

    for (int i = 0; i < 4; ++i) {
        tie.pszText = const_cast<LPWSTR>(tabNames[i]);
        TabCtrl_InsertItem(hwnd, i, &tie); // Insert each tab
    }
}

// Display content of the selected tab
void DisplayTabContent(HWND hwnd, int tabIndex) {
    currentTabIndex = tabIndex; // Update the current tab index
    if (hChildWnd) {
        DestroyWindow(hChildWnd); // Destroy the previous child window
        hChildWnd = nullptr;
    }

    // Destroy the "Browse Folder" button if it exists
    if (hButton) {
        DestroyWindow(hButton);
        hButton = nullptr;
    }

    // Stop the timer if it's running
    KillTimer(hwnd, 1);

    // Show content based on the selected tab index
    switch (tabIndex) {
        case 0:
            ShowHelloWorld(hwnd);
            break;
        case 1:
            ShowClock(hwnd);
            UpdateClock(hwnd);  // Update clock immediately
            SetTimer(hwnd, 1, 1000, nullptr); // Start the timer for updating the clock
            break;
        case 2:
            ShowFileContent(hwnd);
            // Create and show the "Browse Folder" button
            RECT rc;
            GetClientRect(hwnd, &rc);
            hButton = CreateWindow(L"BUTTON", L"Browse Folder", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                   20, rc.bottom - 40, 120, 30, hwnd, (HMENU)1, GetModuleHandle(nullptr), nullptr);
            break;
        case 3:
            ShowRainbow(hwnd);
            break;
    }
}

// Show "Hello, World!" message in the "Hello World" tab
void ShowHelloWorld(HWND hwnd) {
    hChildWnd = CreateWindow(L"STATIC", L"Hello, World!", WS_CHILD | WS_VISIBLE,
                             20, 50, 150, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
}

// Show the clock in the "Clock" tab
void ShowClock(HWND hwnd) {
    hChildWnd = CreateWindow(L"STATIC", L"", WS_CHILD | WS_VISIBLE,
                             20, 50, 150, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
}

// Update the clock in the "Clock" tab
void UpdateClock(HWND hwnd) {
    time_t now = time(nullptr);
    struct tm localTime;
    localtime_s(&localTime, &now);

    wchar_t timeStr[100];
    wcsftime(timeStr, sizeof(timeStr) / sizeof(wchar_t), L"%H:%M:%S", &localTime);
    SetWindowText(hChildWnd, timeStr); // Set the clock text
}

// Show the file content interface in the "Read File" tab
void ShowFileContent(HWND hwnd) {
    // Create list view for displaying files
    hChildWnd = CreateWindow(WC_LISTVIEW, nullptr, WS_CHILD | WS_VISIBLE | LVS_REPORT,
                             0, 50, 600, 280, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);

    LVCOLUMN lvColumn;
    lvColumn.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    // Array of column names and widths
    const wchar_t* columnNames[] = { L"File Name", L"File Size (bytes)" };
    int columnWidths[] = { 300, 100 };

    // Insert columns into the list view
    for (int i = 0; i < 2; ++i) {
        lvColumn.pszText = const_cast<LPWSTR>(columnNames[i]);
        lvColumn.cx = columnWidths[i];
        ListView_InsertColumn(hChildWnd, i, &lvColumn);
    }
}

// Show a rainbow in the "Rainbow" tab
void ShowRainbow(HWND hwnd) {
    static COLORREF rainbowColors[] = { RGB(148, 0, 211), RGB(75, 0, 130), RGB(0, 0, 255), RGB(0, 255, 0), RGB(255, 255, 0), RGB(255, 127, 0), RGB(255, 0, 0) };
    RECT rc;
    GetClientRect(hwnd, &rc);

    HDC hdc = GetDC(hwnd);
    int bandHeight = (rc.bottom - 100) / 7;

    for (int i = 0; i < 7; ++i) {
        HBRUSH hBrush = CreateSolidBrush(rainbowColors[i]);
        RECT band = { 0, 50 + i * bandHeight, rc.right, 50 + (i + 1) * bandHeight };
        FillRect(hdc, &band, hBrush);
        DeleteObject(hBrush);
    }
    ReleaseDC(hwnd, hdc);
}

// Open folder browser dialog
void BrowseFolder(HWND hwnd) {
    BROWSEINFO bi = { 0 };
    bi.lpszTitle = L"Select a Folder";
    LPITEMIDLIST pidl = SHBrowseForFolder(&bi); // Open folder browser dialog
    if (pidl != 0) {
        wchar_t path[MAX_PATH];
        if (SHGetPathFromIDList(pidl, path)) {
            ListFilesInFolder(hwnd, path); // List files in the selected folder
        }
        CoTaskMemFree(pidl); // Free memory
    }
}

// List files in the specified folder
void ListFilesInFolder(HWND hwnd, const std::wstring& folderPath) {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile((folderPath + L"\\*").c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        MessageBox(hwnd, L"Invalid folder path.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    ListView_DeleteAllItems(hChildWnd); // Clear existing items in the list view

    do {
        if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            // Insert file name and size into the list view
            LVITEM lvItem;
            lvItem.mask = LVIF_TEXT;
            lvItem.iItem = ListView_GetItemCount(hChildWnd);
            lvItem.iSubItem = 0;
            lvItem.pszText = findFileData.cFileName;
            ListView_InsertItem(hChildWnd, &lvItem);

            std::wstringstream wss;
            wss << findFileData.nFileSizeLow;
            lvItem.iSubItem = 1;
            lvItem.pszText = const_cast<LPWSTR>(wss.str().c_str());
            ListView_SetItem(hChildWnd, &lvItem);
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind); // Close the find handle
}

// Resize controls based on the new size of the window
void ResizeControls(HWND hwnd) {
    RECT rc;
    GetClientRect(hwnd, &rc);

    // Resize the tab control
    if (hTab) {
        SetWindowPos(hTab, nullptr, 0, 0, rc.right, rc.bottom - 40, SWP_NOZORDER);
    }

    // Resize the child window (tab content)
    if (hChildWnd) {
        SetWindowPos(hChildWnd, nullptr, 0, 50, rc.right, rc.bottom - 100, SWP_NOZORDER);
    }

    // Resize the "Browse Folder" button
    if (hButton) {
        SetWindowPos(hButton, nullptr, 20, rc.bottom - 40, 120, 30, SWP_NOZORDER);
    }

    // Invalidate the entire window to force a redraw
    InvalidateRect(hwnd, nullptr, TRUE);
    UpdateWindow(hwnd);
}
