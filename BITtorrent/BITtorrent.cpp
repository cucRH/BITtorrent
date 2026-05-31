// BITtorrent.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "BITtorrent.h"
#include "UIManager.h"
#include "TorrentCore.h"
#include "BitTorrentClient.h"
#include "Logger.h"
#include <commctrl.h>
#include <stdexcept>

#define MAX_LOADSTRING 100
#define TIMER_UPDATE 1001
#define TIMER_INTERVAL 500

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
UIManager* g_pUIManager = nullptr;
BitTorrentClient* g_pBitTorrentClient = nullptr;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    try {
        // Инициализация common controls
        INITCOMMONCONTROLSEX icc;
        icc.dwSize = sizeof(icc);
        icc.dwICC = ICC_LISTVIEW_CLASSES | ICC_PROGRESS_CLASS | ICC_BAR_CLASSES;
        InitCommonControlsEx(&icc);

        // Инициализация глобальных строк
        wcscpy_s(szTitle, MAX_LOADSTRING, L"BitTorrent Client");
        wcscpy_s(szWindowClass, MAX_LOADSTRING, L"BitTorrentWindow");
        MyRegisterClass(hInstance);

        // Выполнить инициализацию приложения:
        if (!InitInstance (hInstance, nCmdShow))
        {
            MessageBoxW(nullptr, L"Failed to initialize application", L"Error", MB_OK | MB_ICONERROR);
            return FALSE;
        }

        HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BITTORRENT));

        MSG msg;

        // Цикл основного сообщения:
        while (GetMessage(&msg, nullptr, 0, 0))
        {
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        return (int) msg.wParam;
    } catch (const std::exception& e) {
        MessageBoxA(nullptr, e.what(), "Error", MB_OK | MB_ICONERROR);
        return FALSE;
    } catch (...) {
        MessageBoxA(nullptr, "Unknown error", "Error", MB_OK | MB_ICONERROR);
        return FALSE;
    }
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BITTORRENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_BITTORRENT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   // Создаем и инициализируем BitTorrent Client
   g_pBitTorrentClient = new BitTorrentClient();

   // Инициализация может не быть критична для UI - продолжаем даже если не удалась
   g_pBitTorrentClient->Initialize("./BitTorrent_Data/config.ini");

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 800, 520, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      DWORD error = GetLastError();
      wchar_t errorMsg[256];
      swprintf_s(errorMsg, sizeof(errorMsg)/sizeof(wchar_t), L"Failed to create window. Error: %d", error);
      MessageBoxW(nullptr, errorMsg, L"Error", MB_OK | MB_ICONERROR);
      return FALSE;
   }

   // Создаем UI Manager
   g_pUIManager = new UIManager(hWnd);
   g_pUIManager->Initialize();

   // Установим таймер для обновления UI
   SetTimer(hWnd, TIMER_UPDATE, TIMER_INTERVAL, nullptr);

   // Логируем успешный старт
   Logger::GetInstance().Info("Application started successfully", "Main");

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                if (g_pUIManager) {
                    g_pUIManager->OnCommand(wmId, hWnd);
                }
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            BeginPaint(hWnd, &ps);
            if (g_pUIManager) {
                g_pUIManager->OnPaint(ps.hdc, ps.rcPaint);
            }
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_SIZE:
        {
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);
            if (g_pUIManager) {
                g_pUIManager->OnSize(width, height);
            }
        }
        break;
    case WM_TIMER:
        if (wParam == TIMER_UPDATE) {
            if (g_pUIManager) {
                g_pUIManager->UpdateUI();
            }
        }
        break;
    case WM_USER + 1:
        // Обновляем UI после загрузки торрента
        if (g_pUIManager) {
            g_pUIManager->UpdateUI();
        }
        break;
    case WM_DESTROY:
        if (g_pUIManager) {
            delete g_pUIManager;
            g_pUIManager = nullptr;
        }
        if (g_pBitTorrentClient) {
            g_pBitTorrentClient->Shutdown();
            delete g_pBitTorrentClient;
            g_pBitTorrentClient = nullptr;
        }
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
