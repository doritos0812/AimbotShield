#ifndef AIMBOT_WATCHER_H
#define AIMBOT_WATCHER_H

#include <windows.h>

void ProcessRawInput(LPARAM lParam, HWND hWnd);
void HandleMouseMovement(int dx, int dy, DWORD currentTime, HWND hWnd);
void CalculateAndCheckSpeed(int dx, int dy, DWORD currentTime, HWND hWnd);
void CheckConsistentMovement(int dx, int dy, HWND hWnd);
void CheckLinearMovement(int dx, int dy, HWND hWnd);

#endif // AIMBOT_WATCHER_H
