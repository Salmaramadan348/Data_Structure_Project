#include "framework.h"
#include "test.h"
#include "XmlValidator.h"
#include  "XmlParser.h"
#include "Compressor.h"
#include "Detect_UFT8.h"
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

#include <commdlg.h>
#include <windowsx.h>
#include <commctrl.h>

#pragma comment(lib, "Comctl32.lib")
XmlValidator validator;
XmlParser parser;

#define MAX_LOADSTRING 100

#ifndef IDC_MY
#define IDC_MY 101
#endif

#ifndef IDS_APP_TITLE
#define IDS_APP_TITLE 102
#endif

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

HWND hEditor = NULL;
HWND hTab = NULL;
HWND hBtnCompress = NULL;      
HWND hBtnDecompress = NULL;       
int currentTab = 0;
std::wstring lastFilePath = L"";
std::wstring lastDirectory = L"";


// ---------------- IDs ----------------
enum {
    ID_BTN_FIX = 1,
    ID_BTN_VALIDATE = 2,
    ID_BTN_OPEN = 3,
    ID_BTN_SAVE = 4,
    ID_BTN_COMPRESS = 5,
    ID_BTN_DECOMPRESS = 6,
    ID_EDIT_MAIN = 100,
    ID_TAB_MAIN = 200
};

// ---------------- UTF helpers ----------------
std::string wstringToUtf8(const std::wstring& wstr) {
    if (wstr.empty()) return {};
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
        (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string str(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
        (int)wstr.size(), &str[0], size, NULL, NULL);
    return str;
}

std::wstring utf8ToWstring(const std::string& str) {
    if (str.empty()) return {};
    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(),
        (int)str.size(), NULL, 0);
    std::wstring wstr(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(),
        (int)str.size(), &wstr[0], size);
    return wstr;
}

// ---------------- Prototypes ----------------
ATOM MyRegisterClass(HINSTANCE);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// ---------------- WinMain ----------------
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow) {

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MY, szWindowClass, MAX_LOADSTRING);

    if (wcslen(szTitle) == 0) wcscpy_s(szTitle, L"XML Validator");
    if (wcslen(szWindowClass) == 0) wcscpy_s(szWindowClass, L"XMLValidatorWnd");

    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

// ---------------- Register Class ----------------
ATOM MyRegisterClass(HINSTANCE hInstance) {
    WNDCLASSEXW wcex{};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = szWindowClass;
    return RegisterClassExW(&wcex);
}

// ---------------- Init Window ----------------
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
    hInst = hInstance;

    HWND hWnd = CreateWindowW(
        szWindowClass, szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 900, 700,
        nullptr, nullptr, hInstance, nullptr);

    if (!hWnd) return FALSE;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}

// ---------------- WndProc ----------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

    switch (message) {

    case WM_CREATE:
    {
        INITCOMMONCONTROLSEX icex{ sizeof(icex), ICC_TAB_CLASSES };
        InitCommonControlsEx(&icex);

        // ---- Tabs ----
        hTab = CreateWindowEx(
            0, WC_TABCONTROL, L"",
            WS_CHILD | WS_VISIBLE,
            10, 10, 860, 620,
            hWnd, (HMENU)ID_TAB_MAIN, hInst, NULL
        );

        TCITEM tie{};
        tie.mask = TCIF_TEXT;

        tie.pszText = (LPWSTR)L"Verify";   TabCtrl_InsertItem(hTab, 0, &tie);
        tie.pszText = (LPWSTR)L"Format";   TabCtrl_InsertItem(hTab, 1, &tie);
        tie.pszText = (LPWSTR)L"JSON";     TabCtrl_InsertItem(hTab, 2, &tie);
        tie.pszText = (LPWSTR)L"Compress"; TabCtrl_InsertItem(hTab, 3, &tie);
        tie.pszText = (LPWSTR)L"Graph";    TabCtrl_InsertItem(hTab, 4, &tie);

        // ---- Buttons ----
        CreateWindowW(L"BUTTON", L"Fix XML",
            WS_CHILD | WS_VISIBLE,
            20, 60, 120, 30,
            hWnd, (HMENU)ID_BTN_FIX, hInst, NULL);

        CreateWindowW(L"BUTTON", L"Validate XML",
            WS_CHILD | WS_VISIBLE,
            150, 60, 150, 30,
            hWnd, (HMENU)ID_BTN_VALIDATE, hInst, NULL);

        CreateWindowW(L"BUTTON", L"Open XML",
            WS_CHILD | WS_VISIBLE,
            320, 60, 120, 30,
            hWnd, (HMENU)ID_BTN_OPEN, hInst, NULL);

        CreateWindowW(L"BUTTON", L"Save XML",
            WS_CHILD | WS_VISIBLE,
            460, 60, 120, 30,
            hWnd, (HMENU)ID_BTN_SAVE, hInst, NULL);

        // ---- Editor ----
        hEditor = CreateWindowEx(
            WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_VSCROLL |
            ES_MULTILINE | ES_AUTOVSCROLL,
            20, 100, 840, 500,
            hWnd, (HMENU)ID_EDIT_MAIN, hInst, NULL
        );
        
        hBtnCompress =CreateWindowW(L"BUTTON", L"Compress",
            WS_CHILD ,
            20, 60, 120, 30,
            hWnd, (HMENU)ID_BTN_COMPRESS, hInst, NULL);

       hBtnDecompress = CreateWindowW(L"BUTTON", L"Decompress",
            WS_CHILD,
            150, 60, 120, 30,
            hWnd, (HMENU)ID_BTN_DECOMPRESS, hInst, NULL);
        
        break;
    }


    // ---------------- Tab Change ----------------
    case WM_NOTIFY:
    {
        LPNMHDR hdr = (LPNMHDR)lParam;
        if (hdr->idFrom == ID_TAB_MAIN && hdr->code == TCN_SELCHANGE) {

            currentTab = TabCtrl_GetCurSel(hTab);
            BOOL show = (currentTab == 0);
            BOOL showin_tab3 = (currentTab == 3);

            ShowWindow(hEditor, show ? SW_SHOW : SW_HIDE);
            ShowWindow(GetDlgItem(hWnd, ID_BTN_FIX), show ? SW_SHOW : SW_HIDE);
            ShowWindow(GetDlgItem(hWnd, ID_BTN_VALIDATE), show ? SW_SHOW : SW_HIDE);
            ShowWindow(GetDlgItem(hWnd, ID_BTN_OPEN), show ? SW_SHOW : SW_HIDE);
            ShowWindow(GetDlgItem(hWnd, ID_BTN_SAVE), show ? SW_SHOW : SW_HIDE);

            ShowWindow(hBtnCompress, showin_tab3 ? SW_SHOW : SW_HIDE);
            ShowWindow(hBtnDecompress, showin_tab3 ? SW_SHOW : SW_HIDE);

        }break;
    }


    // ---------------- Buttons ----------------
    case WM_COMMAND:
    {
        switch (LOWORD(wParam)) {

        case ID_BTN_OPEN:
        {
            OPENFILENAMEW ofn{};
            wchar_t fileName[MAX_PATH] = L"";

            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFilter = L"XML Files\0*.xml\0All Files\0*.*\0";
            ofn.lpstrFile = fileName;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;

            if (!lastDirectory.empty())
                ofn.lpstrInitialDir = lastDirectory.c_str();

            if (GetOpenFileNameW(&ofn)) {
                lastFilePath = fileName;
                lastDirectory = std::wstring(fileName).substr(0, std::wstring(fileName).find_last_of(L"\\/"));

                std::ifstream file(wstringToUtf8(fileName));
                if (file) {
                    std::stringstream buffer;
                    buffer << file.rdbuf();

                    // Load raw content directly to preserve original formatting and spacing
                    SetWindowTextW(hEditor, utf8ToWstring(buffer.str()).c_str());
                }
            }break;
        }

        
        case ID_BTN_FIX:
        {
            // 1. Get text from editor
            int len = GetWindowTextLengthW(hEditor);
            std::wstring wbuf(len + 1, L'\0');
            GetWindowTextW(hEditor, &wbuf[0], len + 1);

            std::string input = wstringToUtf8(wbuf);

            // 2. Fix the XML using your Tree logic
            std::string fixed = validator.fixXMLUsingTree(parser.extractTags(input));

            // 3. Re-extract tags to remove the tree's formatting/indentation
            auto parts = parser.extractTags(fixed);

            std::string noFormatResult;
            for (auto& p : parts) {
                // Remove any leading/trailing spaces or newlines from the tag/data
                size_t first = p.find_first_not_of(" \t\n\r");
                if (std::string::npos == first) continue; // Skip if it's only whitespace
                size_t last = p.find_last_not_of(" \t\n\r");
                std::string trimmed = p.substr(first, (last - first + 1));

                // Add to result starting at the beginning of the line
                noFormatResult += trimmed + "\r\n";
            }

            // 4. Update the editor
            SetWindowTextW(hEditor, utf8ToWstring(noFormatResult).c_str());
        }
        break;
        
        case ID_BTN_VALIDATE:
        {
            int len = GetWindowTextLengthW(hEditor);
            std::wstring wbuf(len + 1, L'\0');
            GetWindowTextW(hEditor, &wbuf[0], len + 1);

            // Call the new checkXml function which returns std::vector<XmlError>
            auto errors = validator.checkXml({ wstringToUtf8(wbuf) });


            if (errors.empty()) {
                MessageBoxW(hWnd, L"XML is valid!", L"Success", MB_OK | MB_ICONINFORMATION);
            }
            else {
                std::string msg = "Found " + std::to_string(errors.size()) + " error(s):\n\n";
                for (const auto& e : errors) {
                    msg += "Line " + std::to_string(e.lineNumber) + ": " + e.message + "\n";
                }
                MessageBoxW(hWnd, utf8ToWstring(msg).c_str(), L"Validation Errors", MB_OK | MB_ICONERROR);
            }
        }
        break;
        

        case ID_BTN_SAVE:
        {
            OPENFILENAMEW ofn{};
            wchar_t fileName[MAX_PATH] = L"output.xml";

            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFilter = L"XML Files\0*.xml\0All Files\0*.*\0";
            ofn.lpstrFile = fileName;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST;

            if (!lastDirectory.empty())
                ofn.lpstrInitialDir = lastDirectory.c_str();

            if (GetSaveFileNameW(&ofn)) {
                std::wstring fileNameStr(fileName);

                if (fileNameStr.find(L".xml") == std::wstring::npos) {
                    fileNameStr += L".xml";
                }

                lastFilePath = fileNameStr;
                lastDirectory = lastFilePath.substr(0, lastFilePath.find_last_of(L"\\/"));

                int len = GetWindowTextLengthW(hEditor);
                std::wstring wbuf(len + 1, L'\0');
                GetWindowTextW(hEditor, &wbuf[0], len + 1);

                std::ofstream out(wstringToUtf8(fileNameStr));
                out << wstringToUtf8(wbuf);
                out.close();

                MessageBoxW(hWnd, L"Saved Successfully!", L"Done", MB_OK);
            }

        }break;

        case ID_BTN_COMPRESS: {
            if (lastFilePath.empty()) {
                MessageBoxW(hWnd, L"No file opened from disk!", L"Error", MB_OK | MB_ICONERROR);
                break;
            }


            OPENFILENAMEW ofn{};
            wchar_t outPath[MAX_PATH] = L"compressed.pbx";

            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFilter = L"Compressed Files\0*.pbx\0All Files\0*.*\0";
            ofn.lpstrFile = outPath;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST;

            if (!GetSaveFileNameW(&ofn))
                break; // user cancelled

            //  Convert paths to UTF-8
            std::string inputFile = wstringToUtf8(lastFilePath);
            std::string outputFile = wstringToUtf8(outPath);

            Compressor compressor;


            if (compressor.compressPairByte(inputFile, outputFile)) {
                MessageBoxW(hWnd, L"Compression successful!", L"Done", MB_OK);
            }
            else {
                MessageBoxW(hWnd, L"Compression failed!", L"Error", MB_OK | MB_ICONERROR);
            }
            
            
            
            break; }
        
        case ID_BTN_DECOMPRESS: {

            // choose (.pbx) file
            OPENFILENAMEW ofn{};
            wchar_t inputFileName[MAX_PATH] = L"";

            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFilter = L"Compressed Files\0*.pbx\0All Files\0*.*\0";
            ofn.lpstrFile = inputFileName;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;

            if (!GetOpenFileNameW(&ofn)) break; 

            std::string inputFile = wstringToUtf8(inputFileName);

           

            OPENFILENAMEW saveOfn{};
            wchar_t outputFileName[MAX_PATH] = L"decompressed_output.xml";

            saveOfn.lStructSize = sizeof(saveOfn);
            saveOfn.hwndOwner = hWnd;
            saveOfn.lpstrFilter = L"XML Files\0*.xml\0All Files\0*.*\0";
            saveOfn.lpstrFile = outputFileName;
            saveOfn.nMaxFile = MAX_PATH;
            saveOfn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

            if (!lastDirectory.empty())
                saveOfn.lpstrInitialDir = lastDirectory.c_str();

            if (!GetSaveFileNameW(&saveOfn)) {
                // user cancelled
                break;
            }

            std::string outputFile = wstringToUtf8(outputFileName);

            Compressor compressor;

            if (compressor.decompressPairByte(inputFile, outputFile)) {
                MessageBoxW(hWnd, L"Decompression successful!", L"Done", MB_OK);
            }
            else {
                MessageBoxW(hWnd, L"Decompression failed!", L"Error", MB_OK | MB_ICONERROR);
            }

            lastDirectory = std::wstring(outputFileName).substr(
                0, std::wstring(outputFileName).find_last_of(L"\\/")
            );

            break;
        

        }

        }

        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;

}