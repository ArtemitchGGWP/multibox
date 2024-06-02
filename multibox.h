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
void ShowStyleSettings(HWND hwnd);
void UpdateClock(HWND hwnd);
void BrowseFolder(HWND hwnd);
void ListFilesInFolder(HWND hwnd, const std::wstring& folderPath);
void ResizeControls(HWND hwnd);
void ChangeFont(HWND hwnd, const wchar_t* fontName, int fontSize);
void ChangeColor(HWND hwnd, COLORREF color);
void UpdateStyle(HWND hwnd);

// Global variables
extern HWND hTab;              // Handle to the tab control
extern HWND hChildWnd;         // Handle to the child window (content of the tab)
extern HWND hButton;           // Handle to the "Browse Folder" button
extern int currentTabIndex;    // Index of the currently selected tab
extern HFONT hFont;            // Handle to the font
extern COLORREF textColor;     // Color for the text
extern HWND hFontSizeSlider;   // Handle to the font size slider
extern HWND hRedSlider;        // Handle to the red color slider
extern HWND hGreenSlider;      // Handle to the green color slider
extern HWND hBlueSlider;       // Handle to the blue color slider

#endif // MULTIBOX_H
