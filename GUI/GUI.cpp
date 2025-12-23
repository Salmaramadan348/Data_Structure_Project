#include "framework.h"
#include "test.h"
#include "XmlValidator.h"
#include "XmlParser.h"
#include "Xml_to_Json.h"
#include "Compress.h"
#include "Decompress.h"
#include "XmlPrettify.h" 
#include "Post_search.h"
#include "User.h"
#include "XMLToUsersParser.h"
#include "Graph.h"  
#include "minify.h"

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
Graph graph;  // Global Graph object

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
HWND hBtnPrettify = NULL;
 
HWND hOutputEditor = NULL; 

HWND hJsonLeftEdit = NULL;
HWND hJsonRightEdit = NULL;
HWND hBtnOpenXmlJson = NULL;
HWND hBtnConvertJson = NULL;
HWND hBtnSaveJson = NULL;
HWND hEditUserID; 
HWND hEditSuggest;  



int currentTab = 0;
std::wstring lastFilePath = L"";
std::wstring lastDirectory = L"";

// Global variables for search functionality
std::vector<User> loadedUsers;
std::wstring searchFilePath = L"";

// ---------------- IDs ----------------
enum {
    ID_BTN_FIX = 1,
    ID_BTN_VALIDATE = 2,
    ID_BTN_OPEN = 3,
    ID_BTN_SAVE = 4,
    ID_BTN_COMPRESS = 5,
    ID_BTN_DECOMPRESS = 6,
    ID_BTN_PRETTIFY = 7,
    ID_BTN_MINIFY = 1003,

    ID_EDIT_MAIN = 100,
    ID_TAB_MAIN = 200,

    // Level Two Window IDs
    ID_EDIT_FILEPATH_SEARCH = 300,
    ID_BTN_BROWSE_SEARCH = 301,
    ID_RADIO_TOPIC = 302,
    ID_RADIO_WORD = 303,
    ID_EDIT_SEARCH_KEY = 304,
    ID_BTN_GO_SEARCH = 305,
    ID_EDIT_RESULTS = 306,
    ID_TAB_SEARCH = 307,
    ID_EDIT_FILEPATH = 2001,
    
    // Analysis Tab IDs
    ID_BTN_MOST_ACTIVE = 400,
    ID_BTN_MOST_INFLUENCER = 401,
    ID_BTN_MUTUAL = 402,
    ID_BTN_SUGGEST = 403,
    ID_EDIT_ANALYSIS_RESULTS = 404,
     IDD_INPUT_DIALOG= 500,
     IDC_EDIT_INPUT  = 501,
     ID_EDIT_USERID= 3001,
     ID_EDIT_SUGGEST= 3002,
 // JSON Tab buttons
        ID_BTN_OPEN_XML_JSON = 200,
        ID_BTN_CONVERT_JSON = 201,
        ID_BTN_SAVE_JSON = 202,

        ID_EDIT_JSON_LEFT = 101,
        ID_EDIT_JSON_RIGHT = 102,


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
ATOM RegisterLevelTwoClass(HINSTANCE);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK LevelTwoWndProc(HWND, UINT, WPARAM, LPARAM);
void CreateLevelTwoWindow(HINSTANCE hInstance);

// ---------------- WinMain ----------------
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow) {

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MY, szWindowClass, MAX_LOADSTRING);

    if (wcslen(szTitle) == 0) wcscpy_s(szTitle, L"XML Validator");
    if (wcslen(szWindowClass) == 0) wcscpy_s(szWindowClass, L"XMLValidatorWnd");

    MyRegisterClass(hInstance);
    RegisterLevelTwoClass(hInstance);

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

// ---------------- Register Level Two Class ----------------
ATOM RegisterLevelTwoClass(HINSTANCE hInstance) {
    WNDCLASSEXW wcex{};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = LevelTwoWndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = L"LevelTwoWindow";
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

// ---------------- Create Level Two Window ----------------
void CreateLevelTwoWindow(HINSTANCE hInstance) {
    HWND hLevelTwo = CreateWindowW(
        L"LevelTwoWindow", L"MainWindow",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 900, 700,
        nullptr, nullptr, hInstance, nullptr);

    if (hLevelTwo) {
        ShowWindow(hLevelTwo, SW_SHOW);
        UpdateWindow(hLevelTwo);
    }
}

// ---------------- Level Two Window Procedure ----------------
LRESULT CALLBACK LevelTwoWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static HWND hEditFilePath = NULL;
    static HWND hBtnBrowse = NULL;
    static HWND hRadioTopic = NULL;
    static HWND hRadioWord = NULL;
    static HWND hEditSearchKey = NULL;
    static HWND hBtnGo = NULL;
    static HWND hEditResults = NULL;
    static HWND hTabLevelTwo = NULL;
    
    // Analysis Tab Controls
    static HWND hBtnMostActive = NULL;
    static HWND hBtnMostInfluencer = NULL;
    static HWND hBtnMutual = NULL;
    static HWND hBtnSuggest = NULL;
    static HWND hEditAnalysisResults = NULL;
    static int currentLevelTwoTab = 1; // Default to Search tab

    switch (message) {
    case WM_CREATE:
    {
        INITCOMMONCONTROLSEX icex{ sizeof(icex), ICC_TAB_CLASSES };
        InitCommonControlsEx(&icex);

        // Create Tabs
        hTabLevelTwo = CreateWindowEx(
            0, WC_TABCONTROL, L"",
            WS_CHILD | WS_VISIBLE,
            10, 10, 1260, 50,
            hWnd, (HMENU)ID_TAB_SEARCH, hInst, NULL
        );

        TCITEM tie{};
        tie.mask = TCIF_TEXT;
        tie.pszText = (LPWSTR)L"Analysis";   TabCtrl_InsertItem(hTabLevelTwo, 0, &tie);
        tie.pszText = (LPWSTR)L"Search";     TabCtrl_InsertItem(hTabLevelTwo, 1, &tie);
        tie.pszText = (LPWSTR)L"Draw";       TabCtrl_InsertItem(hTabLevelTwo, 2, &tie);
        
        TabCtrl_SetCurSel(hTabLevelTwo, 1); // Set "Search" tab as active

        // ============ SEARCH TAB CONTROLS ============
       


        hEditFilePath = CreateWindowExW(
            WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | ES_AUTOHSCROLL | ES_READONLY,
            100, 58, 400, 25,
            hWnd, (HMENU)ID_EDIT_FILEPATH, hInst, NULL);

        hBtnBrowse = CreateWindowW(L"BUTTON", L"Open XML File",
            WS_CHILD,
            10, 90, 120, 25,
            hWnd, (HMENU)ID_BTN_BROWSE_SEARCH, hInst, NULL);

        HWND hStaticSelect = CreateWindowW(L"STATIC", L"Select File:",
            WS_CHILD,
            10, 60, 80, 20,
            hWnd, NULL, hInst, NULL);
        ShowWindow(hStaticSelect, SW_SHOW);

        HWND hStaticSearchBy = CreateWindowW(L"STATIC", L"Search By:",
            WS_CHILD,
            10, 130, 80, 20,
            hWnd, NULL, hInst, NULL);
        ShowWindow(hStaticSearchBy, SW_SHOW);



        hRadioTopic = CreateWindowW(L"BUTTON", L"Topic",
            WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,
            200, 135, 80, 20,
            hWnd, (HMENU)ID_RADIO_TOPIC, hInst, NULL);

        hRadioWord = CreateWindowW(L"BUTTON", L"Word",
            WS_CHILD | BS_AUTORADIOBUTTON,
            200, 160, 80, 20,
            hWnd, (HMENU)ID_RADIO_WORD, hInst, NULL);

        SendMessage(hRadioTopic, BM_SETCHECK, BST_CHECKED, 0);

        CreateWindowW(L"STATIC", L"Key",
            WS_CHILD | WS_VISIBLE,  
            10, 200, 30, 20,
            hWnd, NULL, hInst, NULL);


        hEditSearchKey = CreateWindowExW(
            WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | ES_AUTOHSCROLL,
            50, 198, 400, 25,
            hWnd, (HMENU)ID_EDIT_SEARCH_KEY, hInst, NULL);

        hBtnGo = CreateWindowW(L"BUTTON", L"Go",
            WS_CHILD | BS_DEFPUSHBUTTON,
            300, 135, 150, 35,
            hWnd, (HMENU)ID_BTN_GO_SEARCH, hInst, NULL);

        hEditResults = CreateWindowExW(
            WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
            470, 60, 400, 600,
            hWnd, (HMENU)ID_EDIT_RESULTS, hInst, NULL);

        // ============ ANALYSIS TAB CONTROLS ============
        hBtnMostActive = CreateWindowW(L"BUTTON", L"Most Active",
            WS_CHILD | BS_DEFPUSHBUTTON,
            20, 100, 200, 50,
            hWnd, (HMENU)ID_BTN_MOST_ACTIVE, hInst, NULL);

        hBtnMostInfluencer = CreateWindowW(L"BUTTON", L"Most Influencer",
            WS_CHILD | BS_DEFPUSHBUTTON,
            20, 170, 200, 50,
            hWnd, (HMENU)ID_BTN_MOST_INFLUENCER, hInst, NULL);

       
        hBtnMutual = CreateWindowW(
            L"BUTTON",
            L"Mutual Followers",
            WS_CHILD | BS_DEFPUSHBUTTON,  
            20, 240, 200, 40,
            hWnd,
            (HMENU)ID_BTN_MUTUAL,
            hInst,
            NULL
        );

        
        hEditUserID = CreateWindowExW(
            0, L"EDIT", L"",
            WS_CHILD | WS_BORDER | ES_LEFT,  
            230, 240, 50, 30,
            hWnd,
            (HMENU)ID_EDIT_USERID,
            hInst,
            NULL
        );

        


        hBtnSuggest = CreateWindowW(
            L"BUTTON",
            L"Suggest Followers",
            WS_CHILD | BS_DEFPUSHBUTTON, 
            20, 300, 200, 40,
            hWnd,
            (HMENU)ID_BTN_SUGGEST,
            hInst,
            NULL
        );

        hEditSuggest = CreateWindowExW(
            0, L"EDIT", L"",
            WS_CHILD | WS_BORDER | ES_LEFT,  
            230, 300, 50, 30,
            hWnd,
            (HMENU)ID_EDIT_SUGGEST,
            hInst,
            NULL
        );





        hEditAnalysisResults = CreateWindowExW(
            WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
            300, 60, 570, 600,
            hWnd, (HMENU)ID_EDIT_ANALYSIS_RESULTS, hInst, NULL);

        // Set font for all controls
        HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
        
        SendMessage(hEditResults, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hEditAnalysisResults, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hBtnMostActive, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hBtnMostInfluencer, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hBtnMutual, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hBtnSuggest, WM_SETFONT, (WPARAM)hFont, TRUE);
        
        // Show Search tab controls by default
        ShowWindow(hEditFilePath, SW_SHOW);
        ShowWindow(hBtnBrowse, SW_SHOW);
        ShowWindow(hRadioTopic, SW_SHOW);
        ShowWindow(hRadioWord, SW_SHOW);
        ShowWindow(hEditSearchKey, SW_SHOW);
        ShowWindow(hBtnGo, SW_SHOW);
        ShowWindow(hEditResults, SW_SHOW);

    }
    break;

   
    case WM_NOTIFY:
    {
        LPNMHDR hdr = (LPNMHDR)lParam;
        if (hdr->idFrom == ID_TAB_SEARCH && hdr->code == TCN_SELCHANGE) {
            currentLevelTwoTab = TabCtrl_GetCurSel(hTabLevelTwo);

            BOOL isAnalysis = (currentLevelTwoTab == 0);
            BOOL isSearch = (currentLevelTwoTab == 1);

            // Show/Hide Search Tab Controls
            ShowWindow(hEditFilePath, isSearch ? SW_SHOW : SW_HIDE);
            ShowWindow(hBtnBrowse, isSearch ? SW_SHOW : SW_HIDE);
            ShowWindow(hRadioTopic, isSearch ? SW_SHOW : SW_HIDE);
            ShowWindow(hRadioWord, isSearch ? SW_SHOW : SW_HIDE);
            ShowWindow(hEditSearchKey, isSearch ? SW_SHOW : SW_HIDE);
            ShowWindow(hBtnGo, isSearch ? SW_SHOW : SW_HIDE);
            ShowWindow(hEditResults, isSearch ? SW_SHOW : SW_HIDE);
            ShowWindow(GetDlgItem(hWnd, 0), isSearch ? SW_SHOW : SW_HIDE); // Static labels

            // Show/Hide Analysis Tab Controls
            ShowWindow(hBtnMostActive, isAnalysis ? SW_SHOW : SW_HIDE);
            ShowWindow(hBtnMostInfluencer, isAnalysis ? SW_SHOW : SW_HIDE);
            ShowWindow(hBtnMutual, isAnalysis ? SW_SHOW : SW_HIDE);
            ShowWindow(hBtnSuggest, isAnalysis ? SW_SHOW : SW_HIDE);
            ShowWindow(hEditAnalysisResults, isAnalysis ? SW_SHOW : SW_HIDE);

            // *** ADD THESE TWO LINES ***
            ShowWindow(hEditUserID, isAnalysis ? SW_SHOW : SW_HIDE);
            ShowWindow(hEditSuggest, isAnalysis ? SW_SHOW : SW_HIDE);
        }
    }
    break;

    case WM_COMMAND:
    {
        switch (LOWORD(wParam)) {
        case ID_BTN_BROWSE_SEARCH:
        {
            OPENFILENAMEW ofn{};
            wchar_t fileName[MAX_PATH] = L"";

            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFilter = L"XML Files\0*.xml\0All Files\0*.*\0";
            ofn.lpstrFile = fileName;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;

            if (GetOpenFileNameW(&ofn)) {
                searchFilePath = fileName;
                SetWindowTextW(hEditFilePath, fileName);

                std::ifstream file(wstringToUtf8(fileName));
                if (!file) {
                    MessageBoxW(hWnd, L"Failed to open file!", L"Error", MB_OK | MB_ICONERROR);
                    break;
                }

                std::stringstream buffer;
                buffer << file.rdbuf();
                std::string xmlContent = buffer.str();

                std::vector<std::string> tags = parser.extractTags(xmlContent);
                loadedUsers = parseUsersFromTags(tags);

                // Build graph for analysis
                graph.buildGraph(loadedUsers);

                SetWindowTextW(hEditAnalysisResults, L"File loaded successfully. Graph built.");
            }
        }
        break;

        case ID_BTN_GO_SEARCH:
        {
            wchar_t filePath[MAX_PATH];
            GetWindowTextW(hEditFilePath, filePath, MAX_PATH);

            if (wcslen(filePath) == 0) {
                MessageBoxW(hWnd, L"Please select a file first!", L"Error", MB_OK | MB_ICONERROR);
                break;
            }

            wchar_t searchKey[256];
            GetWindowTextW(hEditSearchKey, searchKey, 256);

            if (wcslen(searchKey) == 0) {
                MessageBoxW(hWnd, L"Please enter a search key!", L"Error", MB_OK | MB_ICONWARNING);
                break;
            }

            bool searchByTopicMode = (SendMessage(hRadioTopic, BM_GETCHECK, 0, 0) == BST_CHECKED);
            std::string searchKeyStr = parser.trim(wstringToUtf8(searchKey));

            std::vector<PostResult> resultsWithUser;
            if (searchByTopicMode)
                resultsWithUser = searchByTopicWithUser(loadedUsers, searchKeyStr);
            else
                resultsWithUser = searchByWordWithUser(loadedUsers, searchKeyStr);

            std::wstring resultsText;
            if (resultsWithUser.empty()) {
                resultsText = L"No results found.";
            }
            else {
                for (const PostResult& res : resultsWithUser) {
                    resultsText += L"User ID: " + std::to_wstring(res.userId) + L"\r\n";
                    resultsText += L"Post Topics: ";
                    for (size_t j = 0; j < res.post.topics.size(); j++) {
                        resultsText += L"< " + utf8ToWstring(parser.trim(res.post.topics[j])) + L" >";
                        if (j < res.post.topics.size() - 1) resultsText += L" ";
                    }
                    resultsText += L"\r\nPost Content:\r\n";
                    resultsText += utf8ToWstring(parser.trim(res.post.body)) + L"\r\n";
                    resultsText += L"\r\n--------------------------------------------------\r\n\r\n";
                }
            }

            SetWindowTextW(hEditResults, resultsText.c_str());
        }
        break;

        case ID_BTN_MOST_ACTIVE:
        {
            if (loadedUsers.empty()) {
                MessageBoxW(hWnd, L"Please load an XML file first!", L"Error", MB_OK | MB_ICONERROR);
                break;
            }

            std::vector<int> active = graph.mostActive();
            std::wstring result = L"Most Active Users:\r\n\r\n";

            // Get outgoing map
            const auto& outgoingMap = graph.getOutgoing();

            for (int id : active) {
                result += L"User ID: " + std::to_wstring(id) + L"\r\n";

                // Safely check if id exists in map
                auto it = outgoingMap.find(id);
                size_t followingCount = (it != outgoingMap.end()) ? it->second.size() : 0;

                result += L"Following Count: " + std::to_wstring(followingCount) + L"\r\n\r\n";
            }

            SetWindowTextW(hEditAnalysisResults, result.c_str());
        }
        break;

        case ID_BTN_MOST_INFLUENCER:
        {
            if (loadedUsers.empty()) {
                MessageBoxW(hWnd, L"Please load an XML file first!", L"Error", MB_OK | MB_ICONERROR);
                break;
            }

            std::vector<int> influencers = graph.mostInfluencer();
            std::wstring result = L"Most Influential Users:\r\n\r\n";

            // Get incoming map
            const auto& incomingMap = graph.getIncoming();

            for (int id : influencers) {
                result += L"User ID: " + std::to_wstring(id) + L"\r\n";

                auto it = incomingMap.find(id);
                size_t followersCount = (it != incomingMap.end()) ? it->second.size() : 0;

                result += L"Followers Count: " + std::to_wstring(followersCount) + L"\r\n\r\n";
            }

            SetWindowTextW(hEditAnalysisResults, result.c_str());
        }
        break;


        case ID_BTN_MUTUAL:
        {
            wchar_t input[1024];
            GetWindowTextW(hEditUserID, input, 1024); 

            std::wstring idsStr = input;
            std::vector<int> ids;
            size_t start = 0;
            size_t pos = idsStr.find(L',');

            while (pos != std::wstring::npos) {
                std::wstring part = idsStr.substr(start, pos - start);
                part.erase(0, part.find_first_not_of(L" \t"));
                part.erase(part.find_last_not_of(L" \t") + 1);

                if (!part.empty()) {
                    int id = _wtoi(part.c_str());
                    if (id > 0) ids.push_back(id);
                }

                start = pos + 1;
                pos = idsStr.find(L',', start);
            }

            std::wstring part = idsStr.substr(start);
            part.erase(0, part.find_first_not_of(L" \t"));
            part.erase(part.find_last_not_of(L" \t") + 1);
            if (!part.empty()) {
                int id = _wtoi(part.c_str());
                if (id > 0) ids.push_back(id);
            }

            if (ids.empty()) {
                MessageBoxW(hWnd, L"Invalid User IDs", L"Error", MB_OK | MB_ICONERROR);
                break;
            }

            auto mutual = graph.mutualFollowers(ids);

            std::wstring result = L"Mutual Followers:\r\n";
            for (int id : mutual)
                result += L"User ID: " + std::to_wstring(id) + L"\r\n";

            SetWindowTextW(hEditAnalysisResults, result.c_str());
        }
        break;





        case ID_BTN_SUGGEST:
        {
            wchar_t input[1024] = L"";
            GetWindowTextW(hEditSuggest, input, 1024);  

            std::wstring s = input;
            s.erase(0, s.find_first_not_of(L" \t\r\n"));
            s.erase(s.find_last_not_of(L" \t\r\n") + 1);

            int userId = _wtoi(s.c_str());

            if (userId <= 0) {
                MessageBoxW(hWnd, L"Invalid User ID", L"Error", MB_OK | MB_ICONERROR);
                break;
            }

            auto suggestions = graph.suggest(userId);

            std::wstring result = L"Suggestions:\r\n";
            for (int id : suggestions)
                result += L"User ID: " + std::to_wstring(id) + L"\r\n";

            SetWindowTextW(hEditAnalysisResults, result.c_str());
        }
        break;


        
        }
    }
    break;

    case WM_DESTROY:
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
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
        
        tie.pszText = (LPWSTR)L"Level Two"; TabCtrl_InsertItem(hTab, 4, &tie);

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

        hBtnPrettify = CreateWindowW(L"BUTTON", L"Prettify", WS_CHILD, 20, 60, 150, 30, hWnd, (HMENU)ID_BTN_PRETTIFY, hInst, NULL);
        // ---- Output Editor for Prettify ----
        hOutputEditor = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_MULTILINE |
            ES_AUTOVSCROLL | WS_VSCROLL | WS_HSCROLL | ES_READONLY, 445, 100, 415, 500, hWnd, NULL, hInst, NULL);

        ShowWindow(hOutputEditor, SW_HIDE);


        hBtnCompress = CreateWindowW(L"BUTTON", L"Compress",
            WS_CHILD,
            50, 250, 320, 100,
            hWnd, (HMENU)ID_BTN_COMPRESS, hInst, NULL);

        hBtnDecompress = CreateWindowW(L"BUTTON", L"Decompress",
            WS_CHILD,
            450, 250, 320, 100,
            hWnd, (HMENU)ID_BTN_DECOMPRESS, hInst, NULL);
        CreateWindowW(L"BUTTON", L"Minify XML",
                            WS_CHILD | WS_VISIBLE,
                            680, 60, 120, 30,
                            hWnd, (HMENU)ID_BTN_MINIFY, hInst, NULL);
        // Create Editor
        hEditor = CreateWindowEx(
            WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | 
            ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,    
            20, 100, 840, 500,
            hWnd, (HMENU)ID_EDIT_MAIN, hInst, NULL
        );

        // ---- JSON Tab Controls ----
            // Buttons for JSON Tab
            hBtnOpenXmlJson = CreateWindowW(L"BUTTON", L"Open XML",
                WS_CHILD,
                20, 60, 120, 30,
                hWnd, (HMENU)ID_BTN_OPEN_XML_JSON, hInst, NULL);

            hBtnConvertJson = CreateWindowW(L"BUTTON", L"Convert to JSON",
                WS_CHILD,
                150, 60, 150, 30,
                hWnd, (HMENU)ID_BTN_CONVERT_JSON, hInst, NULL);

            hBtnSaveJson = CreateWindowW(L"BUTTON", L"Save JSON",
                WS_CHILD,
                320, 60, 120, 30,
                hWnd, (HMENU)ID_BTN_SAVE_JSON, hInst, NULL);

            // Left editor (XML)
            hJsonLeftEdit = CreateWindowEx(
                WS_EX_CLIENTEDGE, L"EDIT", L"",
                WS_CHILD | WS_VSCROLL | WS_HSCROLL |
                ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
                20, 100, 410, 500,
                hWnd, (HMENU)ID_EDIT_JSON_LEFT, hInst, NULL
            );

            // Right editor (JSON)
            hJsonRightEdit = CreateWindowEx(
                WS_EX_CLIENTEDGE, L"EDIT", L"",
                WS_CHILD | WS_VSCROLL | WS_HSCROLL |
                ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_READONLY,
                440, 100, 410, 500,
                hWnd, (HMENU)ID_EDIT_JSON_RIGHT, hInst, NULL
            );

            // Set font for better readability
            HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY, FF_MODERN, L"Consolas");

            SendMessage(hJsonLeftEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hJsonRightEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
        }
        break;

    // ---------------- Tab Change ----------------
    case WM_NOTIFY:
    {
        LPNMHDR hdr = (LPNMHDR)lParam;
        if (hdr->idFrom == ID_TAB_MAIN && hdr->code == TCN_SELCHANGE) {

            currentTab = TabCtrl_GetCurSel(hTab);
            BOOL show = (currentTab == 0);
            BOOL showin_tab3 = (currentTab == 3);
           
            BOOL isVerify = (currentTab == 0);
            BOOL isLevelTwo = (currentTab == 4);
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
                            // JSON Tab (2)
                BOOL showJson = (currentTab == 2);
           ShowWindow(hBtnOpenXmlJson, showJson ? SW_SHOW : SW_HIDE);
           ShowWindow(hBtnConvertJson, showJson ? SW_SHOW : SW_HIDE);
           ShowWindow(hBtnSaveJson, showJson ? SW_SHOW : SW_HIDE);
           ShowWindow(hJsonLeftEdit, showJson ? SW_SHOW : SW_HIDE);
           ShowWindow(hJsonRightEdit, showJson ? SW_SHOW : SW_HIDE);
           ShowWindow(GetDlgItem(hWnd, ID_BTN_MINIFY), show ? SW_SHOW : SW_HIDE);
            // Open Level Two window when Level Two tab is clicked
            if (isLevelTwo) {
                CreateLevelTwoWindow(hInst);
            }
        }
    }
    break;

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
                    SetWindowTextW(hEditor, utf8ToWstring(buffer.str()).c_str());
                }
            }
        }
        break;

        case ID_BTN_FIX:
        {
            int len = GetWindowTextLengthW(hEditor);
            std::wstring wbuf(len + 1, L'\0');
            GetWindowTextW(hEditor, &wbuf[0], len + 1);

            std::string input = wstringToUtf8(wbuf);
            std::string fixed = validator.fixXMLUsingTree(parser.extractTags(input));
            auto parts = parser.extractTags(fixed);

            std::string noFormatResult;
            for (auto& p : parts) {
                size_t first = p.find_first_not_of(" \t\n\r");
                if (std::string::npos == first) continue;
                size_t last = p.find_last_not_of(" \t\n\r");
                std::string trimmed = p.substr(first, (last - first + 1));
                noFormatResult += trimmed + "\r\n";
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
        case ID_BTN_MINIFY: {
                                int len = GetWindowTextLengthW(hEditor);
                                if (len == 0) break;
            
                                std::wstring wbuf(len + 1, L'\0');
                                GetWindowTextW(hEditor, &wbuf[0], len + 1);
                                std::string input = wstringToUtf8(wbuf);
            
                                std::string minified = minifyXML(input);
            
                                SetWindowTextW(hEditor, utf8ToWstring(minified).c_str());
                            }
                            break;

        case ID_BTN_PRETTIFY:
        {
            if (!hOutputEditor) break; // safety check
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

            std::string result = smartFormatter.getFormattedXML(root, 80);
            if (!result.empty())
                SetWindowTextW(hOutputEditor, utf8ToWstring(result).c_str());
            else
                MessageBoxW(hWnd, L"Format failed. Verify XML tags are balanced.", L"Error", MB_OK);
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
        }
        break;




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

            // Convert paths to UTF-8
            std::string inputFile = wstringToUtf8(lastFilePath);
            std::string outputFile = wstringToUtf8(outPath);

            SimpleXMLCompressor compressor;
            compressor.compress(inputFile, outputFile);

            MessageBoxW(hWnd, L"Compression successful!", L"Done", MB_OK);
        } break;

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

            if (!GetOpenFileNameW(&ofn))
                break;

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
                break; // user cancelled
            }

            std::string outputFile = wstringToUtf8(outputFileName);

            SimpleXMLDecompressor decompressor;
            decompressor.decompress(inputFile, outputFile);

            MessageBoxW(hWnd, L"Decompression successful!", L"Done", MB_OK);

            lastDirectory = std::wstring(outputFileName).substr(
                0, std::wstring(outputFileName).find_last_of(L"\\/")
            );
        } break;
                              // ========== JSON TAB BUTTONS (WORKING) ==========
                                  case ID_BTN_OPEN_XML_JSON:
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
                      
                                          std::ifstream file(wstringToUtf8(fileName), std::ios::binary);
                                          if (file) {
                                              std::stringstream buffer;
                                              buffer << file.rdbuf();
                                              std::string content = buffer.str();
                      
                                              // Convert line endings to Windows format (\r\n)
                                              std::string result;
                                              for (size_t i = 0; i < content.length(); i++) {
                                                  if (content[i] == '\n' && (i == 0 || content[i - 1] != '\r')) {
                                                      result += "\r\n";
                                                  }
                                                  else if (content[i] != '\r' || (i + 1 < content.length() && content[i + 1] != '\n')) {
                                                      result += content[i];
                                                  }
                                                  else {
                                                      result += content[i];
                                                  }
                                              }
                      
                                              // Load XML content to left editor
                                              SetWindowTextW(hJsonLeftEdit, utf8ToWstring(result).c_str());
                                              // Clear right editor
                                              SetWindowTextW(hJsonRightEdit, L"");
                                          }
                                      }
                                  }
                                  break;
                      
                                  case ID_BTN_CONVERT_JSON:
                                  {
                                      // Get XML from left editor
                                      int len = GetWindowTextLengthW(hJsonLeftEdit);
                                      if (len == 0) {
                                          MessageBoxW(hWnd, L"Please open an XML file first!", L"Error", MB_OK | MB_ICONERROR);
                                          break;
                                      }
                      
                                      std::wstring wbuf(len + 1, L'\0');
                                      GetWindowTextW(hJsonLeftEdit, &wbuf[0], len + 1);
                                      std::string xmlContent = wstringToUtf8(wbuf);
                      
                                      // Convert XML to JSON
                                      std::string jsonResult = convertXMLtoJSON(xmlContent);
                      
                                      if (jsonResult.empty()) {
                                          MessageBoxW(hWnd, L"Failed to convert XML to JSON!", L"Error", MB_OK | MB_ICONERROR);
                                      }
                                      else {
                                          // Convert line endings to Windows format (\r\n) for proper display
                                          std::string result;
                                          for (size_t i = 0; i < jsonResult.length(); i++) {
                                              if (jsonResult[i] == '\n' && (i == 0 || jsonResult[i - 1] != '\r')) {
                                                  result += "\r\n";
                                              }
                                              else if (jsonResult[i] != '\r' || (i + 1 < jsonResult.length() && jsonResult[i + 1] != '\n')) {
                                                  result += jsonResult[i];
                                              }
                                              else {
                                                  result += jsonResult[i];
                                              }
                                          }
                      
                                          // Display JSON in right editor
                                          SetWindowTextW(hJsonRightEdit, utf8ToWstring(result).c_str());
                                      }
                                  }
                                  break;
                      
                                  case ID_BTN_SAVE_JSON:
                                  {
                                      // Check if there's JSON to save
                                      int len = GetWindowTextLengthW(hJsonRightEdit);
                                      if (len == 0) {
                                          MessageBoxW(hWnd, L"No JSON content to save!", L"Error", MB_OK | MB_ICONERROR);
                                          break;
                                      }
                      
                                      OPENFILENAMEW ofn{};
                                      wchar_t fileName[MAX_PATH] = L"output.json";
                      
                                      ofn.lStructSize = sizeof(ofn);
                                      ofn.hwndOwner = hWnd;
                                      ofn.lpstrFilter = L"JSON Files\0*.json\0All Files\0*.*\0";
                                      ofn.lpstrFile = fileName;
                                      ofn.nMaxFile = MAX_PATH;
                                      ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST;
                      
                                      if (!lastDirectory.empty())
                                          ofn.lpstrInitialDir = lastDirectory.c_str();
                      
                                      if (GetSaveFileNameW(&ofn)) {
                                          std::wstring fileNameStr(fileName);
                      
                                          if (fileNameStr.find(L".json") == std::wstring::npos) {
                                              fileNameStr += L".json";
                                          }
                      
                                          lastDirectory = fileNameStr.substr(0, fileNameStr.find_last_of(L"\\/"));
                      
                                          std::wstring wbuf(len + 1, L'\0');
                                          GetWindowTextW(hJsonRightEdit, &wbuf[0], len + 1);
                      
                                          std::ofstream out(wstringToUtf8(fileNameStr));
                                          out << wstringToUtf8(wbuf);
                                          out.close();
                      
                                          MessageBoxW(hWnd, L"JSON Saved Successfully!", L"Done", MB_OK);
                                      }
                                  }
                                  break;
                      
                                 

            }

        }

        break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        return 0;

    }




  
