#include "multibox.h"

// Global variables
HWND hTab;              // Handle to the tab control
HWND hChildWnd;         // Handle to the child window (content of the tab)
HWND hButton;           // Handle to the "Browse Folder" button
int currentTabIndex = 0; // Index of the currently selected tab

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
                    currentTabIndex = (currentTabIndex - 1 + 3) % 3;
                } else {  // Tab for next tab
                    currentTabIndex = (currentTabIndex + 1) % 3;
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

void AddTabs(HWND hwnd) {
    TCITEM tie;
    tie.mask = TCIF_TEXT;

    // Array of tab names
    const wchar_t* tabNames[] = { L"Hello World", L"Clock", L"Read File" };

    for (int i = 0; i < 3; ++i) {
        tie.pszText = const_cast<LPWSTR>(tabNames[i]);
        TabCtrl_InsertItem(hwnd, i, &tie); // Insert each tab
    }
}

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
    }
}

void ShowHelloWorld(HWND hwnd) {
    hChildWnd = CreateWindow(L"STATIC", L"Hello, World!", WS_CHILD | WS_VISIBLE,
                             20, 50, 150, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
}

void ShowClock(HWND hwnd) {
    hChildWnd = CreateWindow(L"STATIC", L"", WS_CHILD | WS_VISIBLE,
                             20, 50, 150, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
}

void UpdateClock(HWND hwnd) {
    time_t now = time(nullptr);
    struct tm localTime;
    localtime_s(&localTime, &now);

    wchar_t timeStr[100];
    wcsftime(timeStr, sizeof(timeStr) / sizeof(wchar_t), L"%H:%M:%S", &localTime);
    SetWindowText(hChildWnd, timeStr); // Set the clock text
}

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
