#include "AimbotAlert.h"
#include <iostream>

void ShowWarningPopup(HWND hWnd) {
    std::cout << "���������� ���콺 �ӵ� ����! Aimbot �ǽ�\n";
    MessageBox(hWnd, L"���������� ���콺 �ӵ� ����! Aimbot �ǽ�", L"���", MB_OK | MB_ICONWARNING);
}
