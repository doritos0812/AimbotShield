#include "SpeedChecker.h"
#include "InputHandler.h"
#include "AimbotAlert.h"
#include <iostream>
#include <cmath>

const size_t MAX_MOVEMENTS = 100;
const int MAX_SPEED_THRESHOLD = 6000;

void ProcessRawInput(LPARAM lParam, HWND hWnd) {
    UINT dwSize;
    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
    LPBYTE lpb = new BYTE[dwSize];
    if (lpb == NULL) return;

    if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) == dwSize) {
        RAWINPUT* raw = (RAWINPUT*)lpb;

        if (raw->header.dwType == RIM_TYPEMOUSE && isDetectionActive) {
            int dx = raw->data.mouse.lLastX;
            int dy = raw->data.mouse.lLastY;
            DWORD currentTime = GetTickCount64();

            HandleMouseMovement(dx, dy, currentTime, hWnd);
        }
        else if (raw->header.dwType == RIM_TYPEKEYBOARD) {
            if (raw->data.keyboard.VKey == VK_ESCAPE && raw->data.keyboard.Flags == RI_KEY_MAKE) {
                ToggleDetection();
            }
        }
    }
    delete[] lpb;
}

void HandleMouseMovement(int dx, int dy, DWORD currentTime, HWND hWnd) {
    double distance = std::sqrt(dx * dx + dy * dy);

    if (!mouseMovements.empty()) {
        DWORD previousTime = mouseMovements.back().timestamp;
        CalculateAndCheckSpeed(dx, dy, currentTime, hWnd);
    }

    mouseMovements.push_back({ dx, dy, currentTime });
    if (mouseMovements.size() > MAX_MOVEMENTS) {
        mouseMovements.pop_front();
    }
}

void CalculateAndCheckSpeed(int dx, int dy, DWORD currentTime, HWND hWnd) {
    DWORD previousTime = mouseMovements.back().timestamp;
    double deltaTime = (currentTime - previousTime) / 1000.0;  // Convert ms to seconds

    if (deltaTime > 0) {
        double distance = std::sqrt(dx * dx + dy * dy);
        double speed = distance / deltaTime;
        std::cout << "현재 x,y 이동 거리 : ( " << dx << ", " << dy << " ), 대각선 거리 : "
            << distance << ", 속도: " << speed << " 픽셀/초\n";

        CheckAbnormalSpeed(speed, hWnd);
    }
}

void CheckAbnormalSpeed(double speed, HWND hWnd) {
    if (speed > MAX_SPEED_THRESHOLD) {
        ShowWarningPopup(hWnd);
    }
}
