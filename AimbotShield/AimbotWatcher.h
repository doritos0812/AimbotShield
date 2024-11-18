#ifndef AIMBOT_WATCHER_H
#define AIMBOT_WATCHER_H

#include <windows.h>

// Process
void ProcessRawInput(LPARAM lParam, HWND hWnd);
void HandleMouseMovement(int dx, int dy, DWORD currentTime, HWND hWnd);

// Utils
double CalculateAngle(int dx, int dy);
bool IsAngleCloseToFixedDirections(double angle);

// Checking Logics
void CheckMouseSpeed(int dx, int dy, DWORD currentTime, HWND hWnd);
void CheckConsistentMovement(int dx, int dy, HWND hWnd);
void CheckLinearMovement(int dx, int dy, HWND hWnd);
void ChecktLimitedDirection(int dx, int dy, HWND hWnd);

#endif // AIMBOT_WATCHER_H
