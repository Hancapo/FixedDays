#include "window.h"

void SetClientSizeCentered(HWND hwnd, int clientW, int clientH)
{
    if (!hwnd) return;

    RECT rc{ 0, 0, clientW, clientH };

    DWORD style   = (DWORD)GetWindowLongPtrA(hwnd, GWL_STYLE);
    DWORD exStyle = (DWORD)GetWindowLongPtrA(hwnd, GWL_EXSTYLE);

    AdjustWindowRectEx(&rc, style, FALSE, exStyle);

    int winW = rc.right - rc.left;
    int winH = rc.bottom - rc.top;

    HMONITOR mon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{ sizeof(mi) };
    GetMonitorInfoA(mon, &mi);

    int mx = mi.rcWork.left;
    int my = mi.rcWork.top;
    int mw = mi.rcWork.right  - mi.rcWork.left;
    int mh = mi.rcWork.bottom - mi.rcWork.top;

    int x = mx + (mw - winW) / 2;
    int y = my + (mh - winH) / 2;

    SetWindowPos(hwnd, nullptr, x, y, winW, winH,
        SWP_NOZORDER | SWP_NOACTIVATE);
}
