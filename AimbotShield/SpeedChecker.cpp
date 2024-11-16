#include "SpeedChecker.h"
#include "InputHandler.h"
#include "AimbotAlert.h"
#include <iostream>
#include <cmath>

const size_t MAX_MOVEMENTS = 100;
const size_t CONSISTENT_THRESHOLD = 10;
const size_t LINEAR_THRESHOLD = 20;
const int MAX_SPEED_THRESHOLD = 6000;
const int MIN_MOVEMENT_THRESHOLD = 20;
size_t consistentCount = 0;
size_t linearCount = 0;

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
        if (std::abs(dx) > MIN_MOVEMENT_THRESHOLD || std::abs(dy) > MIN_MOVEMENT_THRESHOLD) {
            CalculateAndCheckSpeed(dx, dy, currentTime, hWnd);
            CheckConsistentMovement(dx, dy, hWnd);
            CheckLinearMovement(dx, dy, hWnd);
        }
    }

    mouseMovements.push_back({ dx, dy, currentTime });
    if (mouseMovements.size() > MAX_MOVEMENTS) {
        mouseMovements.pop_front();
    }
}

void CalculateAndCheckSpeed(int dx, int dy, DWORD currentTime, HWND hWnd) {
    DWORD previousTime = mouseMovements.back().timestamp;
    double deltaTime = (currentTime - previousTime) / 1000.0;  // Convert ms to seconds

    if (deltaTime <= 0) return;
    double distance = std::sqrt(dx * dx + dy * dy);
    double speed = distance / deltaTime;
    std::cout << "현재 x,y 이동 거리 : ( " << dx << ", " << dy << " ), 속도: " << speed << " 픽셀/초\n";

    if (speed > MAX_SPEED_THRESHOLD) ShowWarningPopup(hWnd);
}

void CheckConsistentMovement(int dx, int dy, HWND hWnd) {
    int previousX = mouseMovements.back().dx;
    int previousY = mouseMovements.back().dy;

    double previousDistance = std::sqrt(previousX * previousX + previousY * previousY);
    double currentdistance = std::sqrt(dx * dx + dy * dy);
    std::cout << "절대 이동 거리 : " << currentdistance << "\n";

    if (currentdistance == previousDistance) {
        consistentCount++;
    }
    else {
        consistentCount = 0;
    }

    if (consistentCount >= CONSISTENT_THRESHOLD) ShowWarningPopup(hWnd);
}

void CheckLinearMovement(int dx, int dy, HWND hWnd) {
    int previousX = mouseMovements.back().dx;
    int previousY = mouseMovements.back().dy;

    // 분모가 0이면 무한대 기울기 처리
    double prevGradient = (previousX != 0) ? static_cast<double>(previousY) / previousX : DBL_MAX;
    double currentGradient = (dx != 0) ? static_cast<double>(dy) / dx : DBL_MAX;

    // 방향성 검사: (dx, dy)와 (previousX, previousY)의 부호 비교
    bool isSameDirection = (dx * previousX >= 0) && (dy * previousY >= 0);

    std::cout << "기울기 : " << currentGradient << "\n";

    if (isSameDirection && currentGradient == prevGradient) {
        linearCount++;
    }
    else {
        linearCount = 0;
    }

    if (linearCount >= LINEAR_THRESHOLD) ShowWarningPopup(hWnd);
}