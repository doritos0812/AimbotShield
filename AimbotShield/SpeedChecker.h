#ifndef SPEED_CHECKER_H
#define SPEED_CHECKER_H

#include <windows.h>

void ProcessRawInput(LPARAM lParam, HWND hWnd);
void HandleMouseMovement(int dx, int dy, DWORD currentTime, HWND hWnd);
void CalculateAndCheckSpeed(int dx, int dy, DWORD currentTime, HWND hWnd);
void CheckConsistentMovement(int dx, int dy, HWND hWnd);
void CheckLinearMovement(int dx, int dy, HWND hWnd);

#endif // SPEED_CHECKER_H
