#include <windows.h>
#include "log.h"
#include "state.h"
#include "../thirdparty/include/MinHook.h"


using GetCursorPos_t = BOOL (WINAPI*)(LPPOINT);
static GetCursorPos_t real_GetCursorPos = nullptr;

static BOOL WINAPI hk_GetCursorPos(LPPOINT p)
{
    BOOL ok = real_GetCursorPos(p);
    if (!ok || !p) return ok;

    if (!g_state.hwnd || g_state.bbW == 0 || g_state.bbH == 0)
        return ok;

    POINT c = *p;
    if (!ScreenToClient(g_state.hwnd, &c))
        return ok;

    int dx = g_state.lastDst.left;
    int dy = g_state.lastDst.top;
    int dw = g_state.lastDst.right  - g_state.lastDst.left;
    int dh = g_state.lastDst.bottom - g_state.lastDst.top;
    if (dw <= 0 || dh <= 0) return ok;

    int x = c.x - dx;
    int y = c.y - dy;

    if (x < 0) x = 0; if (x > dw) x = dw;
    if (y < 0) y = 0; if (y > dh) y = dh;

    double nx = (double)x / (double)dw;
    double ny = (double)y / (double)dh;

    int mx = (int)(nx * (double)g_state.bbW + 0.5);
    int my = (int)(ny * (double)g_state.bbH + 0.5);

    POINT out{ mx, my };
    ClientToScreen(g_state.hwnd, &out);
    *p = out;

    return ok;
}

void HookUser32Cursor()
{
    HMODULE user32 = GetModuleHandleA("user32.dll");
    if (!user32) user32 = LoadLibraryA("user32.dll");
    if (!user32) { Log("[days-fix] ERROR: user32 not loaded"); return; }

    void* target = (void*)GetProcAddress(user32, "GetCursorPos");
    if (!target) { Log("[days-fix] ERROR: GetProcAddress(GetCursorPos)"); return; }

    if (MH_CreateHook(target, &hk_GetCursorPos, (void**)&real_GetCursorPos) == MH_OK)
    {
        MH_EnableHook(target);
        Log("[days-fix] Hooked USER32!GetCursorPos target=%p hk=%p real=%p",
            target, (void*)&hk_GetCursorPos, (void*)real_GetCursorPos);
    }
    else
    {
        Log("[days-fix] ERROR: MH_CreateHook(GetCursorPos) failed");
    }
}
