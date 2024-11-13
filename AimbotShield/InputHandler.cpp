#include "InputHandler.h"
#include "SpeedChecker.h"
#include "AimbotAlert.h"
#include <iostream>

std::deque<MouseMovement> mouseMovements;
bool isDetectionActive = true;

void InitializeRawInput(HWND hwnd) {
    RAWINPUTDEVICE rid[2];

    rid[0].usUsagePage = 0x01;
    rid[0].usUsage = 0x02;
    rid[0].dwFlags = RIDEV_INPUTSINK;
    rid[0].hwndTarget = hwnd;

    rid[1].usUsagePage = 0x01;
    rid[1].usUsage = 0x06;
    rid[1].dwFlags = RIDEV_INPUTSINK;
    rid[1].hwndTarget = hwnd;

    if (!RegisterRawInputDevices(rid, 2, sizeof(rid[0]))) {
        std::cerr << "Raw Input 초기화 실패\n";
    }
}

void ToggleDetection() {
    isDetectionActive = !isDetectionActive;
    std::cout << (isDetectionActive ? "Raw Input 감지 재개\n" : "Raw Input 감지 중지\n");
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_INPUT) {
        ProcessRawInput(lParam, hWnd);
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}
