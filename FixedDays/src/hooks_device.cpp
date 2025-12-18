#include <windows.h>
#include "log.h"
#include "state.h"
#include "../thirdparty/include/MinHook.h"
#include "window.h"
#include "log.h"


using Present_t = HRESULT (WINAPI*)(void* self, const RECT* src, const RECT* dst, HWND hOverride, const void* dirty);
static Present_t real_Present = nullptr;

static HRESULT WINAPI hk_Present(void* self, const RECT* src, const RECT* dst, HWND hOverride, const void* dirty)
{
    if (!g_state.hwnd || g_state.bbW == 0 || g_state.bbH == 0 || !real_Present)
        return real_Present(self, src, dst, hOverride, dirty);

    RECT cr{};
    if (!GetClientRect(g_state.hwnd, &cr))
        return real_Present(self, src, dst, hOverride, dirty);
    
    static bool pressed7 = false, pressed8 = false, pressed9 = false;

    auto edge = [](int vk, bool& prev) -> bool {
        SHORT s = GetAsyncKeyState(vk);
        bool down = (s & 0x8000) != 0;
        bool fired = down && !prev;
        prev = down;
        return fired;
    };

    if (edge(VK_F7, pressed7)) { Log("[days-fix] Hotkey F7 -> 1920x1080"); SetClientSizeCentered(g_state.hwnd, 1920, 1080); }
    if (edge(VK_F8, pressed8)) { Log("[days-fix] Hotkey F8 -> 2560x1440"); SetClientSizeCentered(g_state.hwnd, 2560, 1440); }
    if (edge(VK_F9, pressed9)) { Log("[days-fix] Hotkey F9 -> 3840x2160"); SetClientSizeCentered(g_state.hwnd, 3840, 2160); }


    int cw = cr.right - cr.left;
    int ch = cr.bottom - cr.top;
    if (cw <= 0 || ch <= 0)
        return real_Present(self, src, dst, hOverride, dirty);

    double sx = (double)cw / (double)g_state.bbW;
    double sy = (double)ch / (double)g_state.bbH;
    double s = (sx < sy) ? sx : sy;

    int dw = (int)(g_state.bbW * s + 0.5);
    int dh = (int)(g_state.bbH * s + 0.5);

    int dx = (cw - dw) / 2;
    int dy = (ch - dh) / 2;

    RECT dst2{};
    dst2.left = dx;
    dst2.top = dy;
    dst2.right = dx + dw;
    dst2.bottom = dy + dh;

    g_state.lastDst = dst2;

    return real_Present(self, nullptr, &dst2, nullptr, dirty);
}

void HookDevicePresent(void* d3dDevice)
{
    if (!d3dDevice) return;

    void** vtbl = *(void***)(d3dDevice);

    void* target = vtbl[17];

    if (real_Present) return;

    if (MH_CreateHook(target, &hk_Present, (void**)&real_Present) == MH_OK)
    {
        MH_EnableHook(target);
        Log("[days-fix] Hooked Device::Present target=%p hk=%p real=%p",
            target, (void*)&hk_Present, (void*)real_Present);
    }
    else
    {
        Log("[days-fix] ERROR: MH_CreateHook(Present) failed");
    }
}
