#include <windows.h>
#include "log.h"
#include "state.h"
#include "../thirdparty/include/MinHook.h"

struct D3DPRESENT_PARAMETERS_MIN
{
    UINT BackBufferWidth;
    UINT BackBufferHeight;
    UINT BackBufferFormat;
    UINT BackBufferCount;
    UINT MultiSampleType;
    UINT MultiSampleQuality;
    UINT SwapEffect;
    HWND hDeviceWindow;
    BOOL Windowed;
    BOOL EnableAutoDepthStencil;
    UINT AutoDepthStencilFormat;
    DWORD Flags;
    UINT FullScreen_RefreshRateInHz;
    UINT PresentationInterval;
};

using Reset_t = HRESULT (WINAPI*)(void* self, D3DPRESENT_PARAMETERS_MIN* pp);
static Reset_t real_Reset = nullptr;

static HRESULT WINAPI hk_Reset(void* self, D3DPRESENT_PARAMETERS_MIN* pp)
{
    if (pp && pp->Windowed && g_state.forceInternal720)
    {
        pp->BackBufferWidth  = g_state.forcedW;
        pp->BackBufferHeight = g_state.forcedH;
        Log("[days-fix] Reset: forcing internal BB to %ux%u (windowed)", g_state.forcedW, g_state.forcedH);
    }

    HRESULT hr = real_Reset(self, pp);

    // refrescar estado (por si acaso)
    if (SUCCEEDED(hr) && pp)
    {
        g_state.hwnd = pp->hDeviceWindow;
        g_state.bbW  = pp->BackBufferWidth;
        g_state.bbH  = pp->BackBufferHeight;
        Log("[days-fix] Reset OK: BB=%ux%u Windowed=%d", (unsigned)g_state.bbW, (unsigned)g_state.bbH, (int)pp->Windowed);
    }

    return hr;
}

void HookDeviceReset(void* d3dDevice)
{
    if (!d3dDevice) return;
    if (real_Reset) return;

    void** vtbl = *(void***)(d3dDevice);

    // índice típico IDirect3DDevice9::Reset = 16 (0-based)
    void* target = vtbl[16];

    if (MH_CreateHook(target, &hk_Reset, (void**)&real_Reset) == MH_OK)
    {
        MH_EnableHook(target);
        Log("[days-fix] Hooked Device::Reset target=%p hk=%p real=%p", target, (void*)&hk_Reset, (void*)real_Reset);
    }
    else
    {
        Log("[days-fix] ERROR: MH_CreateHook(Reset) failed");
    }
}
