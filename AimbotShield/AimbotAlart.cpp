#include "AimbotAlart.h"
#include <iostream>
#include <string>

std::wstring ConvertToWideChar(const char* message) {
    if (message == nullptr) return L"";

    // 변환에 필요한 버퍼 크기 계산
    int bufferSize = MultiByteToWideChar(CP_ACP, 0, message, -1, NULL, 0);

    // 변환 수행
    std::wstring wideMessage(bufferSize, 0);
    MultiByteToWideChar(CP_ACP, 0, message, -1, &wideMessage[0], bufferSize);

    return wideMessage;
}

void ShowWarningPopup(HWND hWnd, const char* message) {
    std::cout << message << " Aimbot 의심\n";
    std::wstring wideMessage = ConvertToWideChar(message);
    MessageBox(hWnd, wideMessage.c_str(), L"경고", MB_OK | MB_ICONWARNING);
}
