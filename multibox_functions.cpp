#include "multibox.h"

// Global variables
HWND hTab;              // Handle to the tab control
HWND hChildWnd;         // Handle to the child window (content of the tab)
HWND hButton;           // Handle to the "Browse Folder" button
int currentTabIndex = 0; // Index of the currently selected tab
HFONT hFont = nullptr;  // Handle to the font
COLORREF textColor = RGB(0, 0, 0); // Default text color (black)
HWND hFontSizeSlider = nullptr;   // Handle to the font size slider
HWND hRedSlider = nullptr;        // Handle to the red color slider
HWND hGreenSlider = nullptr;      // Handle to the green color slider
HWND hBlueSlider = nullptr;       // Handle to the blue color slider
HWND hColorPreview = nullptr;     // Handle to the color preview
HWND hRGBValues = nullptr;        // Handle to display RGB values

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
            if (wParam == 1) {
                UpdateClock(hwnd); // Update the clock if the "Clock" tab is selected
            }
            break;

        case WM_DESTROY:
            // Handle window destruction
            KillTimer(hwnd, 1);       // Kill the timer
            PostQuitMessage(0);       // Post quit message
            break;

        case WM_COMMAND:
            // Handle button click events
            if (LOWORD(wParam) == 1 && currentTabIndex == 2) {
                BrowseFolder(hwnd); // Open folder browser when "Browse Folder" button is clicked
            }
            break;

        case WM_HSCROLL:
            // Handle slider changes
            if ((HWND)lParam == hFontSizeSlider || (HWND)lParam == hRedSlider || (HWND)lParam == hGreenSlider || (HWND)lParam == hBlueSlider) {
                UpdateStyle(hwnd);
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
    const wchar_t* tabNames[] = { L"Hello World", L"Clock", L"Read File", L"Style Settings" };

    for (int i = 0; i < 4; ++i) {
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

    // Destroy sliders and color preview if they exist
    if (hFontSizeSlider) {
        DestroyWindow(hFontSizeSlider);
        hFontSizeSlider = nullptr;
    }
    if (hRedSlider) {
        DestroyWindow(hRedSlider);
        hRedSlider = nullptr;
    }
    if (hGreenSlider) {
        DestroyWindow(hGreenSlider);
        hGreenSlider = nullptr;
    }
    if (hBlueSlider) {
        DestroyWindow(hBlueSlider);
        hBlueSlider = nullptr;
    }
    if (hColorPreview) {
        DestroyWindow(hColorPreview);
        hColorPreview = nullptr;
    }
    if (hRGBValues) {
        DestroyWindow(hRGBValues);
        hRGBValues = nullptr;
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
            ShowStyleSettings(hwnd);
            break;
    }
    ApplyStyleToAllWindows(hwnd);
}

void ShowHelloWorld(HWND hwnd) {
    hChildWnd = CreateWindow(L"STATIC", L"Hello, World!", WS_CHILD | WS_VISIBLE,
                             20, 50, 150, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
}

void ShowClock(HWND hwnd) {
    hChildWnd = CreateWindow(L"STATIC", L"", WS_CHILD | WS_VISIBLE,
                             20, 50, 150, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
    UpdateClock(hwnd);  // Update clock immediately
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

void ShowStyleSettings(HWND hwnd) {
    // Create controls for changing style settings
    hChildWnd = CreateWindow(L"STATIC", L"Style Settings", WS_CHILD | WS_VISIBLE,
                             20, 20, 150, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);

    CreateWindow(L"STATIC", L"Font Size:", WS_CHILD | WS_VISIBLE,
                 20, 60, 80, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);

    hFontSizeSlider = CreateWindow(TRACKBAR_CLASS, nullptr, WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
                                   100, 60, 200, 30, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
    SendMessage(hFontSizeSlider, TBM_SETRANGE, TRUE, MAKELONG(10, 30)); // Set range from 10 to 30
    SendMessage(hFontSizeSlider, TBM_SETPOS, TRUE, 20); // Set initial position

    CreateWindow(L"STATIC", L"Red:", WS_CHILD | WS_VISIBLE,
                 20, 100, 80, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);

    hRedSlider = CreateWindow(TRACKBAR_CLASS, nullptr, WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
                              100, 100, 200, 30, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
    SendMessage(hRedSlider, TBM_SETRANGE, TRUE, MAKELONG(0, 255)); // Set range from 0 to 255
    SendMessage(hRedSlider, TBM_SETPOS, TRUE, GetRValue(textColor)); // Set initial position

    CreateWindow(L"STATIC", L"Green:", WS_CHILD | WS_VISIBLE,
                 20, 140, 80, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);

    hGreenSlider = CreateWindow(TRACKBAR_CLASS, nullptr, WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
                                100, 140, 200, 30, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
    SendMessage(hGreenSlider, TBM_SETRANGE, TRUE, MAKELONG(0, 255)); // Set range from 0 to 255
    SendMessage(hGreenSlider, TBM_SETPOS, TRUE, GetGValue(textColor)); // Set initial position

    CreateWindow(L"STATIC", L"Blue:", WS_CHILD | WS_VISIBLE,
                 20, 180, 80, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);

    hBlueSlider = CreateWindow(TRACKBAR_CLASS, nullptr, WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
                               100, 180, 200, 30, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
    SendMessage(hBlueSlider, TBM_SETRANGE, TRUE, MAKELONG(0, 255)); // Set range from 0 to 255
    SendMessage(hBlueSlider, TBM_SETPOS, TRUE, GetBValue(textColor)); // Set initial position

    hColorPreview = CreateWindow(L"STATIC", nullptr, WS_CHILD | WS_VISIBLE | SS_NOTIFY | SS_SUNKEN,
                                 320, 100, 100, 100, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);

    hRGBValues = CreateWindow(L"STATIC", L"", WS_CHILD | WS_VISIBLE,
                              20, 220, 400, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);

    // Apply initial style
    UpdateStyle(hwnd);
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

    // Resize sliders and color preview
    if (hFontSizeSlider) {
        SetWindowPos(hFontSizeSlider, nullptr, 100, 60, 200, 30, SWP_NOZORDER);
    }
    if (hRedSlider) {
        SetWindowPos(hRedSlider, nullptr, 100, 100, 200, 30, SWP_NOZORDER);
    }
    if (hGreenSlider) {
        SetWindowPos(hGreenSlider, nullptr, 100, 140, 200, 30, SWP_NOZORDER);
    }
    if (hBlueSlider) {
        SetWindowPos(hBlueSlider, nullptr, 100, 180, 200, 30, SWP_NOZORDER);
    }
    if (hColorPreview) {
        SetWindowPos(hColorPreview, nullptr, 320, 100, 100, 100, SWP_NOZORDER);
    }
    if (hRGBValues) {
        SetWindowPos(hRGBValues, nullptr, 20, 220, 400, 20, SWP_NOZORDER);
    }

    // Invalidate the entire window to force a redraw
    InvalidateRect(hwnd, nullptr, TRUE);
    UpdateWindow(hwnd);
}

void ChangeFont(HWND hwnd, const wchar_t* fontName, int fontSize) {
    if (hFont) {
        DeleteObject(hFont); // Delete the old font
    }
    hFont = CreateFont(fontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                       OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, fontName);
    ApplyStyleToAllWindows(hwnd);
}

void ChangeColor(HWND hwnd, COLORREF color) {
    textColor = color;
    ApplyStyleToAllWindows(hwnd);
    UpdatePreview(hwnd);
}

void UpdateStyle(HWND hwnd) {
    // Get the font size from the slider
    int fontSize = SendMessage(hFontSizeSlider, TBM_GETPOS, 0, 0);

    // Get the RGB values from the sliders
    int red = SendMessage(hRedSlider, TBM_GETPOS, 0, 0);
    int green = SendMessage(hGreenSlider, TBM_GETPOS, 0, 0);
    int blue = SendMessage(hBlueSlider, TBM_GETPOS, 0, 0);

    // Update the font and color
    ChangeFont(hwnd, L"Arial", fontSize);
    ChangeColor(hwnd, RGB(red, green, blue));

    // Update the RGB values display
    DisplayRGBValues(hwnd);
}

void UpdatePreview(HWND hwnd) {
    HDC hdc = GetDC(hColorPreview);
    RECT rect;
    GetClientRect(hColorPreview, &rect);
    HBRUSH hBrush = CreateSolidBrush(textColor);
    FillRect(hdc, &rect, hBrush);
    DeleteObject(hBrush);
    ReleaseDC(hColorPreview, hdc);
}

void DisplayRGBValues(HWND hwnd) {
    int red = SendMessage(hRedSlider, TBM_GETPOS, 0, 0);
    int green = SendMessage(hGreenSlider, TBM_GETPOS, 0, 0);
    int blue = SendMessage(hBlueSlider, TBM_GETPOS, 0, 0);

    wchar_t rgbStr[100];
    swprintf(rgbStr, sizeof(rgbStr) / sizeof(wchar_t), L"RGB: (%d, %d, %d)", red, green, blue);
    SetWindowText(hRGBValues, rgbStr);
}

void ApplyStyleToAllWindows(HWND hwnd) {
    EnumChildWindows(hwnd, [](HWND child, LPARAM lParam) -> BOOL {
        SendMessage(child, WM_SETFONT, (WPARAM)hFont, TRUE);
        InvalidateRect(child, nullptr, TRUE);
        UpdateWindow(child);
        return TRUE;
    }, 0);
}
