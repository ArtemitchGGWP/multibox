#ifndef MULTIBOX_H
#define MULTIBOX_H

#include <windows.h>    // Core Windows functions
#include <commctrl.h>   // Common controls library
#include <ctime>        // Time functions
#include <fstream>      // File input/output
#include <sstream>      // String stream
#include <shlobj.h>     // Shell API functions
#include <string>       // String functions

// Function declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void AddTabs(HWND hwnd);
void DisplayTabContent(HWND hwnd, int tabIndex);
void ShowHelloWorld(HWND hwnd);
void ShowClock(HWND hwnd);
void ShowFileContent(HWND hwnd);
void UpdateClock(HWND hwnd);
void BrowseFolder(HWND hwnd);
void ListFilesInFolder(HWND hwnd, const std::wstring& folderPath);
void ResizeControls(HWND hwnd);

// Global variables
extern HWND hTab;              // Handle to the tab control
extern HWND hChildWnd;         // Handle to the child window (content of the tab)
extern HWND hButton;           // Handle to the "Browse Folder" button
extern int currentTabIndex;    // Index of the currently selected tab

#endif // MULTIBOX_H
