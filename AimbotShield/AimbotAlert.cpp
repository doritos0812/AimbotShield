#include "AimbotAlert.h"
#include <iostream>

void ShowWarningPopup(HWND hWnd) {
    std::cout << "비정상적인 마우스 속도 감지! Aimbot 의심\n";
    MessageBox(hWnd, L"비정상적인 마우스 속도 감지! Aimbot 의심", L"경고", MB_OK | MB_ICONWARNING);
}
