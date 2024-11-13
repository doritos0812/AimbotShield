﻿#include <windows.h>
#include <iostream>
#include <deque>
#include <cmath>

struct MouseMovement {
    int dx;
    int dy;
    DWORD timestamp;
};

std::deque<MouseMovement> mouseMovements;
const size_t MAX_MOVEMENTS = 100;
const int MAX_SPEED_THRESHOLD = 5000;

// Raw Input 감지 활성화/비활성화
bool isDetectionActive = true;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_INPUT) {
        UINT dwSize;
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
        LPBYTE lpb = new BYTE[dwSize];
        if (lpb == NULL) return 0;

        if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) == dwSize) {
            RAWINPUT* raw = (RAWINPUT*)lpb;

            // 마우스 이벤트 감지
            if (raw->header.dwType == RIM_TYPEMOUSE && isDetectionActive) {
                int dx = raw->data.mouse.lLastX;
                int dy = raw->data.mouse.lLastY;
                DWORD currentTime = GetTickCount64();

                // 이동 거리 계산 (피타고라스 정리)
                double distance = std::sqrt(dx * dx + dy * dy);

                // 이전 이동 기록이 있을 때만 속도 계산
                if (!mouseMovements.empty()) {
                    DWORD previousTime = mouseMovements.back().timestamp;
                    double deltaTime = (currentTime - previousTime) / 1000.0; // ms -> s
                    if (deltaTime > 0) {
                        double speed = distance / deltaTime;

                        printf("현재 x,y 이동 거리 : ( %d, %d ), 대각선 거리 : %.2f, 속도: %.2f 픽셀/초\n", dx, dy, distance, speed);

                        if (speed > MAX_SPEED_THRESHOLD) {
                            std::cout << "비정상적인 마우스 속도 감지! Aimbot 의심\n";
                            MessageBox(hWnd, L"비정상적인 마우스 속도 감지! Aimbot 의심", L"경고", MB_OK | MB_ICONWARNING);
                        }
                    }
                }

                mouseMovements.push_back({ dx, dy, currentTime });
                if (mouseMovements.size() > MAX_MOVEMENTS) {
                    mouseMovements.pop_front();
                }
            }
            // 키보드 이벤트 감지 (ESC 키 확인)
            else if (raw->header.dwType == RIM_TYPEKEYBOARD) {
                // ESC 키 눌림 감지
                if (raw->data.keyboard.VKey == VK_ESCAPE && raw->data.keyboard.Flags == RI_KEY_MAKE) {
                    isDetectionActive = !isDetectionActive;
                    if (isDetectionActive) {
                        std::cout << "Raw Input 감지 재개\n";
                    }
                    else {
                        std::cout << "Raw Input 감지 중지\n";
                    }
                }
            }
        }
        delete[] lpb;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

int main() {
    const wchar_t* CLASS_NAME = L"AimbotShield";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClass(&wc)) {
        std::cerr << "윈도우 클래스 등록 실패\n";
        return -1;
    }

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"Aimbot Shield", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    if (hwnd == NULL) {
        std::cerr << "윈도우 생성 실패\n";
        return -1;
    }

    // 마우스 및 키보드 Raw Input 초기화
    RAWINPUTDEVICE rid[2];

    // 마우스
    rid[0].usUsagePage = 0x01;
    rid[0].usUsage = 0x02;
    rid[0].dwFlags = RIDEV_INPUTSINK;
    rid[0].hwndTarget = hwnd;

    // 키보드
    rid[1].usUsagePage = 0x01;
    rid[1].usUsage = 0x06;
    rid[1].dwFlags = RIDEV_INPUTSINK;
    rid[1].hwndTarget = hwnd;

    if (!RegisterRawInputDevices(rid, 2, sizeof(rid[0]))) {
        std::cerr << "Raw Input 초기화 실패\n";
        return -1;
    }

    // 메시지 루프
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
