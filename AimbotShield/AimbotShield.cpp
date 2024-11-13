#include <windows.h>
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
const int MAX_SPEED_THRESHOLD = 30;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_INPUT) {
        UINT dwSize;
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
        LPBYTE lpb = new BYTE[dwSize];
        if (lpb == NULL) return 0;

        if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) == dwSize) {
            RAWINPUT* raw = (RAWINPUT*)lpb;
            if (raw->header.dwType == RIM_TYPEMOUSE) {
                int dx = raw->data.mouse.lLastX;
                int dy = raw->data.mouse.lLastY;
                DWORD currentTime = GetTickCount64();

                // 이동 거리 계산 (피타고라스 정리)
                double distance = std::sqrt(dx * dx + dy * dy);

                mouseMovements.push_back({ dx, dy, currentTime });
                if (mouseMovements.size() > MAX_MOVEMENTS) {
                    mouseMovements.pop_front();
                }

                printf("현재 x,y 이동 거리 : ( %d, %d ), 대각선 거리 : %.2f\n", dx, dy, distance);
                if (distance > MAX_SPEED_THRESHOLD) {
                    std::cout << "비정상적인 마우스 이동 감지! Aimbot 의심\n";
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

    // Raw Input 초기화
    RAWINPUTDEVICE rid;
    rid.usUsagePage = 0x01;
    rid.usUsage = 0x02;
    rid.dwFlags = RIDEV_INPUTSINK;
    rid.hwndTarget = hwnd;  // 윈도우 핸들로 설정

    if (!RegisterRawInputDevices(&rid, 1, sizeof(rid))) {
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
