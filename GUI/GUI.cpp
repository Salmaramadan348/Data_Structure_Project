#include "framework.h"
#include "XmlValidator.h"
#include "XmlParser.h"
#include "Compress.h"
#include "Decompress.h"
#include "XmlPrettify.h" 

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
XMLTree smartFormatter;

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

HWND hEditor = NULL;        // Left / Main Editor
HWND hOutputEditor = NULL;  // Right Editor (For Format Tab)
HWND hTab = NULL;
HWND hBtnCompress = NULL;
HWND hBtnDecompress = NULL;
HWND hBtnPrettify = NULL;

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
    ID_BTN_PRETTIFY = 7,

    ID_EDIT_MAIN = 100,
    ID_TAB_MAIN = 200
};

// ---------------- UTF helpers ----------------
std::string wstringToUtf8(const std::wstring& wstr) {
    if (wstr.empty()) return {};
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string str(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &str[0], size, NULL, NULL);
    return str;
}

std::wstring utf8ToWstring(const std::string& str) {
    if (str.empty()) return {};
    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
    std::wstring wstr(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstr[0], size);
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
    if (wcslen(szTitle) == 0) wcscpy_s(szTitle, L"XML Tool Suite");
    if (wcslen(szWindowClass) == 0) wcscpy_s(szWindowClass, L"XMLValidatorWnd");

    MyRegisterClass(hInstance);
    if (!InitInstance(hInstance, nCmdShow)) return FALSE;

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

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

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
    hInst = hInstance;
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 900, 700, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd) return FALSE;
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
    {
        INITCOMMONCONTROLSEX icex{ sizeof(icex), ICC_TAB_CLASSES };
        InitCommonControlsEx(&icex);

        hTab = CreateWindowEx(0, WC_TABCONTROL, L"", WS_CHILD | WS_VISIBLE, 10, 10, 860, 620, hWnd, (HMENU)ID_TAB_MAIN, hInst, NULL);
        TCITEM tie{};
        tie.mask = TCIF_TEXT;
        tie.pszText = (LPWSTR)L"Verify";   TabCtrl_InsertItem(hTab, 0, &tie);
        tie.pszText = (LPWSTR)L"Format";   TabCtrl_InsertItem(hTab, 1, &tie);
        tie.pszText = (LPWSTR)L"JSON";     TabCtrl_InsertItem(hTab, 2, &tie);
        tie.pszText = (LPWSTR)L"Compress"; TabCtrl_InsertItem(hTab, 3, &tie);
        tie.pszText = (LPWSTR)L"Graph";    TabCtrl_InsertItem(hTab, 4, &tie);

        // Standard Buttons
        CreateWindowW(L"BUTTON", L"Fix XML", WS_CHILD | WS_VISIBLE, 20, 60, 120, 30, hWnd, (HMENU)ID_BTN_FIX, hInst, NULL);
        CreateWindowW(L"BUTTON", L"Validate XML", WS_CHILD | WS_VISIBLE, 150, 60, 150, 30, hWnd, (HMENU)ID_BTN_VALIDATE, hInst, NULL);
        CreateWindowW(L"BUTTON", L"Open XML", WS_CHILD | WS_VISIBLE, 320, 60, 120, 30, hWnd, (HMENU)ID_BTN_OPEN, hInst, NULL);
        CreateWindowW(L"BUTTON", L"Save XML", WS_CHILD | WS_VISIBLE, 460, 60, 120, 30, hWnd, (HMENU)ID_BTN_SAVE, hInst, NULL);

        hBtnPrettify = CreateWindowW(L"BUTTON", L"Prettify", WS_CHILD, 20, 60, 150, 30, hWnd, (HMENU)ID_BTN_PRETTIFY, hInst, NULL);
        hBtnCompress = CreateWindowW(L"BUTTON", L"Compress",
            WS_CHILD,
            50, 250, 320, 100,
            hWnd, (HMENU)ID_BTN_COMPRESS, hInst, NULL);

        hBtnDecompress = CreateWindowW(L"BUTTON", L"Decompress",
            WS_CHILD,
            450, 250, 320, 100,
            hWnd, (HMENU)ID_BTN_DECOMPRESS, hInst, NULL);

        // Create Both Editors with Vertical and Horizontal Scrollbars
        hEditor = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
            20, 100, 840, 500, hWnd, (HMENU)ID_EDIT_MAIN, hInst, NULL);

        hOutputEditor = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_READONLY,
            445, 100, 415, 500, hWnd, NULL, hInst, NULL);
    }
    break;

    case WM_NOTIFY:
    {
        LPNMHDR hdr = (LPNMHDR)lParam;
        if (hdr->idFrom == ID_TAB_MAIN && hdr->code == TCN_SELCHANGE) {
            currentTab = TabCtrl_GetCurSel(hTab);

            BOOL isVerify = (currentTab == 0);
            BOOL isFormat = (currentTab == 1);
            BOOL isCompress = (currentTab == 3);

            // Side-by-Side Logic: Resize hEditor if in Format mode
            SetWindowPos(hEditor, NULL, 20, 100, isFormat ? 415 : 840, 500, SWP_NOZORDER);
            ShowWindow(hOutputEditor, isFormat ? SW_SHOW : SW_HIDE);

            ShowWindow(hEditor, (isVerify || isFormat) ? SW_SHOW : SW_HIDE);
            ShowWindow(GetDlgItem(hWnd, ID_BTN_FIX), isVerify ? SW_SHOW : SW_HIDE);
            ShowWindow(GetDlgItem(hWnd, ID_BTN_VALIDATE), isVerify ? SW_SHOW : SW_HIDE);
            ShowWindow(hBtnPrettify, isFormat ? SW_SHOW : SW_HIDE);
            ShowWindow(GetDlgItem(hWnd, ID_BTN_OPEN), (isVerify || isFormat) ? SW_SHOW : SW_HIDE);
            ShowWindow(GetDlgItem(hWnd, ID_BTN_SAVE), (isVerify || isFormat) ? SW_SHOW : SW_HIDE);
            ShowWindow(hBtnCompress, isCompress ? SW_SHOW : SW_HIDE);
            ShowWindow(hBtnDecompress, isCompress ? SW_SHOW : SW_HIDE);
        }
    }
    break;

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
            if (!lastDirectory.empty()) ofn.lpstrInitialDir = lastDirectory.c_str();

            if (GetOpenFileNameW(&ofn)) {
                lastFilePath = fileName;
                lastDirectory = std::wstring(fileName).substr(0, std::wstring(fileName).find_last_of(L"\\/"));
                std::ifstream file(wstringToUtf8(fileName));
                if (file) {
                    std::stringstream buffer;
                    buffer << file.rdbuf();
                    SetWindowTextW(hEditor, utf8ToWstring(buffer.str()).c_str());
                    SetWindowTextW(hOutputEditor, L""); // Clear output on new open
                }
            }
        }
        break;

        case ID_BTN_PRETTIFY:
        {
            int len = GetWindowTextLengthW(hEditor);
            if (len <= 0) break;
            std::wstring wbuf(len + 1, L'\0');
            GetWindowTextW(hEditor, &wbuf[0], len + 1);

            std::vector<std::string> tokens = parser.extractTags(wstringToUtf8(wbuf));
            Tree tree;
            TreeNode* root = tree.getRoot();
            TreeNode* current = root;

            for (const std::string& token : tokens) {
                std::string t = parser.trim(token);
                if (t.empty()) continue;
                if (validator.isOpeningTag(t)) {
                    TreeNode* newNode = new TreeNode(validator.getTagName(t));
                    current->addChild(newNode);
                    current = newNode;
                }
                else if (validator.isClosingTag(t)) {
                    if (current && current->parent) current = current->parent;
                }
                else {
                    if (current != root) current->tagValue = t;
                }
            }
            // Inside ID_BTN_PRETTIFY
            std::string result = smartFormatter.getFormattedXML(root, 80);
            if (!result.empty()) SetWindowTextW(hOutputEditor, utf8ToWstring(result).c_str());
            else MessageBoxW(hWnd, L"Format failed. Verify XML tags are balanced.", L"Error", MB_OK);
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
            if (!lastDirectory.empty()) ofn.lpstrInitialDir = lastDirectory.c_str();

            if (GetSaveFileNameW(&ofn)) {
                std::wstring fileNameStr(fileName);
                if (fileNameStr.find(L".xml") == std::wstring::npos) fileNameStr += L".xml";

                // Save from Output editor if we are on Format Tab
                HWND hTarget = (currentTab == 1) ? hOutputEditor : hEditor;
                int len = GetWindowTextLengthW(hTarget);
                std::wstring wbuf(len + 1, L'\0');
                GetWindowTextW(hTarget, &wbuf[0], len + 1);

                std::ofstream out(wstringToUtf8(fileNameStr));
                out << wstringToUtf8(wbuf);
                out.close();
                MessageBoxW(hWnd, L"Saved Successfully!", L"Done", MB_OK);
            }
        }
        break;

        // ... Rest of your existing ID_BTN_FIX, VALIDATE, COMPRESS logic ...
        case ID_BTN_FIX:
        {
            int len = GetWindowTextLengthW(hEditor);
            std::wstring wbuf(len + 1, L'\0');
            GetWindowTextW(hEditor, &wbuf[0], len + 1);
            std::string fixed = validator.fixXMLUsingTree(parser.extractTags(wstringToUtf8(wbuf)));
            auto parts = parser.extractTags(fixed);
            std::string noFormatResult;
            for (auto& p : parts) {
                size_t first = p.find_first_not_of(" \t\n\r");
                if (std::string::npos == first) continue;
                size_t last = p.find_last_not_of(" \t\n\r");
                noFormatResult += p.substr(first, (last - first + 1)) + "\r\n";
            }
            SetWindowTextW(hEditor, utf8ToWstring(noFormatResult).c_str());
        }
        break;

        case ID_BTN_VALIDATE:
        {
            int len = GetWindowTextLengthW(hEditor);
            std::wstring wbuf(len + 1, L'\0');
            GetWindowTextW(hEditor, &wbuf[0], len + 1);
            auto errors = validator.checkXml({ wstringToUtf8(wbuf) });
            if (errors.empty()) MessageBoxW(hWnd, L"XML is valid!", L"Success", MB_OK | MB_ICONINFORMATION);
            else {
                std::string msg = "Found " + std::to_string(errors.size()) + " error(s):\n\n";
                for (const auto& e : errors) msg += "Line " + std::to_string(e.lineNumber) + ": " + e.message + "\n";
                MessageBoxW(hWnd, utf8ToWstring(msg).c_str(), L"Validation Errors", MB_OK | MB_ICONERROR);
            }
        }
        break;

        case ID_BTN_COMPRESS:
        {
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
            if (GetSaveFileNameW(&ofn)) {
                SimpleXMLCompressor compressor;
                compressor.compress(wstringToUtf8(lastFilePath), wstringToUtf8(outPath));
                MessageBoxW(hWnd, L"Compression successful!", L"Done", MB_OK);
            }
        }
        break;

        case ID_BTN_DECOMPRESS:
        {
            OPENFILENAMEW ofn{};
            wchar_t inputFileName[MAX_PATH] = L"";
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFilter = L"Compressed Files\0*.pbx\0All Files\0*.*\0";
            ofn.lpstrFile = inputFileName;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
            if (GetOpenFileNameW(&ofn)) {
                OPENFILENAMEW saveOfn{};
                wchar_t outputFileName[MAX_PATH] = L"decompressed_output.xml";
                saveOfn.lStructSize = sizeof(saveOfn);
                saveOfn.hwndOwner = hWnd;
                saveOfn.lpstrFilter = L"XML Files\0*.xml\0All Files\0*.*\0";
                saveOfn.lpstrFile = outputFileName;
                saveOfn.nMaxFile = MAX_PATH;
                saveOfn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
                if (GetSaveFileNameW(&saveOfn)) {
                    SimpleXMLDecompressor decompressor;
                    decompressor.decompress(wstringToUtf8(inputFileName), wstringToUtf8(outputFileName));
                    MessageBoxW(hWnd, L"Decompression successful!", L"Done", MB_OK);
                }
            }
        }
        break;
        }
    }
    break;

    case WM_DESTROY: PostQuitMessage(0); break;
    default: return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}