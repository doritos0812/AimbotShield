#include "App.h"
#include "InputHandler.h"
#include "AimbotWatcher.h"
#include <windows.h>
#include <iostream>

void InitializeApp(WNDCLASS& wc, HWND& hwnd) {
    const wchar_t* CLASS_NAME = L"AimbotShield";
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClass(&wc)) {
        std::cerr << "������ Ŭ���� ��� ����\n";
        exit(-1);
    }

    hwnd = CreateWindowEx(0, CLASS_NAME, L"Aimbot Shield", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    if (hwnd == NULL) {
        std::cerr << "������ ���� ����\n";
        exit(-1);
    }

    // Raw Input �ʱ�ȭ
    InitializeRawInput(hwnd);
}

void run_app() {
    WNDCLASS wc = {};
    HWND hwnd;
    InitializeApp(wc, hwnd);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
