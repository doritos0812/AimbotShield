#include "AimbotWatcher.h"
#include "InputHandler.h"
#include "AimbotAlart.h"
#include <iostream>
#include <numeric>

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>


const size_t MAX_MOVEMENTS = 100;

const size_t CONSISTENT_THRESHOLD = 10;
const size_t LINEAR_THRESHOLD = 20;
const int MAX_SPEED_THRESHOLD = 6000;
const int MIN_MOVEMENT_THRESHOLD = 20;
const double DIRECTION_THRESHOLD = 0.85;

size_t consistentCount = 0;
size_t linearCount = 0;
size_t fixedDirectionCount = 0;
size_t checkDirectionCount = 0;

// 8방향의 각도 (라디안 단위)
const double ANGLE_THRESHOLDS[] = { -M_PI, -3 * M_PI_4, -M_PI_2, -M_PI_4, 0, M_PI_4, M_PI_2, 3 * M_PI_4, M_PI};


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
            CheckMouseSpeed(dx, dy, currentTime, hWnd);
            CheckConsistentMovement(dx, dy, hWnd);
            CheckLinearMovement(dx, dy, hWnd);
            ChecktLimitedDirection(dx, dy, hWnd);
        }
    }

    mouseMovements.push_back({ dx, dy, currentTime });
    if (mouseMovements.size() > MAX_MOVEMENTS) {
        mouseMovements.pop_front();
    }
}

void CheckMouseSpeed(int dx, int dy, DWORD currentTime, HWND hWnd) {
    DWORD previousTime = mouseMovements.back().timestamp;
    double deltaTime = (currentTime - previousTime) / 1000.0;  // Convert ms to seconds

    if (deltaTime <= 0) return;
    double distance = std::sqrt(dx * dx + dy * dy);
    double speed = distance / deltaTime;
    std::cout << "현재 x,y 이동 거리 : ( " << dx << ", " << dy << " ), 속도: " << speed << " 픽셀/초\n";

    const char* errMessage = "비정상적인 마우스 속도 감지!";
    if (speed > MAX_SPEED_THRESHOLD) ShowWarningPopup(hWnd, errMessage);
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

    const char* errMessage = "비정상적인 마우스 이동 간격 감지!";
    if (consistentCount >= CONSISTENT_THRESHOLD) ShowWarningPopup(hWnd, errMessage);
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

    const char* errMessage = "비정상적인 마우스 이동 기울기 감지!";
    if (linearCount >= LINEAR_THRESHOLD) ShowWarningPopup(hWnd, errMessage);
}

double CalculateAngle(int dx, int dy) {
    return std::atan2(dy, dx); // -π ~ π
}

bool IsAngleCloseToFixedDirections(double angle) {
    for (double threshold : ANGLE_THRESHOLDS) {
        if (std::abs(angle - threshold) < 0.2) return true; // 오차 범위 설정
    }
    return false;
}

void ChecktLimitedDirection(int dx, int dy, HWND hWnd) {
    checkDirectionCount++;

    double angle = CalculateAngle(dx, dy);
    if (IsAngleCloseToFixedDirections(angle)) fixedDirectionCount++;

    if (checkDirectionCount >= 300) {
        // 최근 300번 중, 일정 비율 이상이 8방향으로 움직이면 Aimbot 의심
        double fixedDirectionRatio = static_cast<double>(fixedDirectionCount) / checkDirectionCount;
        std::cout << "8방향 일관성 이동 횟수 : " << fixedDirectionCount << "  전체 이동 횟수 : " << checkDirectionCount << "\n";
        std::cout << "일관성 비율 : " << fixedDirectionRatio << "\n";

        // Count 초기화
        checkDirectionCount = 0;
        fixedDirectionCount = 0;

        const char* errMessage = "비정상적인 마우스 이동 방향 일관성 감지!";
        if (fixedDirectionRatio > DIRECTION_THRESHOLD) ShowWarningPopup(hWnd, errMessage);
    }
}