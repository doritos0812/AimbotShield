#include "AimbotAlart.h"
#include <iostream>
#include <string>

std::wstring ConvertToWideChar(const char* message) {
    if (message == nullptr) return L"";

    // ��ȯ�� �ʿ��� ���� ũ�� ���
    int bufferSize = MultiByteToWideChar(CP_ACP, 0, message, -1, NULL, 0);

    // ��ȯ ����
    std::wstring wideMessage(bufferSize, 0);
    MultiByteToWideChar(CP_ACP, 0, message, -1, &wideMessage[0], bufferSize);

    return wideMessage;
}

void ShowWarningPopup(HWND hWnd, const char* message) {
    std::cout << message << " Aimbot �ǽ�\n";
    std::wstring wideMessage = ConvertToWideChar(message);
    MessageBox(hWnd, wideMessage.c_str(), L"���", MB_OK | MB_ICONWARNING);
}
