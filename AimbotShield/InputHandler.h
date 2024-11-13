#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <windows.h>
#include <deque>

struct MouseMovement {
    int dx;
    int dy;
    DWORD timestamp;
};

void InitializeRawInput(HWND hwnd);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void ToggleDetection();

extern bool isDetectionActive;
extern std::deque<MouseMovement> mouseMovements;

#endif // INPUT_HANDLER_H
